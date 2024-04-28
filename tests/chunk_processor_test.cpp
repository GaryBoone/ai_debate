// Only one testing file should have the main function.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest/doctest.h"

#define private public
#include "../src/api_client/api_stream_handler.h"
#include "../src/api_client/claude_chunk_processor.h"

using CLIENT = ApiStreamHandler<ClaudeChunkProcessor>;

// NOLINTBEGIN(cppcoreguidelines-avoid-do-while)
TEST_CASE( // NOLINT(readability-function-cognitive-complexity)
    "ClaudeChunkProcessor extractDataSections extracts valid examples") {

  auto sections = CLIENT::ExtractDataSections("data: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: 0 or more spaces after "data:"
  sections = CLIENT::ExtractDataSections("data:foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: Only one linefeed between data sections.
  sections = CLIENT::ExtractDataSections("data: foo\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  // Allowed: 0 or more spaces before and after "data:"
  sections = CLIENT::ExtractDataSections("\n data:  foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = CLIENT::ExtractDataSections("\n\ndata: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Note: Spaces after the data section are returned.
  sections = CLIENT::ExtractDataSections("\n\ndata: foo   ");
  CHECK(sections == std::vector<std::string>{"foo   "});

  // Empty data section is allowed.
  sections = CLIENT::ExtractDataSections("data: ");
  CHECK(sections == std::vector<std::string>{""});

  sections = CLIENT::ExtractDataSections("data: foo\n");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = CLIENT::ExtractDataSections("data: foo\n\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections = CLIENT::ExtractDataSections("data: foo\n\ndata: bar\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections =
      CLIENT::ExtractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      CLIENT::ExtractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = CLIENT::ExtractDataSections("data: foo\n\ndata: bar\n\ndata: baz");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      CLIENT::ExtractDataSections("\ndata: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = CLIENT::ExtractDataSections(
      "\n\ndata: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});
}

TEST_CASE("_extract_data_sections invalid examples") {

  auto sections = CLIENT::ExtractDataSections("dat: foo");
  CHECK(sections == std::vector<std::string>{});

  sections = CLIENT::ExtractDataSections("data: foo\nbar");
  CHECK(sections == std::vector<std::string>{"foo"});
}
// NOLINTEND(cppcoreguidelines-avoid-do-while)
