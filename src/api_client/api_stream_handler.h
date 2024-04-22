
#pragma once

#include <string>
#include <tl/expected.hpp>

#include "i_chunk_processor.h"

template <typename T> class ApiStreamHandler {
public:
  ApiStreamHandler() { static_assert(std::is_base_of_v<IChunkProcessor, T>); }
  tl::expected<bool, APIError> HandleDataLines(const std::string &lines,
                                               bool print = false);

  // Accessors.
  std::string CombinedText() { return combined_text_; }

private:
  std::vector<std::string> ExtractDataSections(const std::string &input);

  // Instance variables.
  std::string combined_text_;
};
