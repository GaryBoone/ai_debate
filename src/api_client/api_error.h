#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

// APIErrorType is used for for the API module, including errors in creating
// prompts, errors returned by the API, and errors processing responses.
enum class APIErrorType {
  RESPONSE_JSON_PARSE,
  REQUEST_JSON_PARSE,
  RESPONSE_FIELD_PARSE,
  API_RETURNED_ERROR,
  HTTP_ERROR,
  UNKNOWN
};
class APIError : public std::runtime_error {
public:
  APIError(APIErrorType type, const std::string &msg,
           const std::exception &cause_exc)
      : std::runtime_error(_constructMessage(msg, cause_exc)),
        _error_type(type) {}
  APIError(APIErrorType type, const std::string &msg)
      : std::runtime_error(msg), _error_type(type) {}
  APIError(APIErrorType type, const std::exception &cause_exc)
      : std::runtime_error(cause_exc.what()), _error_type(type) {}

  std::string format() const {
    static const std::unordered_map<APIErrorType, std::string>
        errorTypeStrings = {
            {APIErrorType::RESPONSE_JSON_PARSE, "Response JSON Parse Error"},
            {APIErrorType::REQUEST_JSON_PARSE, "Request JSON Parse Error"},
            {APIErrorType::RESPONSE_FIELD_PARSE, "Response Field Parse Error"},
            {APIErrorType::API_RETURNED_ERROR, "API Error"},
            {APIErrorType::HTTP_ERROR, "HTTP Error"},
            {APIErrorType::UNKNOWN, "Unknown Error"}};
    std::string output =
        "API Error: (Type: " + errorTypeStrings.at(_error_type) +
        "): " + std::string(what());
    return output;
  }
  std::string to_string() const { return format(); }

  friend std::ostream &operator<<(std::ostream &os, const APIError &error) {
    os << error.format();
    return os;
  }

  APIErrorType type() const { return _error_type; }
  const std::string message() const { return this->what(); }

private:
  APIErrorType _error_type;

  static std::string _constructMessage(const std::string &apiMsg,
                                       const std::exception &e) {
    return apiMsg + ": " + e.what();
  }
};