#include <nlohmann/json.hpp>
#include <string>

#include "api_error.h"
#include "gemini_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

json GeminiRequestMaker::CreateMessage(
    const std::string &role, // NOLINT(bugprone-easily-swappable-parameters)
    const std::string &text) {
  json parts_obj = json::object({{"text", text}});
  return json{{"role", role}, {"parts", json::array({parts_obj})}};
}

APIRequest GeminiRequestMaker::Create(const std::string &prompt) {
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini#request_body
  // Roles are: "user", "model"
  try {
    json body = {};
    body["systemInstruction"] = CreateMessage(
        "system",
        "You are a playful poet who returns responses with nouns in all-caps.");
    body["contents"] = CreateMessage("user", prompt);
    body["generation_config"] = json{{"max_output_tokens", this->max_tokens_}};
    return APIRequest{
        cpr::Url{this->url_ + "?alt=sse&key=" + this->gemini_api_key_},
        cpr::Header{{"content-type", "application/json"},
                    {"key", this->gemini_api_key_}},
        cpr::Body{body.dump()},
    };
  } catch (json::exception &e) {
    throw APIError(APIErrorType::kRequestJsonParse,
                   "Failed to create JSON for Gemini request: ", e);
  }
};
