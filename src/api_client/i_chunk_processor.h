#pragma once

#include <string>

class IChunkProcessor {
public:
  virtual bool parse_chunk_data(const std::string &chunk_str,
                                bool print = false) = 0;
  virtual ~IChunkProcessor() = default;
};
