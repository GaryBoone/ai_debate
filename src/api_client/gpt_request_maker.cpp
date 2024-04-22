#include <string>

#include "api_error.h"
#include "gpt_error.h"
#include "gpt_request_maker.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

APIRequest GPTRequestMaker::create(const std::string &prompt) {
  // https://platform.openai.com/docs/api-reference/chat/create
  // Roles are: "system", "user", "assistant", "tool", "functions"
  json messages = json::array();
  messages.push_back(json{{"role", "system"},
                          {"content", "You are a playful poet who returns "
                                      "responses with nouns in all-caps."}});
  messages.push_back(json{{"role", "user"}, {"content", prompt}});
  try {
    std::string body = json{
        {"model", this->_model},
        {"messages", messages},
        {"max_tokens", this->_max_tokens},
        {"stream", true}}.dump();
    return APIRequest{
        cpr::Url{this->_url},
        cpr::Header{{"Authorization", "Bearer " + this->_gpt_api_key},
                    {"Content-Type", "application/json"}},
        cpr::Body{body},
    };
  } catch (json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for GPT request: ", e);
  }
};
