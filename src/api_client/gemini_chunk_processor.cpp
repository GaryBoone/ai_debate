
#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gemini_chunk_processor.h"

using json = nlohmann::json;

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
bool GeminiChunkProcessor::parse_chunk_data(const std::string &chunk_str,
                                            bool print) {
  // The chunk may be a data section with a '[DONE]' message, which is not valid
  // JSON. Check for that before attempting to parse JSON.
  std::regex done_re(R"(^\s*[DONE]\s*$)");
  if (std ::regex_search(chunk_str, done_re)) {
    printf("rec'd [DONE]\n");
    fflush(stdout);
    return false;
  }
  try {
    json chunk_data = json::parse(chunk_str);
    // printColoredString(RED, "chunks: -->%s<--", chunk_data.dump().c_str());

    json choices = chunk_data.value("candidates", json::array());
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

    // printf("choice: >>%s<<\n", choice.dump().c_str());
    // fflush(stdout);
    json content = choice.value("content", json::object());
    if (content.empty()) {
      printf("No content in choice.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    // printf("content: >>%s<<\n", content.dump().c_str());
    // fflush(stdout);

    json parts = content.value("parts", json::object());
    if (parts.empty()) {
      printf("No parts in content.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    json text = parts[0].value("text", json::object());
    if (text.empty()) {
      printf("No parts in content.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    std::string str = text.get<std::string>();
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
