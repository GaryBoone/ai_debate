
#pragma once

#include <string>

#include "i_chunk_processor.h"

class ClaudeChunkProcessor : public IChunkProcessor {
public:
  bool parse_chunk_data(const std::string &chunk_str, bool print = false);

private:
  std::string _combined_text;
  std::string _finish_reason;
};