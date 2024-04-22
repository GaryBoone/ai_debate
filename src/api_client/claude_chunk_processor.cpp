
#include <cpr/cpr.h>
#include <cstdio>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <tl/expected.hpp>

#include "api_error.h"
#include "claude_chunk_processor.h"

using json = nlohmann::json;

const char *TYPE_KEY = "type";
const char *CONTENT_DELTA_TYPE = "content_block_delta";
const char *FINISH_REASON_TYPE = "message_delta";
const char *FINISH_REASON_KEY = "stop_reason";
const char *FINISH_REASON_VALUE = "end_turn";
const char *DELTA_KEY = "delta";
const char *TEXT_KEY = "text";

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
ClaudeChunkProcessor::parse_chunk_data(const std::string &chunk_str,
                                       bool print) {

  const std::string types_to_ignore[] = {"ping", "message_start",
                                         "content_block_start",
                                         "content_block_stop", "message_stop"};

  // print_colored_string(CYAN, "-->%s<--\n", chunk_str.c_str());
  try {
    json chunk_data = json::parse(chunk_str);

    // First, check the type.
    json data_type = chunk_data[TYPE_KEY];
    if (!data_type.is_string()) {
      return tl::unexpected(APIError(APIErrorType::RESPONSE_FIELD_PARSE,
                                     "Type field is not a string."));
    }
    std::string data_type_str = data_type.get<std::string>();
    if (std::find(std::begin(types_to_ignore), std::end(types_to_ignore),
                  data_type_str) != std::end(types_to_ignore)) {
      return true;
    }

    // Check for the finish reason.
    if (data_type_str == FINISH_REASON_TYPE) {
      try {
        std::string finish_reason =
            chunk_data[FINISH_REASON_KEY].get<std::string>();
        if (finish_reason != FINISH_REASON_VALUE) {
          return tl::unexpected(
              APIError(APIErrorType::RESPONSE_JSON_PARSE,
                       "unknown finish_reason: " + finish_reason));
        }
        this->_finish_reason = finish_reason;
        return false;
      } catch (const json::exception &e) {
        return tl::unexpected(
            APIError(APIErrorType::RESPONSE_JSON_PARSE, e.what()));
      }
    }

    // The only type left is the CONTENT_DELTA_TYPE.
    if (data_type_str != CONTENT_DELTA_TYPE) {
      return tl::unexpected(APIError(
          APIErrorType::UNKNOWN, "unknown type in chunk: " + data_type_str));
    }

    // Check for the delta and text.
    json delta = chunk_data.value(DELTA_KEY, json::object());
    if (delta.empty()) {
      return tl::unexpected(APIError(APIErrorType::RESPONSE_FIELD_PARSE,
                                     "no delta in chunk data."));
    }
    json text_json = delta[TEXT_KEY];
    if (!text_json.is_string()) {
      return tl::unexpected(APIError(APIErrorType::RESPONSE_FIELD_PARSE,
                                     "text field is not a string"));
    }
    std::string text = text_json.get<std::string>();
    if (print) {
      std::cout << text << std::flush;
    }
    this->_combined_text += text;
  } catch (const json::parse_error &e) {
    return tl::unexpected(
        APIError(APIErrorType::RESPONSE_JSON_PARSE, e.what()));
  }

  return true;
}
