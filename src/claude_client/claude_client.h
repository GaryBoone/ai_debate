#pragma once

#include <string>

class ClaudeClient {
public:
  ClaudeClient(const std::string &claude_api_key,
               const std::string &model = "claude-2")
      : _claude_api_key(claude_api_key), _model(model) {}

  std::string get_completion(const std::string &prompt, bool print = false);

private:
  std::string _claude_api_key;
  std::string _model;
};