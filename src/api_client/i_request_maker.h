#pragma once

#include <string>

#include <cpr/cpr.h>

struct APIRequest {
  cpr::Url url;
  cpr::Header header;
  cpr::Body body;
};

class IRequestMaker {
public:
  virtual APIRequest create(const std::string &prompt) = 0;
  virtual ~IRequestMaker() = default;
};
