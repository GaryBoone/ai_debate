#pragma once

#include <string>

#include "i_request_maker.h"

class GPTRequestMaker : public IRequestMaker {
public:
  GPTRequestMaker(const std::string &api_key,
                  const std::string &model = "gpt-4-1106-preview",
                  const int max_tokens = 1000)
      : _gpt_api_key(api_key), _model(model), _max_tokens(max_tokens) {}
  APIRequest create(const std::string &prompt);

private:
  std::string _url = "https://api.openai.com/v1/chat/completions";
  std::string _gpt_api_key;
  std::string _model;
  int _max_tokens;
};
