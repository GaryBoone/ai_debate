
#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gpt_chunk_processor.h"

using json = nlohmann::json;

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
tl::expected<bool, APIError>
GPTChunkProcessor::parse_chunk_data(const std::string &chunk_str, bool print) {

  // printf("chunk_str: -->%s<--\n", chunk_str.c_str());
  // fflush(stdout);

  // The chunk may be a data section with a '[DONE]' message, which is not
  // valid JSON. Check for that before attempting to parse JSON.
  std::regex done_re(R"(^\s*[DONE]\s*$)");
  if (std ::regex_search(chunk_str, done_re)) {
    printf("rec'd [DONE]\n");
    fflush(stdout);
    return false;
  }

  try {
    json chunk_data = json::parse(chunk_str);

    json choices = chunk_data.value("choices", json::array());
    if (choices.empty()) {
      return tl::unexpected(APIError(APIErrorType::RESPONSE_JSON_PARSE,
                                     "no choices in chunk data."));
    }
    json choice = choices[0];
    if (choice.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::RESPONSE_JSON_PARSE, "no choice in choices."));
    }
    if (choice.contains("finish_reason") &&
        choice["finish_reason"].is_string()) {
      std::string finish_reason = choice["finish_reason"].get<std::string>();
      this->_finish_reason = finish_reason;
      if (finish_reason != "stop") {
        return tl::unexpected(
            APIError(APIErrorType::RESPONSE_JSON_PARSE,
                     "unknown finish_reason: " + finish_reason));
      }
      return false;
    }

    json delta = choice.value("delta", json::object());
    if (delta.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::RESPONSE_JSON_PARSE, "no delta in choice."));
    }

    json content = delta.value("content", json::object());
    if (content.empty()) {
      return tl::unexpected(
          APIError(APIErrorType::RESPONSE_JSON_PARSE, "no content in delta."));
    }
    std::string str = content.get<std::string>();
    if (print) {
      printf("%s", str.c_str());
      fflush(stdout);
    }
    this->_combined_text += str;
  } catch (const json::parse_error &e) {
    return tl::unexpected(
        APIError(APIErrorType::RESPONSE_JSON_PARSE, e.what()));
  }

  return true;
}
