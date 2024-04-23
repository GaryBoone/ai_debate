
#pragma once

#include <string>
#include <tl/expected.hpp>

#include "i_chunk_processor.h"

// The ApiStreamHandler class is a template class that handles the processing of
// data streams from the API. It is templated on the type of IChunkProcessor,
// an interface that will specialize the processing for the specific API being
// used, Claude, Gemini, or GPT.
template <typename T> class ApiStreamHandler {
public:
  ApiStreamHandler() { static_assert(std::is_base_of_v<IChunkProcessor, T>); }
  tl::expected<bool, APIError> HandleDataLines(const std::string &lines,
                                               bool print = false);

  // Accessors.
  std::string CombinedText() { return combined_text_; }

private:
  static std::vector<std::string> ExtractDataSections(const std::string &input);

  std::string combined_text_;
};
