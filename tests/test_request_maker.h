#pragma once

#include <string>

#include "../src/api_client/i_request_maker.h"

class TestRequestMaker : public IRequestMaker {
public:
  explicit TestRequestMaker(const std::string & /*api_key*/,
                            const int /*max_tokens*/ = 0,
                            const std::string & /*model*/ = "") {}

  nlohmann::json CreateMessage(const std::string & /*role*/,
                               const std::string & /*text*/) override {
    return nlohmann::json{};
  }

  APIRequest Create(const std::string & /*system_prompt*/,
                    const std::vector<Message> & /*prompts*/) override {
    return APIRequest{};
  };
};