#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

#define private public
#include "../src/api_client/api_stream_handler.h"
#include "../src/api_client/claude_chunk_processor.h"

TEST_CASE("ClaudeChunkProcessor extractDataSections extracts valid examples") {
  ApiStreamHandler<ClaudeChunkProcessor> cp;

  auto sections = cp._extract_data_sections("data: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: 0 or more spaces after "data:"
  sections = cp._extract_data_sections("data:foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Allowed: Only one linefeed between data sections.
  sections = cp._extract_data_sections("data: foo\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  // Allowed: 0 or more spaces before and after "data:"
  sections = cp._extract_data_sections("\n data:  foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = cp._extract_data_sections("\n\ndata: foo");
  CHECK(sections == std::vector<std::string>{"foo"});

  // Note: Spaces after the data section are returned.
  sections = cp._extract_data_sections("\n\ndata: foo   ");
  CHECK(sections == std::vector<std::string>{"foo   "});

  // Empty data section is allowed.
  sections = cp._extract_data_sections("data: ");
  CHECK(sections == std::vector<std::string>{""});

  sections = cp._extract_data_sections("data: foo\n");
  CHECK(sections == std::vector<std::string>{"foo"});

  sections = cp._extract_data_sections("data: foo\n\ndata: bar\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections = cp._extract_data_sections("data: foo\n\ndata: bar\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar"});

  sections =
      cp._extract_data_sections("data: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extract_data_sections("data: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extract_data_sections("data: foo\n\ndata: bar\n\ndata: baz");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      cp._extract_data_sections("\ndata: foo\n\ndata: bar\n\ndata: baz\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      cp._extract_data_sections("\n\ndata: foo\n\ndata: bar\n\ndata: baz\n\n");
  CHECK(sections == std::vector<std::string>{"foo", "bar", "baz"});
}

TEST_CASE("_extract_data_sections invalid examples") {
  ApiStreamHandler<ClaudeChunkProcessor> cp;

  auto sections = cp._extract_data_sections("dat: foo");
  CHECK(sections == std::vector<std::string>{});

  sections = cp._extract_data_sections("data: foo\nbar");
  CHECK(sections == std::vector<std::string>{"foo"});
}
