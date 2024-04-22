
#pragma once

#include <string>

#include "i_chunk_processor.h"

class GPTChunkProcessor : public IChunkProcessor {
public:
  tl::expected<bool, APIError> ParseChunkData(const std::string &chunk_str,
                                              bool print) override;
  std::string CombinedText() override { return combined_text_; };
  std::string FinishReason() override { return finish_reason_; };

private:
  std::string combined_text_;
  std::string finish_reason_;
};
