#include <nlohmann/json.hpp>
#include <string>

#include "api_error.h"
#include "claude_request_maker.h"

APIRequest ClaudeRequestMaker::create(const std::string &prompt) {
  std::string full_prompt = "\n\nHuman: " + prompt + "\n\nAssistant:";
  std::string body = "";
  try {
    body = nlohmann::json{
        {"model", this->_model},
        {"prompt", full_prompt},
        {"max_tokens_to_sample", _max_tokens},
        {"stream",
         true}}.dump();
  } catch (nlohmann::json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for Claude request: ", e);
  }
  return APIRequest{
      cpr::Url{this->_url},
      cpr::Header{{"anthropic-version", "2023-06-01"},
                  {"content-type", "application/json"},
                  {"x-api-key", this->_claude_api_key}},
      cpr::Body{body},
  };
};
