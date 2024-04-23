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
  static std::unique_ptr<IApiClient>
  CreateGPTClient(const std::string &api_key) {
    return std::make_unique<ApiClient<GPTChunkProcessor, GPTRequestMaker>>(
        api_key);
  }
  static std::unique_ptr<IApiClient>
  CreateClaudeClient(const std::string &api_key) {
    return std::make_unique<
        ApiClient<ClaudeChunkProcessor, ClaudeRequestMaker>>(api_key);
  }
  static std::unique_ptr<IApiClient>
  CreateGeminiClient(const std::string &api_key) {
    return std::make_unique<
        ApiClient<GeminiChunkProcessor, GeminiRequestMaker>>(api_key);
  }
};
