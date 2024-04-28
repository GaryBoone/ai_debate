
#pragma once

#include <string>

#include "../src/api_client/i_chunk_processor.h"

class TestChunkProcessor : public IChunkProcessor {
public:
  tl::expected<bool, APIError> ParseChunkData(const std::string & /*chunk_str*/,
                                              bool /*print*/) override {
    return true;
  };
  std::string ParsedText() override { return ""; };
  std::string FinishReason() override { return ""; };
};
