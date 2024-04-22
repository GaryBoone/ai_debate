#pragma once

#include <string>
#include <tl/expected.hpp>

#include "api_error.h"

class IChunkProcessor { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual tl::expected<bool, APIError>
  ParseChunkData(const std::string &chunk_str, bool print) = 0;
  virtual std::string CombinedText() = 0;
  virtual std::string FinishReason() = 0;
  virtual ~IChunkProcessor() = default;
};
