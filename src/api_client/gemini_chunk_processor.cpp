
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini_chunk_processor.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
GeminiChunkProcessor::ParseChunkData(const std::string &chunk_str, bool print) {
  // The chunk may be a data section with a '[DONE]' message, which is not valid
  // JSON. Check for that before attempting to parse JSON.
  std::regex done_re(R"(^\s*[DONE]\s*$)");
  if (std ::regex_search(chunk_str, done_re)) {
    return false;
  }
  try {
    json chunk_data = json::parse(chunk_str);

    json choices = chunk_data.value("candidates", json::array());
    if (choices.empty()) {
      return tl::unexpected(APIError(APIErrorType::kResponseJsonParse,
                                     "no candidates in chunk data."));
    }
    json choice = choices[0];
    if (choice.empty()) {
      return tl::unexpected(APIError(APIErrorType::kResponseJsonParse,
                                     "no choice in candidates."));
    }

    json content = choice.value("content", json::object());
    if (content.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no content in choice."));
    }

    json parts = content.value("parts", json::object());
    if (parts.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no parts in content."));
    }
    json text = parts[0].value("text", json::object());
    if (text.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::kResponseJsonParse, "no text in parts."));
    }
    std::string str = text.get<std::string>();
    if (print) {
      std::cout << str << std::flush;
    }
    this->combined_text_ += str;

  } catch (const json::parse_error &e) {
    return tl::unexpected(APIError(APIErrorType::kResponseJsonParse, e.what()));
  }

  return true;
}
