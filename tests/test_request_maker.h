#pragma once

#include <string>

#include "../src/api_client/i_request_maker.h"

class TestRequestMaker : public IRequestMaker {
public:
  explicit TestRequestMaker(const std::string &api_key,
                            const int max_tokens = 0,
                            const std::string &model = "") {}
  APIRequest Create(const std::string &prompt) override {
    return APIRequest{};
  };
};