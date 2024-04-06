#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "gpt_client.h"

using json = nlohmann::json;

std::string getOpenAIKey() {
  const char *openai_key = std::getenv("OPENAI_API_KEY");
  if (openai_key == nullptr) {
    std::cerr << "Error: OPENAI_API_KEY environment variable not set."
              << std::endl;
    return "";
  }
  return std::string(openai_key);
}

int main() {
  std::string api_key = getOpenAIKey();
  const std::string prompt = "Explain the significance of the first 2 lines of "
                             "Chaucer's Canterbury Tales.";
  GptClient gpt_client(api_key, "gpt-4-1106-preview");
  std::cout << "Prompt: " << prompt << std::endl;
  std::string completion = gpt_client.get_completion(prompt, true);
  return 0;
}
