#include "doctest/doctest.h"

#define private public
#include "../src/api_client/api_client.h"
#include "../src/api_client/claude_chunk_processor.h"

using CLIENT = ApiClient<ClaudeChunkProcessor>;

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "FilterTypeErrorString filters correctly") {
  CHECK(CLIENT::FilterTypeErrorString("").empty());

  CHECK(CLIENT::FilterTypeErrorString(R"("type": "error",)").empty());

  CHECK(CLIENT::FilterTypeErrorString(R"(foo)") == "foo");

  CHECK(CLIENT::FilterTypeErrorString(R"(\n\n\n)") == R"(\n\n\n)");

  CHECK(CLIENT::FilterTypeErrorString(R"(\n\n"type":    "error",\n)") ==
        R"(\n\n\n)");

  CHECK(CLIENT::FilterTypeErrorString(R"(\nfoo\n"type":    "error",foo\n)") ==
        R"(\nfoo\nfoo\n)");
}

TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "FilterByLine filters correctly") {
  CHECK(CLIENT::FilterByLine("").empty());

  // Specific filterable events.
  CHECK(CLIENT::FilterByLine("event: message_start").empty());
  CHECK(CLIENT::FilterByLine("event: ping").empty());
  CHECK(CLIENT::FilterByLine("event: content_block_start").empty());
  CHECK(CLIENT::FilterByLine("event: content_block_delta").empty());
  CHECK(CLIENT::FilterByLine("event: content_block_stop").empty());
  CHECK(CLIENT::FilterByLine("event: message_delta").empty());
  CHECK(CLIENT::FilterByLine("event: message_stop").empty());

  CHECK(CLIENT::FilterByLine("event: non_filter_event\n") ==
        "event: non_filter_event\n");

  // Mixed content, some lines should be filtered, others preserved. Note that
  // the last line does not have a newline character but one will be added.
  std::string mixed_input = "event: message_start\n"
                            "event: ping\n"
                            "normal message\n"
                            "event: message_stop\n"
                            "another normal message";
  std::string expected_output = "normal message\n"
                                "another normal message\n";
  CHECK(CLIENT::FilterByLine(mixed_input) == expected_output);

  // Test with lines that are similar to filterable events but not exact matches
  std::string non_exact_matches = "event: message_starts\n"
                                  "event ping\n"
                                  "event: message_\n";
  std::string expected_non_matches_output = "event: message_starts\n"
                                            "event ping\n"
                                            "event: message_\n";
  CHECK(CLIENT::FilterByLine(non_exact_matches) == expected_non_matches_output);

  // Test handling of Windows (\r\n) and Unix (\n) style newlines
  std::string mixed_newline_input = "event: message_start\r\n"
                                    "event: ping\n"
                                    "valid line\r\n"
                                    "another valid line\n";
  std::string expected_newline_output = "valid line\n"
                                        "another valid line\n";
  CHECK(CLIENT::FilterByLine(mixed_newline_input) == expected_newline_output);

  // Test input where only newlines or carriage returns are present
  CHECK(CLIENT::FilterByLine("\n").empty());
  CHECK(CLIENT::FilterByLine("\r\n").empty());
}

TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "Testing the ApiClient::ParseError") {
  // Valid JSON with correct fields.
  std::string valid_json = R"({"error": {"message": "An error occurred"}})";
  APIError error = CLIENT::ParseError(valid_json);
  CHECK(error.Type() == APIErrorType::kApiReturnedError);
  CHECK(error.Message() == "An error occurred");

  // JSON without the 'error' field.
  std::string no_error_field_json =
      R"({"invalid": {"message": "An error occurred"}})";
  error = CLIENT::ParseError(no_error_field_json);
  CHECK(error.Type() == APIErrorType::kResponseJsonParse);
  CHECK(error.Message() == "unable to parse API error; no 'error' field");

  std::string no_error_field_json2 = R"({"error": {}})";
  error = CLIENT::ParseError(no_error_field_json2);
  CHECK(error.Type() == APIErrorType::kResponseJsonParse);
  CHECK(error.Message() == "unable to parse API error; no 'error' field");

  // JSON with 'error' but missing 'message' field.
  std::string no_message_field_json = R"({"error": {"foo": "bar"}})";
  error = CLIENT::ParseError(no_message_field_json);
  CHECK(error.Type() == APIErrorType::kResponseJsonParse);
  CHECK(error.Message() == "unable to parse API error; no 'message' field");

  // Malformed JSON.
  std::string incorrect_type_json =
      R"({"error": "This should be an object, not a string"})";
  error = CLIENT::ParseError(incorrect_type_json);
  CHECK(error.Type() == APIErrorType::kResponseJsonParse);
  std::string expected_error_msg = "cannot use value() with string";
  CHECK(error.Message().find(expected_error_msg) != std::string::npos);
}

// NOLINTEND(cppcoreguidelines-avoid-do-while)
