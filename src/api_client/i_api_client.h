#pragma once

#include <string>

#include <nlohmann/json.hpp>
#include <tl/expected.hpp>

#include "api_error.h"
#include "i_request_maker.h"

using json = nlohmann::json; // NOLINT(readability-identifier-naming)

// This is the response structure that the system prompt asks the API to return.
struct Response {
  std::string text;
  bool done;
};

// If defined, from_json() enables nlohmann::json to convert from a JSON object
// to a Response object using `json_obj.get<Response>();`.
inline void from_json( // NOLINT(readability-identifier-naming
    const json &json_obj, Response &resp) {
  try {
    json_obj.at("text").get_to(resp.text);
    json_obj.at("done").get_to(resp.done);
  } catch (const json::type_error &e) {
    throw APIError(APIErrorType::kResponseJsonParse, e.what());
  }
}

class IApiClient { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual ~IApiClient() = default;

  // You'll likely have more methods here, but 'CallApi' is a good start
  virtual tl::expected<Response, APIError>
  CallApi(const std::string &system_prompt,
          const std::vector<Message> &messages, bool print) = 0;
};