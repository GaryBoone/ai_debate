#include <nlohmann/json.hpp>

#include "api_error.h"
#include "claude_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

APIRequest ClaudeRequestMaker::Create(const std::string &prompt) {
  // https://docs.anthropic.com/claude/reference/messages_post
  // Roles: "system", "user", "assistant"
  const auto *system_message =
      "You are a playful poet who returns responses with nouns in all-caps.";
  auto messages = json::array();
  auto user_message = json{{"role", "user"}, {"content", prompt}};
  messages.push_back(user_message);
  try {
    std::string body = json{
        {"model", this->model_},
        {"system", system_message},
        {"messages", messages},
        {"max_tokens", max_tokens_},
        {"stream", true}}.dump();
    return APIRequest{
        cpr::Url{this->url_},
        cpr::Header{{"anthropic-version", "2023-06-01"},
                    {"anthropic-beta", "messages-2023-12-15"},
                    {"content-type", "application/json"},
                    {"x-api-key", this->claude_api_key_}},
        cpr::Body{body},
    };
  } catch (json::exception &e) {
    throw APIError(APIErrorType::kRequestJsonParse,
                   "Failed to create JSON for Claude request: ", e);
  }
};
