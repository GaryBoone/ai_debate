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

  const std::string system_prompt =
      "You are a playful poet who returns responses with nouns in all-caps.";
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
  claude_chat.SetSystemPrompt(system_prompt);
  auto claude_res = claude_chat.SendMessage(Message{true, prompt}, true);
  if (!claude_res) {
    std::cerr << claude_res.error() << std::endl;
  }
  claude_chat.AddMessage(Message{false, claude_res.value()});
  std::cout << std::endl;

  claude_res =
      claude_chat.SendMessage(Message{true, "Reverse those lines."}, true);
  if (!claude_res) {
    std::cerr << claude_res.error() << std::endl;
  }
#endif

#ifdef DO_GPT
  std::cout << "\n\n************** API--GPT **************\n" << std::flush;
  std::string gpt_api_key = ReadApiKey("OPENAI_API_KEY");
  auto gpt_client = APIClientFactory::CreateGPTClient(gpt_api_key);
  Chat gpt_chat = Chat(std::move(gpt_client));
  gpt_chat.SetSystemPrompt(system_prompt);
  auto gpt_res = gpt_chat.SendMessage(Message{true, prompt}, true);
  if (!gpt_res) {
    std::cerr << gpt_res.error() << std::endl;
  }
  gpt_chat.AddMessage(Message{false, gpt_res.value()});
  std::cout << std::endl;

  gpt_res = gpt_chat.SendMessage(Message{true, "Reverse those lines."}, true);
  if (!gpt_res) {
    std::cerr << gpt_res.error() << std::endl;
  }
#endif

#ifdef DO_GEMINI
  std::cout << "\n\n************** API--Gemini **************\n" << std::flush;
  std::string gemini_key = ReadApiKey("GEMINI_API_KEY");
  auto gemini_client = APIClientFactory::CreateGeminiClient(gemini_key);
  Chat gemini_chat = Chat(std::move(gemini_client));
  gemini_chat.SetSystemPrompt(system_prompt);
  auto gemini_res = gemini_chat.SendMessage(Message{true, prompt}, true);
  if (!gemini_res) {
    std::cerr << gemini_res.error() << std::endl;
  }
  gemini_chat.AddMessage(Message{false, gemini_res.value()});
  std::cout << std::endl;

  gemini_res =
      gemini_chat.SendMessage(Message{true, "Reverse those lines."}, true);
  if (!gemini_res) {
    std::cerr << gemini_res.error() << std::endl;
  }
#endif

  std::cout << std::endl;
  return 0;
}
