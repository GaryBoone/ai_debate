

#include <cstdio>
#include <iostream>
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

using json = nlohmann::json;

template <typename T>
tl::expected<std::string, APIError>
ApiClient<T>::get_completion(const std::string &prompt, bool print) {
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
    std::string filtered_line = this->_filter_type_error_string(lines);
    filtered_line = this->_filter_by_line(filtered_line);

    // Just whitespace?
    if (std::all_of(filtered_line.begin(), filtered_line.end(),
                    [](unsigned char c) { return std::isspace(c); })) {
      return true;
    }

    // Decide how to process the chunk.
    if (std::regex_search(filtered_line, data_re)) {
      auto result =
          this->_stream_handler.handle_data_lines(filtered_line, print);
      if (result.has_value()) {
        lines.clear();
        return result.value();
      } else {
        if (result.error().type() == APIErrorType::RESPONSE_JSON_PARSE) {
          // Sometime the OpenAI API returns partial chunks that have to be
          // combined before they can be parsed. If the JSON parsing fails, we
          // can just ignore the error, accumulate the chunks, and try again.
          return true;
        } else {
          throw APIError(APIErrorType::UNKNOWN, result.error());
        }
        return false;
      }
    } else if (std::regex_search(filtered_line, error_re)) {
      throw APIError(APIErrorType::API_RETURNED_ERROR,
                     this->_parse_error(filtered_line));
    } else {
      throw APIError(APIErrorType::UNKNOWN,
                     "unknown response: " + filtered_line);
    }
    return true;
  };

  try {
    APIRequest request = this->_request_maker->create(prompt);

    cpr::Response response =
        cpr::Post(request.url, request.header, request.body,
                  cpr::WriteCallback(callback, 0));

    if (response.status_code != 200) {
      return tl::unexpected(APIError(
          APIErrorType::HTTP_ERROR,
          "Non-200 response code: " + std::to_string(response.status_code) +
              ": " + response.text));
    }
  } catch (const APIError &e) {
    return tl::unexpected(e);
  } catch (const std::exception &e) {
    return tl::unexpected(APIError(APIErrorType::UNKNOWN, e.what()));
  }

  return this->_stream_handler.get_combined_text();
}

// Remove the type error strings if present.
template <typename T>
std::string
ApiClient<T>::_filter_type_error_string(const std::string &raw_lines) {
  std::regex type_error_re(R"("type":\s*"error",)");
  return std::regex_replace(raw_lines, type_error_re, "");
}

// Streamed data can contain lines that are not data lines, but event labels for
// the data lines. This function filters out those lines which in many cases are
// repeated in the data lines anyway.
template <typename T>
std::string ApiClient<T>::_filter_by_line(const std::string &raw_lines) {
  const std::string eventsToFilter[] = {
      "event: message_start",       "event: ping",
      "event: content_block_start", "event: content_block_delta",
      "event: content_block_stop",  "event: message_delta",
      "event: message_stop"};

  std::istringstream iss(raw_lines);
  std::ostringstream oss;
  std::string line;

  while (std::getline(iss, line)) {
    while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
      line.pop_back();
    }

    if (std::none_of(
            std::begin(eventsToFilter), std::end(eventsToFilter),
            [&line](const std::string &event) { return line == event; })) {
      oss << line << '\n';
    }
  }

  // Check the stream for only whitespace, returning an empty string if so.
  return std::all_of(oss.str().begin(), oss.str().end(),
                     [](unsigned char c) { return std::isspace(c); })
             ? ""
             : oss.str();
}

// Parse the given line for error JSON.
template <typename T>
APIError ApiClient<T>::_parse_error(const std::string &error_str) {
  try {
    json chunk_data = json::parse(error_str);

    json error_json = chunk_data.value("error", json::object());
    if (error_json.empty()) {
      return APIError(APIErrorType::RESPONSE_JSON_PARSE,
                      "unable to parse API error obj error; no 'error' field");
    }
    json message = error_json.value("message", json::object());
    if (message.empty()) {
      return APIError(APIErrorType::RESPONSE_JSON_PARSE,
                      "unable to parse API error message; no 'message' field");
    }
    return APIError(APIErrorType::API_RETURNED_ERROR, message);
  } catch (const json::parse_error &e) {
    return APIError(APIErrorType::RESPONSE_JSON_PARSE, e.what());
  }
}

template class ApiClient<GPTChunkProcessor>;
template class ApiClient<GeminiChunkProcessor>;
template class ApiClient<ClaudeChunkProcessor>;
