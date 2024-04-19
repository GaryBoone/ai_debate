#pragma once

#include <iostream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Error {
  std::string message;
  std::string type;
  json param;
  json code;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Error, message, type, param, code)

inline bool handle_gpt_error(const std::string &line) {
  json errorJson = json::parse(line);
  Error error = errorJson["error"].get<Error>();
  // Handle the error accordingly
  std::cout << "GPT Error: " << error.message << " (Type: " << error.type << ")"
            << std::endl
            << std::flush;
  return false;
}