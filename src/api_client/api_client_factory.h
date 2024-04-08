#pragma once

#include <string>

#include "api_client.h"
#include "claude_chunk_processor.h"
#include "claude_request_maker.h"
#include "gemini_chunk_processor.h"
#include "gemini_request_maker.h"
#include "gpt_chunk_processor.h"
#include "gpt_request_maker.h"

class APIClientFactory {
public:
  static ApiClient createGPTClient(std::string api_key) {
    return ApiClient(std::make_unique<GPTRequestMaker>(api_key),
                     std::make_unique<GPTChunkProcessor>());
  }
  static ApiClient createClaudeClient(std::string api_key) {
    return ApiClient(std::make_unique<ClaudeRequestMaker>(api_key),

                     std::make_unique<ClaudeChunkProcessor>());
  }

  static ApiClient createGeminiClient(std::string api_key) {
    return ApiClient(std::make_unique<GeminiRequestMaker>(api_key),

                     std::make_unique<GeminiChunkProcessor>());
  }
};