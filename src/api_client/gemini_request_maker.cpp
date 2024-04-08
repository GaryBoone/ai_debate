#include <string>

#include "gemini_request_maker.h"
#include <nlohmann/json.hpp>

APIRequest GeminiRequestMaker::create(const std::string &prompt) {
  std::string full_prompt = "\n\nHuman: " + prompt + "\n\nAssistant:";
  std::string body = "";
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini#request_body
  try {
    body =
        nlohmann::json{
            {"contents",
             {{"role", "user"},
              {
                  "parts",
                  {{"text", prompt}},
              }}},
            {"generation_config", {{"max_output_tokens", this->_max_tokens}}},
        }
            .dump();
  } catch (nlohmann::json::exception &e) {
    // TODO:
    // throw GeminiError("Failed to create JSON body for Claude request: " +
    //                   std::string(e.what()));
    printf("Failed to create JSON body for Claude request: %s\n",
           std::string(e.what()).c_str());
  }
  return APIRequest{
      cpr::Url{this->_url + "?alt=sse&key=" + this->_gemini_api_key},
      cpr::Header{{"content-type", "application/json"},
                  {"key", this->_gemini_api_key}},
      cpr::Body{body},
  };
};
