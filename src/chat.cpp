#include "chat.h"
#include <string>

tl::expected<std::string, APIError> Chat::SendMessage(const Message &message,
                                                      bool print = false) {
  this->AddMessage(message);
  return this->api_client_->CallApi(this->system_prompt_, this->messages_,
                                    print);
}
