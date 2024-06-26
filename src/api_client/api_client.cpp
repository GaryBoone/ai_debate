#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <tl/expected.hpp>

#include "../../tests/test_chunk_processor.h"
#include "../../tests/test_request_maker.h"
#include "../util/json_printer.h"
#include "../util/string_cleaner.h"
#include "api_client.h"
#include "api_error.h"
#include "claude_chunk_processor.h"
#include "claude_request_maker.h"
#include "gemini_chunk_processor.h"
#include "gemini_request_maker.h"
#include "gpt_chunk_processor.h"
#include "gpt_request_maker.h"
#include "i_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

// Filter the returned text, removing the SSE lines and type error lines that
// are not data.
template <typename CP, typename RM>
std::string ApiClient<CP, RM>::FilterText(std::string &lines) {
  std::string filtered_line = this->FilterTypeErrorString(lines);
  return this->FilterByLine(filtered_line);
}

// Try to handle the data lines in the response. Return whether the data was
// successfully parsed and the contents saved into _combined_text.
template <typename CP, typename RM>
bool ApiClient<CP, RM>::HandleData(std::string &lines,
                                   const std::string &filtered_line,
                                   bool print) {
  std::regex data_re(R"(^\s*data:\s*)");
  if (std::regex_search(filtered_line, data_re)) {
    auto result = this->stream_handler_.HandleDataLines(filtered_line, print);
    // If the data was handled, stop accumulating lines and return the result.
    if (result.has_value()) {
      lines.clear();
      return result.value();
    }

    // Sometime the OpenAI API returns partial chunks that have to be
    // combined before they can be parsed. If the JSON parsing fails, we
    // can just ignore the error, accumulate the chunks, and try again.
    if (result.error().Type() == APIErrorType::kResponseJsonParse) {
      return true;
    }
    throw APIError(APIErrorType::kUnknown, result.error());
  }
  return false;
}

// Handle errors in the API response. That is, the response is valid JSON but
// contains an error message from the API.
template <typename CP, typename RM>
bool ApiClient<CP, RM>::HandleErrors(const std::string &filtered_line) {
  std::regex error_re(R"(\s*"error":\s*)");
  if (std::regex_search(filtered_line, error_re)) {
    throw APIError(APIErrorType::kApiReturnedError,
                   this->ParseError(filtered_line));
  }
  return true;
}

// Process a line of text from the API response. This function is called for
// each line of the response, and it accumulates the lines until a complete
// chunk is received. It then filters the chunk and passes it to the
// appropriate chunk processor determined by the template parameter.
template <typename CP, typename RM>
bool ApiClient<CP, RM>::ProcessLine(std::string &lines,
                                    const std::string &raw_line, bool print) {

  // Accumulate the chunks until we have a complete one that parses correctly.
  lines.append(raw_line);

  std::string filtered_line = this->FilterText(lines);

  if (std::all_of(filtered_line.begin(), filtered_line.end(),
                  [](unsigned char chr) { return std::isspace(chr); })) {
    return true;
  }

  if (!this->HandleData(lines, filtered_line, print)) {
    return this->HandleErrors(filtered_line);
  }

  return true;
}

