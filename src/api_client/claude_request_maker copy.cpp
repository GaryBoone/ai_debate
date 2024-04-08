#include <string>

#include "claude_request_maker.h"
#include <nlohmann/json.hpp>

APIRequest ClaudeRequestMaker::create(const std::string &prompt) {
  std::string full_prompt = "\n\nHuman: " + prompt + "\n\nAssistant:";
  return APIRequest{
      cpr::Url{this->_url},
      cpr::Header{{"anthropic-version", "2023-06-01"},
                  {"content-type", "application/json"},
                  {"x-api-key", this->_claude_api_key}},
      cpr::Body{nlohmann::json{{"model", this->_model},
                               {"prompt", full_prompt},
                               {"max_tokens_to_sample", _max_tokens},
                               {"stream", true}}
                    .dump()},
  };
};
