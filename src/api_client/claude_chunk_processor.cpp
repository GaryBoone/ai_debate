
#include <cstdio>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "claude_chunk_processor.h"

using json = nlohmann::json;

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
bool ClaudeChunkProcessor::parse_chunk_data(const std::string &chunk_str,
                                            bool print) {
  try {
    json chunk_data = json::parse(chunk_str);

    json completion = chunk_data.value("completion", json::array());
    if (completion.empty()) {
      printf("No completion in chunk data.\n"); // TODO: Handle errors.
      fflush(stdout);
      return false;
    }
    if (chunk_data.contains("stop_reason") &&
        chunk_data["stop_reason"].is_string()) {
      std::string finish_reason = chunk_data["stop_reason"].get<std::string>();
      this->_finish_reason = finish_reason;
      if (finish_reason != "stop_sequence") {
        printf("unknown finish_reason: %s\n", finish_reason.c_str());
        fflush(stdout);
      }
      return false;
    }
    std::string str = completion.get<std::string>();
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
