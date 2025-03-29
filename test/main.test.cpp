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

TEST_F(StreamCaptureFixture, HelloWorld) {
  std::cout << "Hello, World!" << std::endl;
  std::string output = GetOutput();
  EXPECT_NE(output.find("Hello, World!"), std::string::npos);
}
