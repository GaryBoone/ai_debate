
#include <cstdio>
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
bool ChunkProcessor::parse_chunk_data(const std::string &chunk_str,
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
    // if (choice.contains("finishReason") &&
    // choice["finishReason"].is_string()) {
    //   std::string finish_reason = choice["finishReason"].get<std::string>();
    //   this->_finish_reason = finish_reason;
    //   if (finish_reason != "STOP") {
    //     printf("unknown finishReason: %s\n", finish_reason.c_str());
    //     fflush(stdout);
    //   }
    //   return false;
    // }

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

// Process the lines of data from the API response by breaking it up into its
// data sections and processing each chunk. Return whether to continue
// processing the event stream.
bool ChunkProcessor::handle_data_lines(const std::string &lines, bool print) {
  // printf("lines: --->%s<---\n", lines.c_str());
  // fflush(stdout);

  auto data_parts = this->_extractDataSections(lines);
  // printf("**data_parts.size(): %lu\n", data_parts.size());
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
ChunkProcessor::_extractDataSections(const std::string &input) {
  // Regex: find 'data:' followed by any amount of whitespace, then lazy capture
  // until a non-capturing newline or end of string. Use [\s\S] to match any
  // character including newlines.
  // std::regex pattern(R"(data:\s*(.*?)(?:\n|$))");
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