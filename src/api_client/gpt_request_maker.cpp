#include <string>

#include "api_error.h"
#include "gpt_request_maker.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

json GPTRequestMaker::CreateMessage(
    const std::string &role, // NOLINT(bugprone-easily-swappable-parameters)
    const std::string &text) {
  return json{{"role", role}, {"content", text}};
}

APIRequest GPTRequestMaker::Create(const std::string &system_prompt,
                                   const std::vector<Message> &prompts) {
  // https://platform.openai.com/docs/api-reference/chat/create
  // Roles are: "system", "user", "assistant", "tool", "functions"
  // Note: To use JSON, the model must be at least "gpt-4-turbo-preview".
  json messages = json::array();
  messages.push_back(this->CreateMessage("system", system_prompt));
  for (const auto &message : prompts) {
    messages.push_back(
        this->CreateMessage(message.user ? "user" : "assistant", message.text));
  }
  try {
    std::string body = json{
        {"model", this->model_},
        {"messages", messages},
        {"max_tokens", this->max_tokens_},
        {"response_format", {{"type", "json_object"}}},
        {"stream", true}}.dump();
    return APIRequest{
        cpr::Url{this->url_},
        cpr::Header{{"Authorization", "Bearer " + this->gpt_api_key_},
                    {"Content-Type", "application/json"}},
        cpr::Body{body},
    };
  } catch (json::exception &e) {
    throw APIError(APIErrorType::kRequestJsonParse,
                   "Failed to create JSON for GPT request: ", e);
  }
};
