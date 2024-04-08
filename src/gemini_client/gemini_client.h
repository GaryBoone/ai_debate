#pragma once

#include <string>

class GeminiClient {
public:
  GeminiClient(const std::string &gemini_api_key,
               const std::string &model = "gpt-4-turbo-preview")
      : _gemini_api_key(gemini_api_key), _model(model) {}

  std::string get_completion(const std::string &prompt, bool print = false);

private:
  std::string _gemini_api_key;
  std::string _model;
};