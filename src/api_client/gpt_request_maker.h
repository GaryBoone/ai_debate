#pragma once

#include <string>

#include "i_request_maker.h"

class GPTRequestMaker : public IRequestMaker {
public:
  explicit GPTRequestMaker(const std::string &api_key,
                           const int max_tokens = 1000,
                           const std::string &model = "gpt-4-1106-preview")
      : gpt_api_key_(api_key), model_(model), max_tokens_(max_tokens) {}
  APIRequest Create(const std::string &prompt) override;

private:
  std::string url_ = "https://api.openai.com/v1/chat/completions";
  std::string gpt_api_key_;
  std::string model_;
  int max_tokens_;
};
