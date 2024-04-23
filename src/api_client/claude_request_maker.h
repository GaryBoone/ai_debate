#pragma once

#include <string>

#include "i_request_maker.h"

class ClaudeRequestMaker : public IRequestMaker {
public:
  explicit ClaudeRequestMaker(
      const std::string &api_key, const int max_tokens = 1000,
      const std::string &model = "claude-3-opus-20240229")
      : claude_api_key_(api_key), model_(model), max_tokens_(max_tokens) {}

  nlohmann::json CreateMessage(const std::string &role,
                               const std::string &text) override;

  APIRequest Create(const std::string &system_prompt,
                    const std::vector<Message> &messages) override;

private:
  std::string url_ = "https://api.anthropic.com/v1/messages";
  std::string claude_api_key_;
  std::string model_;
  int max_tokens_;
};
