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
  static ApiClient<GPTChunkProcessor, GPTRequestMaker>
  CreateGPTClient(std::string api_key) {
    return ApiClient<GPTChunkProcessor, GPTRequestMaker>(api_key);
  }
  static ApiClient<ClaudeChunkProcessor, ClaudeRequestMaker>
  CreateClaudeClient(std::string api_key) {
    return ApiClient<ClaudeChunkProcessor, ClaudeRequestMaker>(api_key);
  }

  static ApiClient<GeminiChunkProcessor, GeminiRequestMaker>
  CreateGeminiClient(std::string api_key) {
    return ApiClient<GeminiChunkProcessor, GeminiRequestMaker>(api_key);
  }
};
