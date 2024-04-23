#pragma once

#include <string>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

struct Message {
  bool user;
  std::string text;
};

struct APIRequest {
  cpr::Url url;
  cpr::Header header;
  cpr::Body body;
};

// Interface for creating API requests.
class IRequestMaker { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual nlohmann::json CreateMessage(
      const std::string &role, // NOLINT(bugprone-easily-swappable-parameters)
      const std::string &text) = 0;
  virtual APIRequest Create(const std::string &system_prompt,
                            const std::vector<Message> &messages) = 0;

  virtual ~IRequestMaker() = default;
};
