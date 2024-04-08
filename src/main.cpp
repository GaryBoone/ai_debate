#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "claude_client/claude_client.h"
#include "gemini_client/gemini_client.h"
#include "gpt_client/gpt_client.h"

using json = nlohmann::json;

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
  const std::string prompt = "Return just the first 2 lines of "
                             "Chaucer's pre-copyright Canterbury Tales.";
  std::cout << "Prompt: " << prompt << std::endl;

  // #if false
  printf("*************** Gemini ***************\n");
  fflush(stdout);
  std::string gemini_key = getAPIKey("GEMINI_API_KEY");
  GeminiClient gemini_client(gemini_key, "claude-2");
  gemini_client.get_completion(prompt, true);
  // #endif

  // #if false
  printf("\n\\n*************** Claude ***************\n");
  fflush(stdout);
  std::string anthropic_key = getAPIKey("ANTHROPIC_API_KEY");
  ClaudeClient claude_client(anthropic_key, "claude-2");
  claude_client.get_completion(prompt, true);
  // #endif

  // #if false
  printf("\n\n*************** GPT ***************\n");
  fflush(stdout);
  std::string gpt_api_key = getAPIKey("OPENAI_API_KEY");
  GptClient gpt_client(gpt_api_key, "gpt-4-1106-preview");
  gpt_client.get_completion(prompt, true);
  // #endif

  return 0;
}
