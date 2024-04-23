#include <cstdlib>
#include <iostream>
#include <string>

#include <cpr/cpr.h>

#include "api_client/api_client_factory.h"
#include "chat.h"
#include "util/stacktrace.h"

std::string ReadApiKey(const char *env_var_name) {
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
  std::cout << "\n\n************** API--Claude **************\n" << std::flush;
  std::string anthropic_key = ReadApiKey("ANTHROPIC_API_KEY");
  auto claude_client = APIClientFactory::CreateClaudeClient(anthropic_key);
  Chat claude_chat = Chat(std::move(claude_client));
  auto claude_res = claude_chat.SendMessage(prompt, true);
  if (!claude_res) {
    std::cerr << claude_res.error() << std::endl;
  }
#endif

#ifdef DO_GPT
  std::cout << "\n\n************** API--GPT **************\n" << std::flush;
  std::string gpt_api_key = ReadApiKey("OPENAI_API_KEY");
  auto gpt_client = APIClientFactory::CreateGPTClient(gpt_api_key);
  Chat gpt_chat = Chat(std::move(gpt_client));
  auto gpt_res = gpt_chat.SendMessage(prompt, true);
  if (!gpt_res) {
    std::cerr << gpt_res.error() << std::endl;
  }
#endif

#ifdef DO_GEMINI
  std::cout << "\n\n************** API--Gemini **************\n" << std::flush;
  std::string gemini_key = ReadApiKey("GEMINI_API_KEY");
  auto gemini_client = APIClientFactory::CreateGeminiClient(gemini_key);
  Chat gemini_chat = Chat(std::move(gemini_client));
  auto gemini_res = gemini_chat.SendMessage(prompt, true);
  if (!gemini_res) {
    std::cerr << gemini_res.error() << std::endl;
  }
#endif

  std::cout << std::endl;
  return 0;
}
