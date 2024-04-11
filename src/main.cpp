#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>

#include "api_client/api_client.h"
#include "api_client/api_client_factory.h"
#include "api_client/claude_chunk_processor.h"
#include "api_client/gemini_chunk_processor.h"
#include "api_client/gpt_chunk_processor.h"
#include "util/stacktrace.h"

std::string getAPIKey(const char *env_var_name) {
  const char *openai_key = std::getenv(env_var_name);
  if (openai_key == nullptr) {
    std::cerr << "Error: API key " << env_var_name
              << " environment variable not set." << std::endl;
    exit(1);
  }
  return std::string(openai_key);
}

int main() {
  std::set_terminate(globalExceptionHandler);

  const std::string prompt = "Return just the first 2 lines of "
                             "Chaucer's pre-copyright Canterbury Tales.";
  std::cout << "Prompt: " << prompt << std::endl;

  // #if false
  printf("\n\n*************** API--Claude ***************\n");
  fflush(stdout);
  std::string anthropic_key = getAPIKey("ANTHROPIC_API_KEY");
  // ClaudeClient claude_client(anthropic_key, "claude-2");
  ApiClient<ClaudeChunkProcessor> claude_client =
      APIClientFactory::createClaudeClient(anthropic_key);
  claude_client.get_completion(prompt, true);
  // #endif

  // #if false
  printf("\n\n*************** API--GPT ***************\n");
  fflush(stdout);
  std::string gpt_api_key = getAPIKey("OPENAI_API_KEY");
  ApiClient<GPTChunkProcessor> gpt_client =
      APIClientFactory::createGPTClient(gpt_api_key);
  gpt_client.get_completion(prompt, true);
  printf("\n\n*************** API--GPT 2 *************\n");
  fflush(stdout);
  gpt_client.get_completion(prompt, true);
  // #endif

  // #if false
  printf("\n\n*************** API--Gemini ***************\n");
  fflush(stdout);
  std::string gemini_key = getAPIKey("GEMINI_API_KEY");
  ApiClient<GeminiChunkProcessor> gemini_client =
      APIClientFactory::createGeminiClient(gemini_key);
  gemini_client.get_completion(prompt, true);
  // #endif

  return 0;
}
