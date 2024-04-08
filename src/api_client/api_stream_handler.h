
#pragma once

#include <string>

#include "i_chunk_processor.h"

class ApiStreamHandler {
public:
  ApiStreamHandler(std::unique_ptr<IChunkProcessor> chunk_processor)
      : _chunk_processor(std::move(chunk_processor)) {}
  bool handle_data_lines(const std::string &lines, bool print = false);

  // Accessors.
  std::string get_combined_text() { return _combined_text; }
  std::string get_finish_reason() { return _finish_reason; }

private:
  std::unique_ptr<IChunkProcessor> _chunk_processor;
  std::vector<std::string> _extractDataSections(const std::string &input);

  // Instance variables.
  std::string _combined_text;
  std::string _finish_reason;
};
