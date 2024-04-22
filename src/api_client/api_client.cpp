#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <tl/expected.hpp>

#include "api_client.h"
#include "api_error.h"
#include "claude_chunk_processor.h"
#include "gemini_chunk_processor.h"
#include "gpt_chunk_processor.h"
#include "i_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

template <typename T>
tl::expected<std::string, APIError>
ApiClient<T>::Chat(const std::string &prompt, bool print) {
  std::string lines;

  // The callback function that processes the response from the API. It should
  // return true to continue processing the response, and false to stop.
  auto callback = [&](std::string raw_line, intptr_t /*userdata*/) {
    std::regex data_re(R"(^\s*data:\s*)");
    std::regex error_re(R"(\s*"error":\s*)"); // It can start with returns.

    // Accumulate the lines until we have a complete chunk.
    lines.append(raw_line);

    // Strip stream lines like ping event and type lines to leave just the data
    // lines.
    std::string filtered_line = this->FilterTypeErrorString(lines);
    filtered_line = this->FilterByLine(filtered_line);

    // Just whitespace?
    if (std::all_of(filtered_line.begin(), filtered_line.end(),
                    [](unsigned char chr) { return std::isspace(chr); })) {
      return true;
    }

    // Decide how to process the chunk.
    if (std::regex_search(filtered_line, data_re)) {
      auto result = this->stream_handler_.HandleDataLines(filtered_line, print);
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

    if (std::regex_search(filtered_line, error_re)) {
      throw APIError(APIErrorType::kApiReturnedError,
                     this->ParseError(filtered_line));
    }

    throw APIError(APIErrorType::kUnknown,
                   "unknown response: " + filtered_line);
  };

  try {
    APIRequest request = this->request_maker_->Create(prompt);

    cpr::Response response =
        cpr::Post(request.url, request.header, request.body,
                  cpr::WriteCallback(callback, 0));

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

  return this->stream_handler_.CombinedText();
}

// Remove the type error strings if present.
template <typename T>
std::string ApiClient<T>::FilterTypeErrorString(const std::string &raw_lines) {
  std::regex type_error_re(R"("type":\s*"error",)");
  return std::regex_replace(raw_lines, type_error_re, "");
}

// Streamed data can contain lines that are not data lines, but event labels for
// the data lines. This function filters out those lines which in many cases are
// repeated in the data lines anyway.
template <typename T>
std::string ApiClient<T>::FilterByLine(const std::string &raw_lines) {
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
template <typename T>
APIError ApiClient<T>::ParseError(const std::string &error_str) {
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

template class ApiClient<GPTChunkProcessor>;
template class ApiClient<GeminiChunkProcessor>;
template class ApiClient<ClaudeChunkProcessor>;
