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

#define DO_CLAUDE
#define DO_GPT
#define DO_GEMINI

#ifdef DO_CLAUDE
  printf("\n\n*************** API--Claude ***************\n");
  fflush(stdout);
  std::string anthropic_key = getAPIKey("ANTHROPIC_API_KEY");
  ApiClient<ClaudeChunkProcessor> claude_client =
      APIClientFactory::createClaudeClient(anthropic_key);
  auto claude_res = claude_client.get_completion(prompt, true);
  if (!claude_res) {
    std::cerr << claude_res.error() << std::endl;
  }
#endif

#ifdef DO_GPT
  printf("\n\n*************** API--GPT ***************\n");
  fflush(stdout);
  std::string gpt_api_key = getAPIKey("OPENAI_API_KEY");
  ApiClient<GPTChunkProcessor> gpt_client =
      APIClientFactory::createGPTClient(gpt_api_key);
  auto gpt_res = gpt_client.get_completion(prompt, true);
  if (!gpt_res) {
    std::cerr << gpt_res.error() << std::endl;
  }
#endif

#ifdef DO_GEMINI
  printf("\n\n*************** API--Gemini ***************\n");
  fflush(stdout);
  std::string gemini_key = getAPIKey("GEMINI_API_KEY");
  ApiClient<GeminiChunkProcessor> gemini_client =
      APIClientFactory::createGeminiClient(gemini_key);
  auto gemini_res = gemini_client.get_completion(prompt, true);
  if (!gemini_res) {
    std::cerr << gemini_res.error() << std::endl;
  }
#endif

  return 0;
}
