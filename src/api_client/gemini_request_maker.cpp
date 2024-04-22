#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

#include "api_error.h"
#include "gemini_request_maker.h"

using json = nlohmann::json;

json GeminiRequestMaker::_create_message(const std::string &role,
                                         const std::string &text) {
  json parts_obj = json::object({{"text", text}});
  return json{{"role", role}, {"parts", json::array({parts_obj})}};
}

APIRequest GeminiRequestMaker::create(const std::string &prompt) {
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini#request_body
  // Roles are: "user", "model"
  try {
    json body = {};
    body["systemInstruction"] = _create_message(
        "system",
        "You are a playful poet who returns responses with nouns in all-caps.");
    body["contents"] = _create_message("user", prompt);
    body["generation_config"] = json{{"max_output_tokens", this->_max_tokens}};
    return APIRequest{
        cpr::Url{this->_url + "?alt=sse&key=" + this->_gemini_api_key},
        cpr::Header{{"content-type", "application/json"},
                    {"key", this->_gemini_api_key}},
        cpr::Body{body.dump()},
    };
  } catch (json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for Gemini request: ", e);
  }
};
