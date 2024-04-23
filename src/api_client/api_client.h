#pragma once

#include <string>

#include "api_stream_handler.h"
#include "i_request_maker.h"

// The ApiClient class manages requests and response to the chat APIs. It is
// templated on the type of the chunk processor and request maker that will
// customize the client to each API:  Claude, Gemini, or GPT.
//
//    ApiClient<GPTChunkProcessor, GPTRequestMaker>(api_key);
//
// Design: The ApiClient class uses interfaces to specify the types of the
// request maker and chunk processor. The client uses the IRequestMaker
// interface to define request creation for each API. The client uses an
// ApiStreamHandler to process the data stream from the API. It uses the
// IChunkProcessor to define processing for each API response.
//
// Note: Templates vs Dependency Injection
// Templates are used rather than dependency injection to simplify the client
// interface:
// 1. Calling client creators don't have to know about or instantiate internal
//    components of the client.
// 2. Templates can be instantiated multiple times while DI depends on a single
//    instance.
// 3. Templates avoid dependency drilling: passing the dependencies through
//    multiple layers of the code to the function that uses it.
// 4. Templates avoid using storage for the dependencies, which is necessary
//    with DI if the dependencies are not passed through the layers of the code.
// 5. Testing is equally simple with templates or DI. Test classes for these
//    interfaces are defined in tests/.
// 6. Templates incur no runtime overhead for virtual function calls, which
//    would be occur with DI. They produce faster code, although the
//    difference is negligible in this case. Also, the binary size is larger
//    with templates, about 2% here.

template <typename CP, typename RM> class ApiClient {
public:
  explicit ApiClient(std::string api_key) : request_maker_(api_key) {
    static_assert(std::is_base_of_v<IChunkProcessor, CP>);
    static_assert(std::is_base_of_v<IRequestMaker, RM>);
  }

  tl::expected<std::string, APIError> CallApi(const std::string &prompt,
                                              bool print = false);

private:
  static const int kSuccessCode = 200;
  RM request_maker_;
  ApiStreamHandler<CP> stream_handler_;

  std::string FilterText(std::string &lines);
  bool HandleData(std::string &lines, const std::string &filtered_line,
                  bool print);
  bool HandleErrors(const std::string &filtered_line);
  bool ProcessLine(std::string &lines, const std::string &raw_line, bool print);
  static std::string FilterTypeErrorString(const std::string &raw_line);
  static std::string FilterByLine(const std::string &raw_line);
  static APIError ParseError(const std::string &error_str);
};