// Chat() is the main function for interacting with the API. It sends the
// prompt to the API and returns the response. The response is a string
// containing the text generated by the API.
template <typename CP, typename RM>
tl::expected<Response, APIError>
ApiClient<CP, RM>::CallApi(const std::string &system_prompt,
                           const std::vector<Message> &messages, bool print) {
  std::string lines;
  this->stream_handler_ = ApiStreamHandler<CP>();
  std::string json_str;
  JsonPrinter json_printer;

  try {
    APIRequest request = this->request_maker_.Create(system_prompt, messages);

    cpr::WriteCallback callback = cpr::WriteCallback(
        [this, &json_str, &lines, &json_printer,
         &print](const std::string &raw_line, intptr_t /*userdata*/) {
          auto res = this->ProcessLine(lines, raw_line, false);
          auto text = this->stream_handler_.CombinedText();
          this->stream_handler_.ClearText();
          if (print) {
            json_printer.Print(text);
          }
          json_str += text;
          return res;
        });
    cpr::Response response =
        cpr::Post(request.url, request.header, request.body, callback);

    if (response.status_code != kSuccessCode) {
      return tl::unexpected(APIError(
          APIErrorType::kHttpError,
          "Non-200 response code: " + std::to_string(response.status_code) +
              ": " + response.text));
    }
  } catch (const APIError &e) {
    return tl::unexpected(e);
  } catch (const std::exception &e) {
    return tl::unexpected(APIError(APIErrorType::kUnknown, e.what()));
  }

  // We have all of the text; parse it into JSON and return it.
  try {
    json_str = {std::find_if_not(json_str.begin(), json_str.end(), isspace),
                json_str.end()};
    json_str = CorrectForJsonParsing(json_str);
    json_str = EscapeLinefeedsInStrings(json_str);
    json_str = std::regex_replace(json_str, std::regex("\n"), " ");
    json json_obj = json::parse(json_str);
    return json_obj.get<Response>();
  } catch (const json::parse_error &e) {
    std::cout << "json::parse_error: " << e.what() << std::endl
              << " in: >>>" + json_str + "<<<" << std::flush;
    return tl::unexpected(APIError(APIErrorType::kResponseJsonParse, e.what()));
  }
}

// Remove the type error strings if present.
template <typename CP, typename RM>
std::string
ApiClient<CP, RM>::FilterTypeErrorString(const std::string &raw_lines) {
  std::regex type_error_re(R"("type":\s*"error",)");
  return std::regex_replace(raw_lines, type_error_re, "");
}

// Streamed data can contain lines that are not data lines, but event labels
// for the data lines. This function filters out those lines which in many
// cases are repeated in the data lines anyway.
template <typename CP, typename RM>
std::string ApiClient<CP, RM>::FilterByLine(const std::string &raw_lines) {
  const std::array<std::string, 7> events_to_filter = {
      "event: message_start",       "event: ping",
      "event: content_block_start", "event: content_block_delta",
      "event: content_block_stop",  "event: message_delta",
      "event: message_stop"};

  std::istringstream iss(raw_lines);
  std::ostringstream oss;
  std::string line;

  while (std::getline(iss, line)) {
    line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
    line.erase(std::remove(line.begin(), line.end(), '\n'), line.end());
    if (line.empty()) {
      continue;
    }

    if (std::find(events_to_filter.begin(), events_to_filter.end(), line) ==
        events_to_filter.end()) {
      oss << line << '\n';
    }
  }

  // Check the stream for only whitespace, returning an empty string if so.
  return std::all_of(oss.str().begin(), oss.str().end(),
                     [](unsigned char chr) { return std::isspace(chr); })
             ? ""
             : oss.str();
}

// Parse the given line for error JSON.
template <typename CP, typename RM>
APIError ApiClient<CP, RM>::ParseError(const std::string &error_str) {
  try {
    json chunk_data = json::parse(error_str);

    json error_json = chunk_data.value("error", json::object());
    if (error_json.empty()) {
      return APIError(APIErrorType::kResponseJsonParse,
                      "unable to parse API error; no 'error' field");
    }
    json message = error_json.value("message", json::object());
    if (message.empty()) {
      return APIError(APIErrorType::kResponseJsonParse,
                      "unable to parse API error; no 'message' field");
    }
    return APIError(APIErrorType::kApiReturnedError, message);
  } catch (const json::parse_error &e) {
    return APIError(APIErrorType::kResponseJsonParse, e.what());
  } catch (const json::type_error &e) {
    return APIError(APIErrorType::kResponseJsonParse, e.what());
  }
}

template class ApiClient<GPTChunkProcessor, GPTRequestMaker>;
template class ApiClient<GeminiChunkProcessor, GeminiRequestMaker>;
template class ApiClient<ClaudeChunkProcessor, ClaudeRequestMaker>;
template class ApiClient<TestChunkProcessor, TestRequestMaker>;
