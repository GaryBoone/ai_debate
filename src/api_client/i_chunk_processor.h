#pragma once

#include <string>
#include <tl/expected.hpp>

#include "api_error.h"

class IChunkProcessor {
public:
  virtual tl::expected<bool, APIError>
  parse_chunk_data(const std::string &chunk_str, bool print = false) = 0;
  virtual std::string get_combined_text() = 0;
  virtual std::string get_finish_reason() = 0;
  virtual ~IChunkProcessor() = default;
};
