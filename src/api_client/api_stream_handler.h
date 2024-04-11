
#pragma once

#include <string>
#include <tl/expected.hpp>

#include "i_chunk_processor.h"

template <typename T> class ApiStreamHandler {
public:
  ApiStreamHandler() { static_assert(std::is_base_of_v<IChunkProcessor, T>); }
  tl::expected<bool, APIError> handle_data_lines(const std::string &lines,
                                                 bool print = false);

  // Accessors.
  std::string get_combined_text() { return _combined_text; }

private:
  std::vector<std::string> _extract_data_sections(const std::string &input);

  // Instance variables.
  std::string _combined_text;
};
