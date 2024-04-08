#include <string>

#include "claude_request_maker.h"
#include <nlohmann/json.hpp>

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
    // TODO:
    // throw ClaudeError("Failed to create JSON body for Claude request: " +
    //                   std::string(e.what()));
    printf("Failed to create JSON body for Claude request: %s\n",
           std::string(e.what()).c_str());
  }
  return APIRequest{
      cpr::Url{this->_url},
      cpr::Header{{"anthropic-version", "2023-06-01"},
                  {"content-type", "application/json"},
                  {"x-api-key", this->_claude_api_key}},
      cpr::Body{body},
  };
};
