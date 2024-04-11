
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>

#include <cpr/cpr.h>

#include "api_stream_handler.h"
#include "claude_chunk_processor.h"
#include "gemini_chunk_processor.h"
#include "gpt_chunk_processor.h"

// Process the lines of data from the API response by breaking it up into its
// data sections and processing each chunk. Return whether to continue
// processing the event stream.
template <typename T>
tl::expected<bool, APIError>
ApiStreamHandler<T>::handle_data_lines(const std::string &lines, bool print) {
  // throw std::runtime_error("This is a test exception.");

  auto data_parts = this->_extract_data_sections(lines);
  for (const std::string &data_str : data_parts) {
    // printColoredString(GREEN, "Part: -->%s<--\n", data_str.c_str());

    T chunk_processor;
    auto cont = chunk_processor.parse_chunk_data(data_str, print);
    if (!cont) {
      // TODO: Handle the error.
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
template <typename T>
std::vector<std::string>
ApiStreamHandler<T>::_extract_data_sections(const std::string &input) {
  // Regex: find 'data:' followed by any amount of whitespace, then lazy capture
  // until a non-capturing newline or end of string. Use [\s\S] to match any
  // character including newlines.
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

template class ApiStreamHandler<GPTChunkProcessor>;
template class ApiStreamHandler<GeminiChunkProcessor>;
template class ApiStreamHandler<ClaudeChunkProcessor>;
