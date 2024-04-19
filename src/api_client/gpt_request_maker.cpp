#include <string>

#include "api_error.h"
#include "gpt_error.h"
#include "gpt_request_maker.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;

APIRequest GPTRequestMaker::create(const std::string &prompt) {
  json messages = json::array();
  messages.push_back(json{{"role", "user"}, {"content", prompt}});
  std::string body = "";
  try {
    body = json{
        {"model", this->_model},
        {"messages", messages},
        {"max_tokens", this->_max_tokens},
        {"stream",
         true}}.dump();
  } catch (json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for GPT request: ", e);
  }
  return APIRequest{
      cpr::Url{this->_url},
      cpr::Header{{"Authorization", "Bearer " + this->_gpt_api_key},
                  {"Content-Type", "application/json"}},
      cpr::Body{body},
  };
};
