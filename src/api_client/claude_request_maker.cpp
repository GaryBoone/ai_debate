#include <nlohmann/json.hpp>

#include "api_error.h"
#include "claude_request_maker.h"
#include "i_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

json ClaudeRequestMaker::CreateMessage(const std::string &role,
                                       const std::string &text) {
  return json{{"role", role}, {"content", text}};
}

APIRequest ClaudeRequestMaker::Create(const std::string &system_prompt,
                                      const std::vector<Message> &messages) {
  // https://docs.anthropic.com/claude/reference/messages_post
  // Roles: "system", "user", "assistant"
  auto json_messages = json::array();
  for (const auto &message : messages) {
    json_messages.push_back(
        this->CreateMessage(message.user ? "user" : "assistant", message.text));
  }
  // The Claude API requires the assistant message to be prefilled with a
  // starter '{' character to ensure that the response is valid JSON. Otherwise,
  // the model tends to generate a text preamble like "Here is my response in
  // JSON format: { ... }" instead of just returning the JSON.
  // https://docs.anthropic.com/claude/docs/control-output-format#prefilling-claudes-response
  json_messages.push_back(this->CreateMessage("assistant", "{"));
  try {
    std::string body = json{
        {"model", this->model_},
        {"system", system_prompt},
        {"messages", json_messages},
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
