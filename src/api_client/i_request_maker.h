#pragma once

#include <string>

#include <cpr/cpr.h>

struct APIRequest {
  cpr::Url url;
  cpr::Header header;
  cpr::Body body;
};

class IRequestMaker { // NOLINT(cppcoreguidelines-special-member-functions)
public:
  virtual APIRequest Create(const std::string &prompt) = 0;
  virtual ~IRequestMaker() = default;
};
