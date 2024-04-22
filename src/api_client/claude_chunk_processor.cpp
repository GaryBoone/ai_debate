
#include <cpr/cpr.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <tl/expected.hpp>

#include "api_error.h"
#include "claude_chunk_processor.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
ClaudeChunkProcessor::ParseChunkData(const std::string &chunk_str, bool print) {

  const std::array<std::string, 5> types_to_ignore = {
      "ping", "message_start", "content_block_start", "content_block_stop",
      "message_stop"};

  try {
    json chunk_data = json::parse(chunk_str);

    // First, check the type.
    json data_type = chunk_data[kTypeKey_];
    if (!data_type.is_string()) {
      return tl::unexpected(APIError(APIErrorType::kResponseFieldParse,
                                     "Type field is not a string."));
    }
    std::string data_type_str = data_type.get<std::string>();
    if (std::find(std::begin(types_to_ignore), std::end(types_to_ignore),
                  data_type_str) != std::end(types_to_ignore)) {
      return true;
    }

    // Check for the finish reason.
    if (data_type_str == kFinishReasonType_) {
      try {
        std::string finish_reason =
            chunk_data[kFinishReasonKey_].get<std::string>();
        if (finish_reason != kFinishReasonValue_) {
          return tl::unexpected(
              APIError(APIErrorType::kResponseJsonParse,
                       "unknown finish_reason: " + finish_reason));
        }
        this->finish_reason_ = finish_reason;
        return false;
      } catch (const json::exception &e) {
        return tl::unexpected(
            APIError(APIErrorType::kResponseJsonParse, e.what()));
      }
    }

    // The only type left is the kContentDeltaType_.
    if (data_type_str != kContentDeltaType_) {
      return tl::unexpected(APIError(
          APIErrorType::kUnknown, "unknown type in chunk: " + data_type_str));
    }

    // Check for the delta and text.
    json delta = chunk_data.value(kDeltaKey_, json::object());
    if (delta.empty()) {
      return tl::unexpected(APIError(APIErrorType::kResponseFieldParse,
                                     "no delta in chunk data."));
    }
    json text_json = delta[kTextKey_];
    if (!text_json.is_string()) {
      return tl::unexpected(APIError(APIErrorType::kResponseFieldParse,
                                     "text field is not a string"));
    }
    std::string text = text_json.get<std::string>();
    if (print) {
      std::cout << text << std::flush;
    }
    this->combined_text_ += text;
  } catch (const json::parse_error &e) {
    return tl::unexpected(APIError(APIErrorType::kResponseJsonParse, e.what()));
  }

  return true;
}
