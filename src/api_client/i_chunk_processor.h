#pragma once

#include <string>
#include <tl/expected.hpp>

#include "api_error.h"

// Interface for processing chunks of data. It provides a method for parsing
// the chunks of data returned from the chat APIs, printing the data as it's
// processed, and returning the combined text from the chunks.
// It also provides accessors for the combined text and the reason for
// finishing.
class IChunkProcessor { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual tl::expected<bool, APIError>
  ParseChunkData(const std::string &chunk_str, bool print) = 0;
  virtual std::string ParsedText() = 0;
  virtual std::string FinishReason() = 0;
  virtual ~IChunkProcessor() = default;
};
