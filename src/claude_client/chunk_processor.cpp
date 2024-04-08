
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "../util/color_print.h"
#include "chunk_processor.h"

using json = nlohmann::json;

// Process the chunks of data from the API response. Return whether to continue
// processing the event stream.
bool ClaudeChunkProcessor2::parse_chunk_data(const std::string &chunk_str,
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

// Process the lines of data from the API response by breaking it up into its
// data sections and processing each chunk. Return whether to continue
// processing the event stream.
bool ClaudeChunkProcessor2::handle_data_lines(const std::string &lines,
                                              bool print) {

  auto data_parts = this->_extractDataSections(lines);
  for (const std::string &data_str : data_parts) {
    // printColoredString(GREEN, "Part: -->%s<--\n", data_str.c_str());

    bool cont = this->parse_chunk_data(data_str, print);
    if (!cont) {
      return false;
    }
  }
  return true;
}

// Extract data sections from the input string. Each data section begins with
// 'data:' and ends with a newline or the end of the string. Return a vector of
// the extracted data sections, without the 'data:' prefix and trailing newline.
// The matcher is forgiving of leading and trailing whitespace and data sections
// do not have to be separated by a blank line.
std::vector<std::string>
ClaudeChunkProcessor2::_extractDataSections(const std::string &input) {
  // Regex: find 'data:' followed by any amount of whitespace, then lazy capture
  // until a newline or end of string.
  std::regex pattern(R"(data:\s*([\s\S]*?)(?:\n|$))");
  std::vector<std::string> dataSections;

  std::sregex_iterator iter(input.begin(), input.end(), pattern);
  std::sregex_iterator end;

  while (iter != end) {
    std::smatch match = *iter;
    // Capture the data excluding the 'data:' prefix and trailing newline.
    dataSections.push_back(match[1].str());
    ++iter;
  }

  return dataSections;
}