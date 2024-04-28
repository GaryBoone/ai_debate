#include <nlohmann/json.hpp>
#include <string>

#include "api_error.h"
#include "gemini_request_maker.h"
#include "i_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

json GeminiRequestMaker::CreateMessage(
    const std::string &role, // NOLINT(bugprone-easily-swappable-parameters)
    const std::string &text) {
  json parts_obj = json::object({{"text", text}});
  return json{{"role", role}, {"parts", json::array({parts_obj})}};
}

APIRequest GeminiRequestMaker::Create(const std::string &system_prompt,
                                      const std::vector<Message> &messages) {
  // https://cloud.google.com/vertex-ai/generative-ai/docs/model-reference/gemini#request_body
  // Roles are: "user", "model"
  try {
    json body = {};
    body["systemInstruction"] = CreateMessage("system", system_prompt);
    auto json_messages = json::array();
    for (const auto &message : messages) {
      json_messages.push_back(
          CreateMessage(message.user ? "user" : "model", message.text));
    }
    body["contents"] = json_messages;
    body["generation_config"] = json{
        {"max_output_tokens", this->max_tokens_},
        {"response_mime_type", "application/json"},
    };
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
