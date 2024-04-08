#pragma once

#include <string>

#include "i_request_maker.h"

class GeminiRequestMaker : public IRequestMaker {
public:
  GeminiRequestMaker(const std::string &api_key, const int max_tokens = 1000)
      : _gemini_api_key(api_key), _max_tokens(max_tokens) {}
  APIRequest create(const std::string &prompt);

private:
  std::string _url = "https://generativelanguage.googleapis.com/v1beta/models/"
                     "gemini-pro:streamGenerateContent";
  std::string _gemini_api_key;
  int _max_tokens;
};
