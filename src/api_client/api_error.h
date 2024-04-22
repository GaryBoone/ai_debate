#pragma once

#include <stdexcept>
#include <string>
#include <unordered_map>

// APIErrorType is used for for the API module, including errors in creating
// prompts, errors returned by the API, and errors processing responses.
enum class APIErrorType {
  kResponseJsonParse,
  kRequestJsonParse,
  kResponseFieldParse,
  kApiReturnedError,
  kHttpError,
  kUnknown
};
class APIError : public std::runtime_error {
public:
  APIError(APIErrorType type, const std::string &msg,
           const std::exception &cause_exc)
      : std::runtime_error(ConstructMessage(msg, cause_exc)),
        error_type_(type) {}
  APIError(APIErrorType type, const std::string &msg)
      : std::runtime_error(msg), error_type_(type) {}
  APIError(APIErrorType type, const std::exception &cause_exc)
      : std::runtime_error(cause_exc.what()), error_type_(type) {}

  std::string Format() const {
    static const std::unordered_map<APIErrorType, std::string>
        kErrorTypeStrings = {
            {APIErrorType::kResponseJsonParse, "Response JSON Parse Error"},
            {APIErrorType::kRequestJsonParse, "Request JSON Parse Error"},
            {APIErrorType::kResponseFieldParse, "Response Field Parse Error"},
            {APIErrorType::kApiReturnedError, "API Error"},
            {APIErrorType::kHttpError, "HTTP Error"},
            {APIErrorType::kUnknown, "Unknown Error"}};
    std::string output =
        "API Error: (Type: " + kErrorTypeStrings.at(error_type_) +
        "): " + std::string(what());
    return output;
  }
  std::string to_string() const { // NOLINT(readability-identifier-naming)
    return Format();
  }

  friend std::ostream &
  operator<<(std::ostream &os, // NOLINT(readability-identifier-length)
             const APIError &error) {
    os << error.Format();
    return os;
  }

  APIErrorType Type() const { return error_type_; }
  std::string Message() const { return this->what(); }

private:
  APIErrorType error_type_;

  static std::string ConstructMessage(const std::string &api_msg,
                                      const std::exception &err) {
    return api_msg + ": " + err.what();
  }
};