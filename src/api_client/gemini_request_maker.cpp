#include <nlohmann/json.hpp>
#include <string>

#include "api_error.h"
#include "gemini_request_maker.h"

APIRequest GeminiRequestMaker::create(const std::string &prompt) {
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini#request_body
  // Roles are: "user", "model"
  try {
    std::string body =
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
    return APIRequest{
        cpr::Url{this->_url + "?alt=sse&key=" + this->_gemini_api_key},
        cpr::Header{{"content-type", "application/json"},
                    {"key", this->_gemini_api_key}},
        cpr::Body{body},
    };
  } catch (nlohmann::json::exception &e) {
    throw APIError(APIErrorType::REQUEST_JSON_PARSE,
                   "Failed to create JSON for Gemini request: ", e);
  }
};
