#include <regex>
#include <string>

#include <cpr/cpr.h>

#include "../../tests/test_chunk_processor.h"
#include "api_stream_handler.h"
#include "claude_chunk_processor.h"
#include "gemini_chunk_processor.h"
#include "gpt_chunk_processor.h"

// Process the lines of data from the API response by breaking it up into its
// data sections and processing each chunk. Return whether to continue
// processing the event stream.
template <typename CP>
tl::expected<bool, APIError>
ApiStreamHandler<CP>::HandleDataLines(const std::string &lines, bool print) {

  auto data_parts = this->ExtractDataSections(lines);
  for (const std::string &data_str : data_parts) {

    CP chunk_processor;
    auto cont = chunk_processor.ParseChunkData(data_str, print);
    if (!cont) {
      return cont;
    }
    this->combined_text_ += chunk_processor.ParsedText();
  }
  return true;
}

// Extract data sections from the input string. Each data section begins with
// 'data:' and ends with a newline or the end of the string. Return a vector of
// the extracted data sections, without the 'data:' prefix and trailing newline.
// The matcher is forgiving of leading and trailing whitespace and data sections
// do not have to be separated by a blank line.
template <typename CP>
std::vector<std::string>
ApiStreamHandler<CP>::ExtractDataSections(const std::string &input) {
  // Regex: find 'data:' followed by any amount of whitespace, then lazy capture
  // until a non-capturing newline or end of string. Use [\s\S] to match any
  // character including newlines.
  std::regex pattern(R"(data:\s*([\s\S]*?)(?:\n|$))");
  std::vector<std::string> data_sections;

  std::sregex_iterator iter(input.begin(), input.end(), pattern);
  std::sregex_iterator end;

  while (iter != end) {
    std::smatch match = *iter;
    // Capture the data excluding the 'data:' prefix and trailing newline.
    data_sections.push_back(match[1].str());
    ++iter;
  }

  return data_sections;
}

template class ApiStreamHandler<GPTChunkProcessor>;
template class ApiStreamHandler<GeminiChunkProcessor>;
template class ApiStreamHandler<ClaudeChunkProcessor>;
template class ApiStreamHandler<TestChunkProcessor>;
