
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
  // The chunk may be a data section with a '[DONE]' message, which is not valid
  // JSON. Check for that before attempting to parse JSON.
  std::regex done_re(R"(^\s*[DONE]\s*$)");
  if (std ::regex_search(chunk_str, done_re)) {
    return false;
  }
  try {
    json chunk_data = json::parse(chunk_str);

    json choices = chunk_data.value("choices", json::array());
    if (choices.empty()) {
      printf("No choices in chunk data.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    json choice = choices[0];
    if (choice.empty()) {
      printf("No choice in choices.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    if (choice.contains("finish_reason") &&
        choice["finish_reason"].is_string()) {
      std::string finish_reason = choice["finish_reason"].get<std::string>();
      this->_finish_reason = finish_reason;
      if (finish_reason != "stop") {
        printf("unknown finish_reason: %s\n", finish_reason.c_str());
        fflush(stdout);
      }
      return false;
    }

    json delta = choice.value("delta", json::object());
    if (delta.empty()) {
      printf("No delta in choice.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }

    json content = delta.value("content", json::object());
    if (content.empty()) {
      printf("No content in delta.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    std::string str = content.get<std::string>();
    if (print) {
      printf("%s", str.c_str());
      fflush(stdout);
    }
    this->_combined_text += str;

  } catch (const json::parse_error &e) {
    std::cout << "chunk error: " << e.what() << std::endl << std::flush;
    fflush(stdout);
    // Ignore parsing errors for incomplete chunks.
  }

  return true;
}
