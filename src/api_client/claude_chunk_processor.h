
#pragma once

#include <string>

#include "i_chunk_processor.h"

class ClaudeChunkProcessor : public IChunkProcessor {
public:
  tl::expected<bool, APIError> parse_chunk_data(const std::string &chunk_str,
                                                bool print = false);
  std::string get_combined_text() { return _combined_text; };
  std::string get_finish_reason() { return _finish_reason; };

private:
  std::string _combined_text;
  std::string _finish_reason;
};
