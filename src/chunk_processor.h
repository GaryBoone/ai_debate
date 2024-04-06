
#pragma once

#include <string>

class ChunkProcessor {
public:
  bool parse_chunk_data(const std::string &chunk_str, bool print=false);

  bool handle_data_lines(const std::string &lines, bool print=false);

  // Accessors.
  std::string get_combined_text() { return _combined_text; }
  std::string get_finish_reason() { return _finish_reason; }

private:
  std::vector<std::string> _extractDataSections(const std::string &input);

  // Instance variables.
  std::string _combined_text;
  std::string _finish_reason;
};
