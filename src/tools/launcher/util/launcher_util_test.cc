// Copyright 2017 The Bazel Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <windows.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include "src/tools/launcher/util/launcher_util.h"
#include "gtest/gtest.h"

namespace bazel {
namespace launcher {

using std::getenv;
using std::ios;
using std::ofstream;
using std::string;

class LaunchUtilTest : public ::testing::Test {
 protected:
  LaunchUtilTest() {}

  virtual ~LaunchUtilTest() {}

  void SetUp() override {
    char* tmpdir = getenv("TEST_TMPDIR");
    if (tmpdir != NULL) {
      test_tmpdir = string(tmpdir);
    } else {
      tmpdir = getenv("TEMP");
      ASSERT_FALSE(tmpdir == NULL);
      test_tmpdir = string(tmpdir);
    }
  }

  void TearDown() override {}

  string GetTmpDir() { return this->test_tmpdir; }

  // Create an empty file at path
  static void CreateEmptyFile(const string& path) {
    ofstream file_stream(path.c_str(), ios::out | ios::binary);
    file_stream.put('\0');
  }

 private:
  string test_tmpdir;
};

TEST_F(LaunchUtilTest, GetBinaryPathWithoutExtensionTest) {
  ASSERT_EQ("foo", GetBinaryPathWithoutExtension("foo.exe"));
  ASSERT_EQ("foo.sh", GetBinaryPathWithoutExtension("foo.sh.exe"));
  ASSERT_EQ("foo.sh", GetBinaryPathWithoutExtension("foo.sh"));
}

TEST_F(LaunchUtilTest, GetBinaryPathWithExtensionTest) {
  ASSERT_EQ("foo.exe", GetBinaryPathWithExtension("foo"));
  ASSERT_EQ("foo.sh.exe", GetBinaryPathWithExtension("foo.sh.exe"));
  ASSERT_EQ("foo.sh.exe", GetBinaryPathWithExtension("foo.sh"));
}

TEST_F(LaunchUtilTest, GetEscapedArgumentTest) {
  ASSERT_EQ("foo", GetEscapedArgument("foo", true));
  ASSERT_EQ("\"foo bar\"", GetEscapedArgument("foo bar", true));
  ASSERT_EQ("\"\\\"foo bar\\\"\"", GetEscapedArgument("\"foo bar\"", true));
  ASSERT_EQ("foo\\\\bar", GetEscapedArgument("foo\\bar", true));
  ASSERT_EQ("foo\\\"bar", GetEscapedArgument("foo\"bar", true));
  ASSERT_EQ("C:\\\\foo\\\\bar\\\\", GetEscapedArgument("C:\\foo\\bar\\", true));
  ASSERT_EQ("\"C:\\\\foo foo\\\\bar\\\\\"",
            GetEscapedArgument("C:\\foo foo\\bar\\", true));

  ASSERT_EQ("foo\\bar", GetEscapedArgument("foo\\bar", false));
  ASSERT_EQ("C:\\foo\\bar\\", GetEscapedArgument("C:\\foo\\bar\\", false));
  ASSERT_EQ("\"C:\\foo foo\\bar\\\"",
            GetEscapedArgument("C:\\foo foo\\bar\\", false));
}

TEST_F(LaunchUtilTest, DoesFilePathExistTest) {
  string file1 = GetTmpDir() + "/foo";
  string file2 = GetTmpDir() + "/bar";
  CreateEmptyFile(file1);
  ASSERT_TRUE(DoesFilePathExist(file1.c_str()));
  ASSERT_FALSE(DoesFilePathExist(file2.c_str()));
}

TEST_F(LaunchUtilTest, DoesDirectoryPathExistTest) {
  string dir1 = GetTmpDir() + "/dir1";
  string dir2 = GetTmpDir() + "/dir2";
  CreateDirectory(dir1.c_str(), NULL);
  ASSERT_TRUE(DoesDirectoryPathExist(dir1.c_str()));
  ASSERT_FALSE(DoesDirectoryPathExist(dir2.c_str()));
}

TEST_F(LaunchUtilTest, SetAndGetEnvTest) {
  ASSERT_TRUE(SetEnv("foo", "bar"));
  string value;
  ASSERT_TRUE(GetEnv("foo", &value));
  ASSERT_EQ(value, "bar");
  SetEnv("FOO", "");
  ASSERT_FALSE(GetEnv("FOO", &value));
}

}  // namespace launcher
}  // namespace bazel
