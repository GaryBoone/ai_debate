#include <catch2/catch_test_macros.hpp>

#define private public
#include "../src/chunk_processor.h"

TEST_CASE("extractDataSections extracts valid examples",
          "[extractDataSections]") {
  ChunkProcessor cp;

  std::vector<std::string> sections = cp._extractDataSections("data: foo");
  REQUIRE(sections == std::vector<std::string>{"foo"});

  // Allowed: 0 or more spaces after "data:"
  sections = cp._extractDataSections("data:foo");
  REQUIRE(sections == std::vector<std::string>{"foo"});

  // Allowed: Only one linefeed between data sections.
  sections = cp._extractDataSections("data: foo\ndata: bar\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar"});

  // Allowed: 0 or more spaces before and after "data:"
  sections = cp._extractDataSections("\n data:  foo");
  REQUIRE(sections == std::vector<std::string>{"foo"});

  sections = cp._extractDataSections("\n\ndata: foo");
  REQUIRE(sections == std::vector<std::string>{"foo"});

  // Note: Spaces after the data section are returned.
  sections = cp._extractDataSections("\n\ndata: foo   ");
  REQUIRE(sections == std::vector<std::string>{"foo   "});

  // Empty data section is allowed.
  sections = cp._extractDataSections("data: ");
  REQUIRE(sections == std::vector<std::string>{""});

  sections = cp._extractDataSections("data: foo\n");
  REQUIRE(sections == std::vector<std::string>{"foo"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("data: foo\n\ndata: bar\n\ndata: baz");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections = cp._extractDataSections("\ndata: foo\n\ndata: bar\n\ndata: baz\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar", "baz"});

  sections =
      cp._extractDataSections("\n\ndata: foo\n\ndata: bar\n\ndata: baz\n\n");
  REQUIRE(sections == std::vector<std::string>{"foo", "bar", "baz"});
}

TEST_CASE("extractDataSections invalid examples", "[extractDataSections]") {
  ChunkProcessor cp;

  std::vector<std::string> sections = cp._extractDataSections("dat: foo");
  REQUIRE(sections == std::vector<std::string>{});

  sections = cp._extractDataSections("data: foo\nbar");
  REQUIRE(sections == std::vector<std::string>{"foo"});
}

TEST_CASE("An example test case", "[example]") {
  REQUIRE(1 == 1); // An example test that always passes
  //   REQUIRE(
  //       cp.parse_chunk_data(
  //           R"({"choices":[{"index":0,"delta":{"role":"assistant","content":"foo"},"logprobs":null,"finish_reason":null}]})")
  //           ==
  //       true);
  //   REQUIRE(1 == 1);
}
