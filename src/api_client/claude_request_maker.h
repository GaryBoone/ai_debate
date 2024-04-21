#pragma once

#include <string>

#include "i_request_maker.h"

class ClaudeRequestMaker : public IRequestMaker {
public:
  ClaudeRequestMaker(const std::string &api_key,
                     const std::string &model = "claude-3-opus-20240229",
                     const int max_tokens = 1000)
      : _claude_api_key(api_key), _model(model), _max_tokens(max_tokens) {}
  APIRequest create(const std::string &prompt);

private:
  std::string _url = "https://api.anthropic.com/v1/messages";
  std::string _claude_api_key;
  std::string _model;
  int _max_tokens;
};
