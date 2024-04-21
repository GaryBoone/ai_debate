#include <nlohmann/json.hpp>

#include "api_error.h"
#include "claude_request_maker.h"

APIRequest ClaudeRequestMaker::create(const std::string &prompt) {
  // https://docs.anthropic.com/claude/reference/messages_post
  // Roles: "system", "user", "assistant"
  auto messages = nlohmann::json::array();
  auto user_message = nlohmann::json{{"role", "user"}, {"content", prompt}};
  messages.push_back(user_message);
  try {
    std::string body = nlohmann::json{
        {"model", this->_model},
        {"messages", messages},
        {"max_tokens", _max_tokens},
        {"stream", true}}.dump();
    return APIRequest{
        cpr::Url{this->_url},
        cpr::Header{{"anthropic-version", "2023-06-01"},
                    {"anthropic-beta", "messages-2023-12-15"},
                    {"content-type", "application/json"},
                    {"x-api-key", this->_claude_api_key}},
        cpr::Body{body},
    };
  } catch (nlohmann::json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for Claude request: ", e);
  }
};
