#include "logger.h"

#include <gtest/gtest.h>

#include <iostream>
#include <string>

class StreamCaptureFixture : public ::testing::Test {
 protected:
  void SetUp() override {
    streambuf = std::cout.rdbuf();
    std::cout.rdbuf(stream.rdbuf());
  }

  void TearDown() override { std::cout.rdbuf(streambuf); }

  std::string GetOutput() { return stream.str(); }

 private:
  std::ostringstream stream;
  std::streambuf *streambuf;
};

using TestLogger = StreamCaptureFixture;

TEST_F(TestLogger, LogsToConsole) {
  LogInfo("This is an info message");
  LogDebug("This is a debug message");
  LogWarning("This is a warning message");
  LogError("This is an error message");

  std::string output = GetOutput();

  EXPECT_NE(output.find("INFO"), std::string::npos);
  EXPECT_NE(output.find("This is an info message"), std::string::npos);
  EXPECT_NE(output.find("DEBUG"), std::string::npos);
  EXPECT_NE(output.find("This is a debug message"), std::string::npos);
  EXPECT_NE(output.find("WARNING"), std::string::npos);
  EXPECT_NE(output.find("This is a warning message"), std::string::npos);
  EXPECT_NE(output.find("ERROR"), std::string::npos);
  EXPECT_NE(output.find("This is an error message"), std::string::npos);
}
