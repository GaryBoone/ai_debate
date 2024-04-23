#pragma once

#include <nlohmann/json.hpp>
#include <string>

#include "i_request_maker.h"

class GeminiRequestMaker : public IRequestMaker {
public:
  explicit GeminiRequestMaker(const std::string &api_key,
                              const int max_tokens = 1000)
      : gemini_api_key_(api_key), max_tokens_(max_tokens) {}

  nlohmann::json CreateMessage(const std::string &role,
                               const std::string &text) override;

  APIRequest Create(const std::string &system_prompt,
                    const std::vector<Message> &messages) override;

private:
  std::string url_ = "https://generativelanguage.googleapis.com/v1beta/models/"
                     "gemini-1.5-pro-latest:streamGenerateContent";
  std::string gemini_api_key_;
  int max_tokens_;
};
