#pragma once

#include <string>

#include "api_client.h"
#include "claude_chunk_processor.h"
#include "claude_request_maker.h"
#include "gemini_chunk_processor.h"
#include "gemini_request_maker.h"
#include "gpt_chunk_processor.h"
#include "gpt_request_maker.h"

// Factory class to create API clients for GPT, Claude, and Gemini.
class APIClientFactory {
public:
  static ApiClient<GPTChunkProcessor> CreateGPTClient(std::string api_key) {
    return ApiClient<GPTChunkProcessor>(
        std::make_unique<GPTRequestMaker>(api_key));
  }
  static ApiClient<ClaudeChunkProcessor>
  CreateClaudeClient(std::string api_key) {
    return ApiClient<ClaudeChunkProcessor>(
        std::make_unique<ClaudeRequestMaker>(api_key));
  }

  static ApiClient<GeminiChunkProcessor>
  CreateGeminiClient(std::string api_key) {
    return ApiClient<GeminiChunkProcessor>(
        std::make_unique<GeminiRequestMaker>(api_key));
  }
};
