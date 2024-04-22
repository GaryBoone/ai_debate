
#pragma once

#include <string>

#include "i_chunk_processor.h"

class ClaudeChunkProcessor : public IChunkProcessor {
public:
  tl::expected<bool, APIError> ParseChunkData(const std::string &chunk_str,
                                              bool print) override;
  std::string CombinedText() override { return combined_text_; };
  std::string FinishReason() override { return finish_reason_; };

private:
  const char *kTypeKey_ = "type";
  const char *kContentDeltaType_ = "content_block_delta";
  const char *kFinishReasonType_ = "message_delta";
  const char *kFinishReasonKey_ = "stop_reason";
  const char *kFinishReasonValue_ = "end_turn";
  const char *kDeltaKey_ = "delta";
  const char *kTextKey_ = "text";

  std::string combined_text_;
  std::string finish_reason_;
};
