
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

const char *FINISH_REASON_KEY = "stop_reason";
const char *FINISH_REASON_VALUE = "stop_sequence";

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
ClaudeChunkProcessor::parse_chunk_data(const std::string &chunk_str,
                                       bool print) {
  try {
    json chunk_data = json::parse(chunk_str);

    json completion = chunk_data.value("completion", json::array());
    if (completion.empty()) {
      return tl::unexpected(APIError(APIErrorType::RESPONSE_JSON_PARSE,
                                     "no completion in chunk data."));
    }
    if (chunk_data.contains("stop_reason") &&
        chunk_data["stop_reason"].is_string()) {

      std::string finish_reason =
          chunk_data[FINISH_REASON_KEY].get<std::string>();
      this->_finish_reason = finish_reason;
      if (finish_reason != FINISH_REASON_VALUE) {
        return tl::unexpected(
            APIError(APIErrorType::RESPONSE_JSON_PARSE,
                     "unknown finish_reason: " + finish_reason));
      }
      return false;
    }
    std::string str = completion.get<std::string>();
    if (print) {
      std::cout << str << std::flush;
    }
    this->_combined_text += str;

  } catch (const json::parse_error &e) {
    return tl::unexpected(
        APIError(APIErrorType::RESPONSE_JSON_PARSE, e.what()));
  }

  return true;
}
