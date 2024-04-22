#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gpt_chunk_processor.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
GPTChunkProcessor::ParseChunkData(const std::string &chunk_str, bool print) {

  // The chunk may be a data section with a '[DONE]' message, which is not
  // valid JSON. Check for that before attempting to parse JSON.
  std::regex done_re(R"(^\s*[DONE]\s*$)");
  if (std ::regex_search(chunk_str, done_re)) {
    return false;
  }

  try {
    json chunk_data = json::parse(chunk_str);

    json choices = chunk_data.value("choices", json::array());
    if (choices.empty()) {
      return tl::unexpected(APIError(APIErrorType::kResponseJsonParse,
                                     "no choices in chunk data."));
    }
    json choice = choices[0];
    if (choice.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no choice in choices."));
    }
    if (choice.contains("finish_reason") &&
        choice["finish_reason"].is_string()) {
      std::string finish_reason = choice["finish_reason"].get<std::string>();
      this->finish_reason_ = finish_reason;
      if (finish_reason != "stop") {
        return tl::unexpected(
            APIError(APIErrorType::kResponseJsonParse,
                     "unknown finish_reason: " + finish_reason));
      }
      return false;
    }

    json delta = choice.value("delta", json::object());
    if (delta.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no delta in choice."));
    }

    json content = delta.value("content", json::object());
    if (content.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no content in delta."));
    }
    std::string str = content.get<std::string>();
    if (print) {
      std::cout << str << std::flush;
    }
    this->combined_text_ += str;
  } catch (const json::parse_error &e) {
    return tl::unexpected(APIError(APIErrorType::kResponseJsonParse, e));
  }

  return true;
}
