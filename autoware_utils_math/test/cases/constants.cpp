// Copyright 2020 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autoware_utils_math/constants.hpp"

#include <gtest/gtest.h>

TEST(constants, pi)  // NOLINT for gtest
{
  using autoware_utils_math::pi;

  EXPECT_DOUBLE_EQ(pi, 3.14159265358979323846);
}

TEST(constants, gravity)  // NOLINT for gtest
{
  using autoware_utils_math::gravity;

  EXPECT_DOUBLE_EQ(gravity, 9.80665);
}
