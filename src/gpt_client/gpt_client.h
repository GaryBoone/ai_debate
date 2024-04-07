#pragma once

#include <string>

class GptClient {
public:
  GptClient(const std::string &openai_api_key,
            const std::string &model = "gpt-4-turbo-preview")
      : _openai_api_key(openai_api_key), _model(model) {}

  std::string get_completion(const std::string &prompt, bool print=false);

private:
  std::string _openai_api_key;
  std::string _model;
};