#include <string>

#include "api_error.h"
#include "gpt_request_maker.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

APIRequest GPTRequestMaker::Create(const std::string &prompt) {
  // https://platform.openai.com/docs/api-reference/chat/create
  // Roles are: "system", "user", "assistant", "tool", "functions"
  json messages = json::array();
  messages.push_back(json{{"role", "system"},
                          {"content", "You are a playful poet who returns "
                                      "responses with nouns in all-caps."}});
  messages.push_back(json{{"role", "user"}, {"content", prompt}});
  try {
    std::string body = json{
        {"model", this->model_},
        {"messages", messages},
        {"max_tokens", this->max_tokens_},
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
