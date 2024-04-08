#include <string>

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
    // TODO:
    //  throw GPTError("Failed to create JSON body for GPT request: " +
    //                 std::string(e.what()));
    printf("Failed to create JSON body for GPT request: %s\n",
           std::string(e.what()).c_str());
  }
  return APIRequest{
      cpr::Url{this->_url},
      cpr::Header{{"Authorization", "Bearer " + this->_gpt_api_key},
                  {"Content-Type", "application/json"}},
      cpr::Body{body},
  };
};
