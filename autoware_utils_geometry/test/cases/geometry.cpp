// Copyright 2020-2024 Tier IV, Inc.
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

#include "autoware_utils_geometry/geometry.hpp"

#include "autoware_utils_geometry/boost_geometry.hpp"
#include "autoware_utils_geometry/ear_clipping.hpp"
#include "autoware_utils_geometry/random_concave_polygon.hpp"
#include "autoware_utils_geometry/random_convex_polygon.hpp"
#include "autoware_utils_geometry/sat_2d.hpp"
#include "autoware_utils_math/unit_conversion.hpp"
#include "autoware_utils_system/stop_watch.hpp"

#include <geometry_msgs/msg/point32.hpp>

#include <boost/geometry/algorithms/correct.hpp>
#include <boost/geometry/io/wkt/write.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

constexpr double epsilon = 1e-6;

TEST(geometry, get_point)
{
  using autoware_utils_geometry::get_point;

  const double x_ans = 1.0;
  const double y_ans = 2.0;
  const double z_ans = 3.0;

  {
    struct AnyPoint
    {
      double x;
      double y;
      double z;
    };
    const AnyPoint p{x_ans, y_ans, z_ans};
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    geometry_msgs::msg::Point p;
    p.x = x_ans;
    p.y = y_ans;
    p.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    geometry_msgs::msg::Pose p;
    p.position.x = x_ans;
    p.position.y = y_ans;
    p.position.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    geometry_msgs::msg::PoseStamped p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    geometry_msgs::msg::PoseWithCovarianceStamped p;
    p.pose.pose.position.x = x_ans;
    p.pose.pose.position.y = y_ans;
    p.pose.pose.position.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    autoware_planning_msgs::msg::PathPoint p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }

  {
    autoware_planning_msgs::msg::TrajectoryPoint p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    const geometry_msgs::msg::Point p_out = get_point(p);
    EXPECT_DOUBLE_EQ(p_out.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.z, z_ans);
  }
}

TEST(geometry, get_pose)
{
  using autoware_utils_geometry::get_pose;

  const double x_ans = 1.0;
  const double y_ans = 2.0;
  const double z_ans = 3.0;
  const double q_x_ans = 0.1;
  const double q_y_ans = 0.2;
  const double q_z_ans = 0.3;
  const double q_w_ans = 0.4;

  {
    geometry_msgs::msg::Pose p;
    p.position.x = x_ans;
    p.position.y = y_ans;
    p.position.z = z_ans;
    p.orientation.x = q_x_ans;
    p.orientation.y = q_y_ans;
    p.orientation.z = q_z_ans;
    p.orientation.w = q_w_ans;
    const geometry_msgs::msg::Pose p_out = get_pose(p);
    EXPECT_DOUBLE_EQ(p_out.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, q_w_ans);
  }

  {
    geometry_msgs::msg::PoseStamped p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    p.pose.orientation.x = q_x_ans;
    p.pose.orientation.y = q_y_ans;
    p.pose.orientation.z = q_z_ans;
    p.pose.orientation.w = q_w_ans;
    const geometry_msgs::msg::Pose p_out = get_pose(p);
    EXPECT_DOUBLE_EQ(p_out.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, q_w_ans);
  }

  {
    autoware_planning_msgs::msg::PathPoint p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    p.pose.orientation.x = q_x_ans;
    p.pose.orientation.y = q_y_ans;
    p.pose.orientation.z = q_z_ans;
    p.pose.orientation.w = q_w_ans;
    const geometry_msgs::msg::Pose p_out = get_pose(p);
    EXPECT_DOUBLE_EQ(p_out.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, q_w_ans);
  }

  {
    autoware_planning_msgs::msg::TrajectoryPoint p;
    p.pose.position.x = x_ans;
    p.pose.position.y = y_ans;
    p.pose.position.z = z_ans;
    p.pose.orientation.x = q_x_ans;
    p.pose.orientation.y = q_y_ans;
    p.pose.orientation.z = q_z_ans;
    p.pose.orientation.w = q_w_ans;
    const geometry_msgs::msg::Pose p_out = get_pose(p);
    EXPECT_DOUBLE_EQ(p_out.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, q_w_ans);
  }
}

TEST(geometry, get_longitudinal_velocity)
{
  using autoware_utils_geometry::get_longitudinal_velocity;

  const double velocity = 1.0;

  {
    autoware_planning_msgs::msg::PathPoint p;
    p.longitudinal_velocity_mps = velocity;
    EXPECT_DOUBLE_EQ(get_longitudinal_velocity(p), velocity);
  }

  {
    autoware_planning_msgs::msg::TrajectoryPoint p;
    p.longitudinal_velocity_mps = velocity;
    EXPECT_DOUBLE_EQ(get_longitudinal_velocity(p), velocity);
  }
}

TEST(geometry, set_pose)
{
  using autoware_utils_geometry::set_pose;

  const double x_ans = 1.0;
  const double y_ans = 2.0;
  const double z_ans = 3.0;
  const double q_x_ans = 0.1;
  const double q_y_ans = 0.2;
  const double q_z_ans = 0.3;
  const double q_w_ans = 0.4;

  geometry_msgs::msg::Pose p;
  p.position.x = x_ans;
  p.position.y = y_ans;
  p.position.z = z_ans;
  p.orientation.x = q_x_ans;
  p.orientation.y = q_y_ans;
  p.orientation.z = q_z_ans;
  p.orientation.w = q_w_ans;

  {
    geometry_msgs::msg::Pose p_out{};
    set_pose(p, p_out);
    EXPECT_DOUBLE_EQ(p_out.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, q_w_ans);
  }

  {
    geometry_msgs::msg::PoseStamped p_out{};
    set_pose(p, p_out);
    EXPECT_DOUBLE_EQ(p_out.pose.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.w, q_w_ans);
  }

  {
    autoware_planning_msgs::msg::PathPoint p_out{};
    set_pose(p, p_out);
    EXPECT_DOUBLE_EQ(p_out.pose.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.w, q_w_ans);
  }

  {
    autoware_planning_msgs::msg::TrajectoryPoint p_out{};
    set_pose(p, p_out);
    EXPECT_DOUBLE_EQ(p_out.pose.position.x, x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.y, y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.position.z, z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.x, q_x_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.y, q_y_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.z, q_z_ans);
    EXPECT_DOUBLE_EQ(p_out.pose.orientation.w, q_w_ans);
  }
}

TEST(geometry, set_orientation)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::set_orientation;
  using autoware_utils_math::deg2rad;

  geometry_msgs::msg::Pose p;
  const auto orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));
  set_orientation(orientation, p);

  EXPECT_DOUBLE_EQ(p.orientation.x, orientation.x);
  EXPECT_DOUBLE_EQ(p.orientation.y, orientation.y);
  EXPECT_DOUBLE_EQ(p.orientation.z, orientation.z);
  EXPECT_DOUBLE_EQ(p.orientation.w, orientation.w);
}

TEST(geometry, set_longitudinal_velocity)
{
  using autoware_utils_geometry::set_longitudinal_velocity;

  const double velocity = 1.0;

  {
    autoware_planning_msgs::msg::PathPoint p{};
    set_longitudinal_velocity(velocity, p);
    EXPECT_DOUBLE_EQ(p.longitudinal_velocity_mps, velocity);
  }

  {
    autoware_planning_msgs::msg::TrajectoryPoint p{};
    set_longitudinal_velocity(velocity, p);
    EXPECT_DOUBLE_EQ(p.longitudinal_velocity_mps, velocity);
  }
}

TEST(geometry, create_point)
{
  using autoware_utils_geometry::create_point;

  const geometry_msgs::msg::Point p_out = create_point(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(p_out.x, 1.0);
  EXPECT_DOUBLE_EQ(p_out.y, 2.0);
  EXPECT_DOUBLE_EQ(p_out.z, 3.0);
}

TEST(geometry, create_quaternion)
{
  using autoware_utils_geometry::create_quaternion;

  // (0.18257419, 0.36514837, 0.54772256, 0.73029674) is normalized quaternion of (1, 2, 3, 4)
  const geometry_msgs::msg::Quaternion q_out =
    create_quaternion(0.18257419, 0.36514837, 0.54772256, 0.73029674);
  EXPECT_DOUBLE_EQ(q_out.x, 0.18257419);
  EXPECT_DOUBLE_EQ(q_out.y, 0.36514837);
  EXPECT_DOUBLE_EQ(q_out.z, 0.54772256);
  EXPECT_DOUBLE_EQ(q_out.w, 0.73029674);
}

TEST(geometry, create_translation)
{
  using autoware_utils_geometry::create_translation;

  const geometry_msgs::msg::Vector3 v_out = create_translation(1.0, 2.0, 3.0);
  EXPECT_DOUBLE_EQ(v_out.x, 1.0);
  EXPECT_DOUBLE_EQ(v_out.y, 2.0);
  EXPECT_DOUBLE_EQ(v_out.z, 3.0);
}

TEST(geometry, create_quaternion_from_rpy)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_math::deg2rad;

  {
    const auto q_out = create_quaternion_from_rpy(0, 0, 0);
    EXPECT_DOUBLE_EQ(q_out.x, 0.0);
    EXPECT_DOUBLE_EQ(q_out.y, 0.0);
    EXPECT_DOUBLE_EQ(q_out.z, 0.0);
    EXPECT_DOUBLE_EQ(q_out.w, 1.0);
  }

  {
    const auto q_out = create_quaternion_from_rpy(0, 0, deg2rad(90));
    EXPECT_DOUBLE_EQ(q_out.x, 0.0);
    EXPECT_DOUBLE_EQ(q_out.y, 0.0);
    EXPECT_DOUBLE_EQ(q_out.z, 0.70710678118654757);
    EXPECT_DOUBLE_EQ(q_out.w, 0.70710678118654757);
  }

  {
    const auto q_out = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));
    EXPECT_DOUBLE_EQ(q_out.x, 0.17677669529663687);
    EXPECT_DOUBLE_EQ(q_out.y, 0.30618621784789724);
    EXPECT_DOUBLE_EQ(q_out.z, 0.17677669529663692);
    EXPECT_DOUBLE_EQ(q_out.w, 0.91855865354369193);
  }
}

TEST(geometry, create_quaternion_from_yaw)
{
  using autoware_utils_geometry::create_quaternion_from_yaw;
  using autoware_utils_math::deg2rad;

  {
    const auto q_out = create_quaternion_from_yaw(0);
    EXPECT_DOUBLE_EQ(q_out.x, 0.0);
    EXPECT_DOUBLE_EQ(q_out.y, 0.0);
    EXPECT_DOUBLE_EQ(q_out.z, 0.0);
    EXPECT_DOUBLE_EQ(q_out.w, 1.0);
  }

  {
    const auto q_out = create_quaternion_from_yaw(deg2rad(90));
    EXPECT_DOUBLE_EQ(q_out.x, 0.0);
    EXPECT_DOUBLE_EQ(q_out.y, 0.0);
    EXPECT_DOUBLE_EQ(q_out.z, 0.70710678118654757);
    EXPECT_DOUBLE_EQ(q_out.w, 0.70710678118654757);
  }

  {
    const auto q_out = create_quaternion_from_yaw(deg2rad(30));
    EXPECT_DOUBLE_EQ(q_out.x, 0.0);
    EXPECT_DOUBLE_EQ(q_out.y, 0.0);
    EXPECT_DOUBLE_EQ(q_out.z, 0.25881904510252074);
    EXPECT_DOUBLE_EQ(q_out.w, 0.96592582628906831);
  }
}

TEST(geometry, calc_elevation_angle)
{
  using autoware_utils_geometry::calc_elevation_angle;
  using autoware_utils_geometry::create_point;
  using autoware_utils_math::deg2rad;

  {
    const auto p1 = create_point(1.0, 1.0, 1.0);
    const auto p2 = create_point(1.0, 1.0, -10.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(-90.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 0.0);
    const auto p2 = create_point(1.0, 0.0, -std::sqrt(3.0));
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(-60.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, -1.0);
    const auto p2 = create_point(0.0, 1.0, -2.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(-45.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 1.0);
    const auto p2 = create_point(1.0, 1.0, 1.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(0.0), epsilon);
  }
  {
    const auto p1 = create_point(-100, -100, 0.0);
    const auto p2 = create_point(0.0, 0.0, 0.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(0.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 1.0);
    const auto p2 = create_point(0.0, 1.0, 2.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(45.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 0.0);
    const auto p2 = create_point(1.0, 0.0, std::sqrt(3.0));
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(60.0), epsilon);
  }
  {
    const auto p1 = create_point(1.0, 1.0, 1.0);
    const auto p2 = create_point(1.0, 1.0, 10.0);
    EXPECT_NEAR(calc_elevation_angle(p1, p2), deg2rad(90.0), epsilon);
  }
}

TEST(geometry, calc_azimuth_angle)
{
  using autoware_utils_geometry::calc_azimuth_angle;
  using autoware_utils_geometry::create_point;
  using autoware_utils_math::deg2rad;

  {
    const auto p1 = create_point(0.0, 0.0, 9.0);
    const auto p2 = create_point(-100, -epsilon, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(-180.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 2.0);
    const auto p2 = create_point(-1.0, -1.0, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(-135.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 10.0, 0.0);
    const auto p2 = create_point(0.0, 0.0, 6.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(-90.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 0.0);
    const auto p2 = create_point(1.0, -1.0, 4.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(-45.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 1.0, 3.3);
    const auto p2 = create_point(10.0, 1.0, -10.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(0.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 2.0);
    const auto p2 = create_point(1.0, 1.0, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(45.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 10.0);
    const auto p2 = create_point(0.0, 10.0, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(90.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 2.0);
    const auto p2 = create_point(-1.0, 1.0, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(135.0), epsilon);
  }
  {
    const auto p1 = create_point(0.0, 0.0, 9.0);
    const auto p2 = create_point(-100, epsilon, 0.0);
    EXPECT_NEAR(calc_azimuth_angle(p1, p2), deg2rad(180.0), epsilon);
  }
}

TEST(geometry, calc_distance2d)
{
  using autoware_utils_geometry::calc_distance2d;

  geometry_msgs::msg::Point point;
  point.x = 1.0;
  point.y = 2.0;
  point.z = 3.0;

  geometry_msgs::msg::Pose pose;
  pose.position.x = 5.0;
  pose.position.y = 5.0;
  pose.position.z = 4.0;

  EXPECT_DOUBLE_EQ(calc_distance2d(point, pose), 5.0);
}

TEST(geometry, calc_squared_distance2d)
{
  using autoware_utils_geometry::calc_squared_distance2d;

  geometry_msgs::msg::Point point;
  point.x = 1.0;
  point.y = 2.0;
  point.z = 3.0;

  geometry_msgs::msg::Pose pose;
  pose.position.x = 5.0;
  pose.position.y = 5.0;
  pose.position.z = 4.0;

  EXPECT_DOUBLE_EQ(calc_squared_distance2d(point, pose), 25.0);
}

TEST(geometry, calc_distance3d)
{
  using autoware_utils_geometry::calc_distance3d;

  geometry_msgs::msg::Point point;
  point.x = 1.0;
  point.y = 2.0;
  point.z = 3.0;

  geometry_msgs::msg::Pose pose;
  pose.position.x = 3.0;
  pose.position.y = 4.0;
  pose.position.z = 4.0;

  EXPECT_DOUBLE_EQ(calc_distance3d(point, pose), 3.0);
}

TEST(geometry, get_rpy)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::get_rpy;
  using autoware_utils_math::deg2rad;

  {
    const double ans_roll = deg2rad(5);
    const double ans_pitch = deg2rad(10);
    const double ans_yaw = deg2rad(15);
    const auto quat = create_quaternion_from_rpy(ans_roll, ans_pitch, ans_yaw);
    const auto rpy = get_rpy(quat);
    EXPECT_NEAR(rpy.x, ans_roll, epsilon);
    EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
    EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
  }
  {
    const double ans_roll = deg2rad(0);
    const double ans_pitch = deg2rad(5);
    const double ans_yaw = deg2rad(-10);
    const auto quat = create_quaternion_from_rpy(ans_roll, ans_pitch, ans_yaw);
    const auto rpy = get_rpy(quat);
    EXPECT_NEAR(rpy.x, ans_roll, epsilon);
    EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
    EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
  }
  {
    const double ans_roll = deg2rad(30);
    const double ans_pitch = deg2rad(-20);
    const double ans_yaw = deg2rad(0);
    const auto quat = create_quaternion_from_rpy(ans_roll, ans_pitch, ans_yaw);
    const auto rpy = get_rpy(quat);
    EXPECT_NEAR(rpy.x, ans_roll, epsilon);
    EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
    EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
  }
}

TEST(geometry, get_rpy_wrapper)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::get_rpy;
  using autoware_utils_math::deg2rad;

  {
    const double ans_roll = deg2rad(45);
    const double ans_pitch = deg2rad(25);
    const double ans_yaw = deg2rad(-5);
    const auto quat = create_quaternion_from_rpy(ans_roll, ans_pitch, ans_yaw);

    // geometry_msgs::Pose
    {
      geometry_msgs::msg::Pose pose{};
      pose.orientation = quat;
      const auto rpy = get_rpy(pose);
      EXPECT_NEAR(rpy.x, ans_roll, epsilon);
      EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
      EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
    }
    // geometry_msgs::PoseStamped
    {
      geometry_msgs::msg::PoseStamped pose{};
      pose.pose.orientation = quat;
      const auto rpy = get_rpy(pose);
      EXPECT_NEAR(rpy.x, ans_roll, epsilon);
      EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
      EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
    }
    // geometry_msgs::PoseWithCovarianceStamped
    {
      geometry_msgs::msg::PoseWithCovarianceStamped pose{};
      pose.pose.pose.orientation = quat;
      const auto rpy = get_rpy(pose);
      EXPECT_NEAR(rpy.x, ans_roll, epsilon);
      EXPECT_NEAR(rpy.y, ans_pitch, epsilon);
      EXPECT_NEAR(rpy.z, ans_yaw, epsilon);
    }
  }
}

TEST(geometry, transform2pose)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::transform2pose;
  using autoware_utils_math::deg2rad;

  {
    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.translation.z = 3.0;
    transform.rotation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Pose pose = transform2pose(transform);

    EXPECT_DOUBLE_EQ(transform.translation.x, pose.position.x);
    EXPECT_DOUBLE_EQ(transform.translation.y, pose.position.y);
    EXPECT_DOUBLE_EQ(transform.translation.z, pose.position.z);
    EXPECT_DOUBLE_EQ(transform.rotation.x, pose.orientation.x);
    EXPECT_DOUBLE_EQ(transform.rotation.y, pose.orientation.y);
    EXPECT_DOUBLE_EQ(transform.rotation.z, pose.orientation.z);
    EXPECT_DOUBLE_EQ(transform.rotation.w, pose.orientation.w);
  }

  {
    geometry_msgs::msg::TransformStamped transform_stamped;
    transform_stamped.header.frame_id = "test";
    transform_stamped.header.stamp = rclcpp::Time(2.0);
    transform_stamped.transform.translation.x = 1.0;
    transform_stamped.transform.translation.y = 2.0;
    transform_stamped.transform.translation.z = 3.0;
    transform_stamped.transform.rotation =
      create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::PoseStamped pose_stamped = transform2pose(transform_stamped);

    EXPECT_EQ(transform_stamped.header.frame_id, pose_stamped.header.frame_id);
    EXPECT_DOUBLE_EQ(
      rclcpp::Time(transform_stamped.header.stamp).seconds(),
      rclcpp::Time(pose_stamped.header.stamp).seconds());

    EXPECT_DOUBLE_EQ(transform_stamped.transform.translation.x, pose_stamped.pose.position.x);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.translation.y, pose_stamped.pose.position.y);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.translation.z, pose_stamped.pose.position.z);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.rotation.x, pose_stamped.pose.orientation.x);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.rotation.y, pose_stamped.pose.orientation.y);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.rotation.z, pose_stamped.pose.orientation.z);
    EXPECT_DOUBLE_EQ(transform_stamped.transform.rotation.w, pose_stamped.pose.orientation.w);
  }
}

TEST(geometry, pose2transform)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::pose2transform;
  using autoware_utils_math::deg2rad;

  {
    geometry_msgs::msg::Pose pose;
    pose.position.x = 1.0;
    pose.position.y = 2.0;
    pose.position.z = 3.0;
    pose.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Transform transform = pose2transform(pose);

    EXPECT_DOUBLE_EQ(pose.position.x, transform.translation.x);
    EXPECT_DOUBLE_EQ(pose.position.y, transform.translation.y);
    EXPECT_DOUBLE_EQ(pose.position.z, transform.translation.z);
    EXPECT_DOUBLE_EQ(pose.orientation.x, transform.rotation.x);
    EXPECT_DOUBLE_EQ(pose.orientation.y, transform.rotation.y);
    EXPECT_DOUBLE_EQ(pose.orientation.z, transform.rotation.z);
    EXPECT_DOUBLE_EQ(pose.orientation.w, transform.rotation.w);
  }

  {
    geometry_msgs::msg::PoseStamped pose_stamped;
    pose_stamped.header.frame_id = "test";
    pose_stamped.header.stamp = rclcpp::Time(2.0);
    pose_stamped.pose.position.x = 1.0;
    pose_stamped.pose.position.y = 2.0;
    pose_stamped.pose.position.z = 3.0;
    pose_stamped.pose.orientation =
      create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));
    const std::string child_frame_id = "child";

    const geometry_msgs::msg::TransformStamped transform_stamped =
      pose2transform(pose_stamped, child_frame_id);

    EXPECT_EQ(pose_stamped.header.frame_id, transform_stamped.header.frame_id);
    EXPECT_EQ(child_frame_id, transform_stamped.child_frame_id);
    EXPECT_DOUBLE_EQ(
      rclcpp::Time(pose_stamped.header.stamp).seconds(),
      rclcpp::Time(transform_stamped.header.stamp).seconds());

    EXPECT_DOUBLE_EQ(pose_stamped.pose.position.x, transform_stamped.transform.translation.x);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.position.y, transform_stamped.transform.translation.y);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.position.z, transform_stamped.transform.translation.z);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.orientation.x, transform_stamped.transform.rotation.x);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.orientation.y, transform_stamped.transform.rotation.y);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.orientation.z, transform_stamped.transform.rotation.z);
    EXPECT_DOUBLE_EQ(pose_stamped.pose.orientation.w, transform_stamped.transform.rotation.w);
  }
}

TEST(geometry, point_2_tf_vector)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::point_2_tf_vector;
  using autoware_utils_math::deg2rad;

  // Point
  {
    geometry_msgs::msg::Point src;
    src.x = 1.0;
    src.y = 2.0;
    src.z = 3.0;

    geometry_msgs::msg::Point dst;
    dst.x = 10.0;
    dst.y = 5.0;
    dst.z = -5.0;

    const auto vec = point_2_tf_vector(src, dst);

    EXPECT_DOUBLE_EQ(vec.x(), 9.0);
    EXPECT_DOUBLE_EQ(vec.y(), 3.0);
    EXPECT_DOUBLE_EQ(vec.z(), -8.0);
  }

  // Pose
  {
    geometry_msgs::msg::Pose src;
    src.position.x = 1.0;
    src.position.y = 2.0;
    src.position.z = 3.0;
    src.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    geometry_msgs::msg::Pose dst;
    dst.position.x = 10.0;
    dst.position.y = 5.0;
    dst.position.z = -5.0;
    dst.orientation = create_quaternion_from_rpy(deg2rad(10), deg2rad(10), deg2rad(10));

    const auto vec = point_2_tf_vector(src, dst);

    EXPECT_DOUBLE_EQ(vec.x(), 9.0);
    EXPECT_DOUBLE_EQ(vec.y(), 3.0);
    EXPECT_DOUBLE_EQ(vec.z(), -8.0);
  }

  // Point and Pose
  {
    geometry_msgs::msg::Point src;
    src.x = 1.0;
    src.y = 2.0;
    src.z = 3.0;

    geometry_msgs::msg::Pose dst;
    dst.position.x = 10.0;
    dst.position.y = 5.0;
    dst.position.z = -5.0;
    dst.orientation = create_quaternion_from_rpy(deg2rad(10), deg2rad(10), deg2rad(10));

    const auto vec = point_2_tf_vector(src, dst);

    EXPECT_DOUBLE_EQ(vec.x(), 9.0);
    EXPECT_DOUBLE_EQ(vec.y(), 3.0);
    EXPECT_DOUBLE_EQ(vec.z(), -8.0);
  }
}

TEST(geometry, transform_point)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::Point2d;
  using autoware_utils_geometry::Point3d;
  using autoware_utils_geometry::transform_point;
  using autoware_utils_math::deg2rad;

  {
    const Point2d p(1.0, 2.0);

    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.rotation = create_quaternion_from_rpy(0, 0, deg2rad(30));

    const Point2d p_transformed = transform_point(p, transform);

    EXPECT_DOUBLE_EQ(p_transformed.x(), 0.86602540378443882);
    EXPECT_DOUBLE_EQ(p_transformed.y(), 4.2320508075688767);
  }

  {
    const Point3d p(1.0, 2.0, 3.0);

    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.translation.z = 3.0;
    transform.rotation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const Point3d p_transformed = transform_point(p, transform);

    EXPECT_DOUBLE_EQ(p_transformed.x(), 3.1919872981077804);
    EXPECT_DOUBLE_EQ(p_transformed.y(), 3.5334936490538906);
    EXPECT_DOUBLE_EQ(p_transformed.z(), 5.6160254037844393);
  }

  {
    const Eigen::Vector3d p(1.0, 2.0, 3.0);

    geometry_msgs::msg::Pose pose_transform;
    pose_transform.position.x = 1.0;
    pose_transform.position.y = 2.0;
    pose_transform.position.z = 3.0;
    pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const Eigen::Vector3d p_transformed = transform_point(p, pose_transform);

    EXPECT_DOUBLE_EQ(p_transformed.x(), 3.1919872981077804);
    EXPECT_DOUBLE_EQ(p_transformed.y(), 3.5334936490538906);
    EXPECT_DOUBLE_EQ(p_transformed.z(), 5.6160254037844393);
  }

  {
    geometry_msgs::msg::Point p;
    p.x = 1.0;
    p.y = 2.0;
    p.z = 3.0;

    geometry_msgs::msg::Pose pose_transform;
    pose_transform.position.x = 1.0;
    pose_transform.position.y = 2.0;
    pose_transform.position.z = 3.0;
    pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Point p_transformed = transform_point(p, pose_transform);

    EXPECT_DOUBLE_EQ(p_transformed.x, 3.1919872981077804);
    EXPECT_DOUBLE_EQ(p_transformed.y, 3.5334936490538906);
    EXPECT_DOUBLE_EQ(p_transformed.z, 5.6160254037844393);
  }

  {
    geometry_msgs::msg::Point32 p;
    p.x = 1.0;
    p.y = 2.0;
    p.z = 3.0;

    geometry_msgs::msg::Pose pose_transform;
    pose_transform.position.x = 1.0;
    pose_transform.position.y = 2.0;
    pose_transform.position.z = 3.0;
    pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Point32 p_transformed = transform_point(p, pose_transform);

    EXPECT_DOUBLE_EQ(p_transformed.x, 3.1919872760772705);
    EXPECT_DOUBLE_EQ(p_transformed.y, 3.5334937572479248);
    EXPECT_DOUBLE_EQ(p_transformed.z, 5.616025447845459);
  }
}

TEST(geometry, transform_pose)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::transform_pose;
  using autoware_utils_math::deg2rad;

  geometry_msgs::msg::Pose pose;
  pose.position.x = 2.0;
  pose.position.y = 4.0;
  pose.position.z = 6.0;
  pose.orientation = create_quaternion_from_rpy(deg2rad(10), deg2rad(20), deg2rad(30));

  // with transform
  {
    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.translation.z = 3.0;
    transform.rotation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Pose pose_transformed = transform_pose(pose, transform);

    EXPECT_NEAR(pose_transformed.position.x, 5.3839745962155598, epsilon);
    EXPECT_NEAR(pose_transformed.position.y, 5.0669872981077804, epsilon);
    EXPECT_NEAR(pose_transformed.position.z, 8.2320508075688785, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.x, 0.24304508436548405, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.y, 0.4296803495383052, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.z, 0.40981009820187703, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.w, 0.76704600096616271, epsilon);
  }

  // with pose_transform
  {
    geometry_msgs::msg::Pose pose_transform;
    pose_transform.position.x = 1.0;
    pose_transform.position.y = 2.0;
    pose_transform.position.z = 3.0;
    pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Pose pose_transformed = transform_pose(pose, pose_transform);

    EXPECT_NEAR(pose_transformed.position.x, 5.3839745962155598, epsilon);
    EXPECT_NEAR(pose_transformed.position.y, 5.0669872981077804, epsilon);
    EXPECT_NEAR(pose_transformed.position.z, 8.2320508075688785, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.x, 0.24304508436548405, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.y, 0.4296803495383052, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.z, 0.40981009820187703, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.w, 0.76704600096616271, epsilon);
  }
}

TEST(geometry, inverse_transform_pose)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::inverse_transform_pose;
  using autoware_utils_math::deg2rad;

  geometry_msgs::msg::Pose pose;
  pose.position.x = 2.0;
  pose.position.y = 4.0;
  pose.position.z = 6.0;
  pose.orientation = create_quaternion_from_rpy(deg2rad(10), deg2rad(20), deg2rad(30));

  // with transform
  {
    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.translation.z = 3.0;
    transform.rotation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Pose pose_transformed = inverse_transform_pose(pose, transform);

    EXPECT_NEAR(pose_transformed.position.x, 0.11602540378443926, epsilon);
    EXPECT_NEAR(pose_transformed.position.y, 2.8325317547305482, epsilon);
    EXPECT_NEAR(pose_transformed.position.z, 2.4419872981077804, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.x, -0.17298739392508941, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.y, -0.08189960831908924, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.z, 0.029809019626209146, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.w, 0.98106026219040698, epsilon);
  }

  // with pose_transform
  {
    geometry_msgs::msg::Pose pose_transform;
    pose_transform.position.x = 1.0;
    pose_transform.position.y = 2.0;
    pose_transform.position.z = 3.0;
    pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const geometry_msgs::msg::Pose pose_transformed = inverse_transform_pose(pose, pose_transform);

    EXPECT_NEAR(pose_transformed.position.x, 0.11602540378443926, epsilon);
    EXPECT_NEAR(pose_transformed.position.y, 2.8325317547305482, epsilon);
    EXPECT_NEAR(pose_transformed.position.z, 2.4419872981077804, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.x, -0.17298739392508941, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.y, -0.08189960831908924, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.z, 0.029809019626209146, epsilon);
    EXPECT_NEAR(pose_transformed.orientation.w, 0.98106026219040698, epsilon);
  }
}

TEST(geometry, inverse_transform_point)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::inverse_transform_point;
  using autoware_utils_geometry::inverse_transform_pose;
  using autoware_utils_math::deg2rad;

  geometry_msgs::msg::Pose pose_transform;
  pose_transform.position.x = 1.0;
  pose_transform.position.y = 2.0;
  pose_transform.position.z = 3.0;
  pose_transform.orientation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

  // calc expected values
  geometry_msgs::msg::Pose pose;
  pose.position.x = 2.0;
  pose.position.y = 4.0;
  pose.position.z = 6.0;
  pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));
  const geometry_msgs::msg::Pose pose_transformed = inverse_transform_pose(pose, pose_transform);
  const geometry_msgs::msg::Point expected_p = pose_transformed.position;

  geometry_msgs::msg::Point p;
  p.x = 2.0;
  p.y = 4.0;
  p.z = 6.0;
  const geometry_msgs::msg::Point p_transformed = inverse_transform_point(p, pose_transform);
  EXPECT_NEAR(p_transformed.x, expected_p.x, epsilon);
  EXPECT_NEAR(p_transformed.y, expected_p.y, epsilon);
  EXPECT_NEAR(p_transformed.z, expected_p.z, epsilon);
}

TEST(geometry, transform_vector)
{
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::MultiPoint3d;
  using autoware_utils_geometry::transform_vector;
  using autoware_utils_math::deg2rad;

  {
    const MultiPoint3d ps{{1.0, 2.0, 3.0}, {2.0, 3.0, 4.0}};

    geometry_msgs::msg::Transform transform;
    transform.translation.x = 1.0;
    transform.translation.y = 2.0;
    transform.translation.z = 3.0;
    transform.rotation = create_quaternion_from_rpy(deg2rad(30), deg2rad(30), deg2rad(30));

    const MultiPoint3d ps_transformed = transform_vector(ps, transform);

    EXPECT_DOUBLE_EQ(ps_transformed.at(0).x(), 3.1919872981077804);
    EXPECT_DOUBLE_EQ(ps_transformed.at(0).y(), 3.5334936490538906);
    EXPECT_DOUBLE_EQ(ps_transformed.at(0).z(), 5.6160254037844393);

    EXPECT_DOUBLE_EQ(ps_transformed.at(1).x(), 4.350480947161671);
    EXPECT_DOUBLE_EQ(ps_transformed.at(1).y(), 4.625);
    EXPECT_DOUBLE_EQ(ps_transformed.at(1).z(), 6.299038105676658);
  }
}

TEST(geometry, calc_curvature)
{
  using autoware_utils_geometry::calc_curvature;
  // Straight Line
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(1.0, 0.0, 0.0);
    geometry_msgs::msg::Point p3 = autoware_utils_geometry::create_point(2.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(calc_curvature(p1, p2, p3), 0.0);
  }

  // Clockwise Curved Road with a 1[m] radius
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(1.0, 1.0, 0.0);
    geometry_msgs::msg::Point p3 = autoware_utils_geometry::create_point(2.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(calc_curvature(p1, p2, p3), -1.0);
  }

  // Clockwise Curved Road with a 5[m] radius
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(5.0, 5.0, 0.0);
    geometry_msgs::msg::Point p3 = autoware_utils_geometry::create_point(10.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(calc_curvature(p1, p2, p3), -0.2);
  }

  // Counter-Clockwise Curved Road with a 1[m] radius
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(-1.0, 1.0, 0.0);
    geometry_msgs::msg::Point p3 = autoware_utils_geometry::create_point(-2.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(calc_curvature(p1, p2, p3), 1.0);
  }

  // Counter-Clockwise Curved Road with a 5[m] radius
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(-5.0, 5.0, 0.0);
    geometry_msgs::msg::Point p3 = autoware_utils_geometry::create_point(-10.0, 0.0, 0.0);
    EXPECT_DOUBLE_EQ(calc_curvature(p1, p2, p3), 0.2);
  }

  // Give same points
  {
    geometry_msgs::msg::Point p1 = autoware_utils_geometry::create_point(0.0, 0.0, 0.0);
    geometry_msgs::msg::Point p2 = autoware_utils_geometry::create_point(1.0, 0.0, 0.0);
    ASSERT_ANY_THROW(calc_curvature(p1, p1, p1));
    ASSERT_ANY_THROW(calc_curvature(p1, p1, p2));
    ASSERT_ANY_THROW(calc_curvature(p1, p2, p1));
    ASSERT_ANY_THROW(calc_curvature(p1, p2, p2));
  }
}

TEST(geometry, calc_offset_pose)
{
  using autoware_utils_geometry::calc_offset_pose;
  using autoware_utils_geometry::create_point;
  using autoware_utils_geometry::create_quaternion;
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_math::deg2rad;

  // Only translation
  {
    geometry_msgs::msg::Pose p_in;
    p_in.position = create_point(1.0, 2.0, 3.0);
    p_in.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    const auto p_out = calc_offset_pose(p_in, 1.0, 1.0, 1.0);

    EXPECT_DOUBLE_EQ(p_out.position.x, 2.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 3.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 4.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, 1.0);
  }

  {
    geometry_msgs::msg::Pose p_in;
    p_in.position = create_point(2.0, 3.0, 1.0);
    p_in.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(90));

    const auto p_out = calc_offset_pose(p_in, 2.0, 1.0, 3.0);

    EXPECT_DOUBLE_EQ(p_out.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 5.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 4.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.70710678118654757);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, 0.70710678118654757);
  }

  {
    geometry_msgs::msg::Pose p_in;
    p_in.position = create_point(2.0, 1.0, 1.0);
    p_in.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(30));

    const auto p_out = calc_offset_pose(p_in, 2.0, 0.0, -1.0);

    EXPECT_DOUBLE_EQ(p_out.position.x, 3.73205080756887729);
    EXPECT_DOUBLE_EQ(p_out.position.y, 2.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.25881904510252068);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, 0.96592582628906831);
  }

  // Only rotation
  {
    geometry_msgs::msg::Pose p_in;
    p_in.position = create_point(2.0, 1.0, 1.0);
    p_in.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(30));

    const auto p_out = calc_offset_pose(p_in, 0.0, 0.0, 0.0, deg2rad(20));

    EXPECT_DOUBLE_EQ(p_out.position.x, 2.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 1.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
    EXPECT_NEAR(p_out.orientation.z, 0.42261826174069944, epsilon);
    EXPECT_NEAR(p_out.orientation.w, 0.9063077870366499, epsilon);
  }

  // Both translation and rotation
  {
    geometry_msgs::msg::Pose p_in;
    p_in.position = create_point(2.0, 1.0, 1.0);
    p_in.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(30));

    const auto p_out = calc_offset_pose(p_in, 2.0, 0.0, -1.0, deg2rad(20));

    EXPECT_DOUBLE_EQ(p_out.position.x, 3.73205080756887729);
    EXPECT_DOUBLE_EQ(p_out.position.y, 2.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
    EXPECT_NEAR(p_out.orientation.z, 0.42261826174069944, epsilon);
    EXPECT_NEAR(p_out.orientation.w, 0.9063077870366499, epsilon);
  }
}

TEST(geometry, is_driving_forward)
{
  using autoware_utils_geometry::calc_interpolated_point;
  using autoware_utils_geometry::create_point;
  using autoware_utils_geometry::create_quaternion;
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_geometry::is_driving_forward;
  using autoware_utils_math::deg2rad;

  const double epsilon = 1e-3;

  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    EXPECT_TRUE(is_driving_forward(src_pose, dst_pose));
  }

  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(180));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(180));

    EXPECT_FALSE(is_driving_forward(src_pose, dst_pose));
  }

  // Boundary Condition
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(90));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(90));

    EXPECT_TRUE(is_driving_forward(src_pose, dst_pose));
  }

  // Boundary Condition
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(90 + epsilon));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(0.0, 0.0, deg2rad(90 + epsilon));

    EXPECT_FALSE(is_driving_forward(src_pose, dst_pose));
  }
}

TEST(geometry, calc_interpolated_point)
{
  using autoware_utils_geometry::calc_interpolated_point;
  using autoware_utils_geometry::create_point;

  {
    const auto src_point = create_point(0.0, 0.0, 0.0);
    const auto dst_point = create_point(3.0, 0.0, 0.0);

    const double epsilon = 1e-3;
    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_point(src_point, dst_point, ratio);

      EXPECT_DOUBLE_EQ(p_out.x, 3.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.z, 0.0);
    }
  }

  // Same points are given
  {
    const auto src_point = create_point(0.0, 0.0, 0.0);
    const auto dst_point = create_point(0.0, 0.0, 0.0);

    const double epsilon = 1e-3;
    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_point(src_point, dst_point, ratio);

      EXPECT_DOUBLE_EQ(p_out.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.z, 0.0);
    }
  }

  // Boundary Condition (Negative Ratio)
  {
    const auto src_point = create_point(0.0, 0.0, 0.0);
    const auto dst_point = create_point(3.0, 0.0, 0.0);

    const auto p_out = calc_interpolated_point(src_point, dst_point, -10.0);
    EXPECT_DOUBLE_EQ(p_out.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.z, 0.0);
  }

  // Boundary Condition (Positive Ratio larger than 1.0)
  {
    const auto src_point = create_point(0.0, 0.0, 0.0);
    const auto dst_point = create_point(3.0, 0.0, 0.0);

    const auto p_out = calc_interpolated_point(src_point, dst_point, 10.0);
    EXPECT_DOUBLE_EQ(p_out.x, 3.0);
    EXPECT_DOUBLE_EQ(p_out.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.z, 0.0);
  }
}

TEST(geometry, calc_interpolated_pose)
{
  using autoware_utils_geometry::calc_interpolated_pose;
  using autoware_utils_geometry::create_point;
  using autoware_utils_geometry::create_quaternion;
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_math::deg2rad;
  const double epsilon = 1e-3;

  // Position Interpolation
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio);

      EXPECT_DOUBLE_EQ(p_out.position.x, 3.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, 1.0);
    }
  }

  // Boundary Condition (Negative Ratio)
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    const auto p_out = calc_interpolated_pose(src_pose, dst_pose, -10.0);

    const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(45));
    EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
  }

  // Boundary Condition (Positive Ratio larger than 1.0)
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    const auto p_out = calc_interpolated_pose(src_pose, dst_pose, 10.0);

    const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));
    EXPECT_DOUBLE_EQ(p_out.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
  }

  // Quaternion Interpolation
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    for (double ratio = 0.0; ratio < 1.0 - epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio);

      const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(45));
      EXPECT_DOUBLE_EQ(p_out.position.x, 1.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.y, 1.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
    }

    // Boundary Condition (ratio = 1.0)
    const auto p_out = calc_interpolated_pose(src_pose, dst_pose, 1.0);

    const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));
    EXPECT_DOUBLE_EQ(p_out.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
  }

  // Same poses are given
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(0.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio);

      EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, 1.0);
    }
  }

  // Same points are given (different orientation)
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(0.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(45));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio);

      const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(45));
      EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
    }
  }

  // Driving Backward
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(180));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(5.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(180));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio);

      const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(180));
      EXPECT_DOUBLE_EQ(p_out.position.x, 5.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
    }
  }
}

TEST(geometry, calc_interpolated_pose_with_Spherical_Interpolation)
{
  using autoware_utils_geometry::calc_interpolated_pose;
  using autoware_utils_geometry::create_point;
  using autoware_utils_geometry::create_quaternion;
  using autoware_utils_geometry::create_quaternion_from_rpy;
  using autoware_utils_math::deg2rad;
  const double epsilon = 1e-3;

  // Position Interpolation
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(3.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion(0.0, 0.0, 0.0, 1.0);

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio, false);

      EXPECT_DOUBLE_EQ(p_out.position.x, 3.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, 1.0);
    }
  }

  // Boundary Condition (Negative Ratio)
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    const auto p_out = calc_interpolated_pose(src_pose, dst_pose, -10.0, false);

    const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));
    EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
  }

  // Boundary Condition (Positive Ratio larger than 1.0)
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    const auto p_out = calc_interpolated_pose(src_pose, dst_pose, 10.0, false);

    const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));
    EXPECT_DOUBLE_EQ(p_out.position.x, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.y, 1.0);
    EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
    EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
    EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
    EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
    EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
  }

  // Quaternion Interpolation1
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(0.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(90));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio, false);

      const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(90 * ratio));
      EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
    }
  }

  // Quaternion Interpolation2
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(1.0, 1.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio, false);

      const auto ans_quat = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(60 * ratio));
      EXPECT_DOUBLE_EQ(p_out.position.x, 1.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.y, 1.0 * ratio);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, ans_quat.x);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, ans_quat.y);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, ans_quat.z);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, ans_quat.w);
    }
  }

  // Same points are given
  {
    geometry_msgs::msg::Pose src_pose;
    src_pose.position = create_point(0.0, 0.0, 0.0);
    src_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    geometry_msgs::msg::Pose dst_pose;
    dst_pose.position = create_point(0.0, 0.0, 0.0);
    dst_pose.orientation = create_quaternion_from_rpy(deg2rad(0), deg2rad(0), deg2rad(0));

    for (double ratio = 0.0; ratio < 1.0 + epsilon; ratio += 0.1) {
      const auto p_out = calc_interpolated_pose(src_pose, dst_pose, ratio, false);

      EXPECT_DOUBLE_EQ(p_out.position.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.position.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.x, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.y, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.z, 0.0);
      EXPECT_DOUBLE_EQ(p_out.orientation.w, 1.0);
    }
  }
}

TEST(geometry, getTwist)
{
  using autoware_utils_geometry::create_vector3;

  geometry_msgs::msg::Vector3 velocity = create_vector3(1.0, 2.0, 3.0);
  geometry_msgs::msg::Vector3 angular = create_vector3(0.1, 0.2, 0.3);

  geometry_msgs::msg::Twist twist = geometry_msgs::build<geometry_msgs::msg::Twist>()
                                      .linear(create_vector3(1.0, 2.0, 3.0))
                                      .angular(create_vector3(0.1, 0.2, 0.3));

  geometry_msgs::msg::TwistWithCovariance twist_with_covariance;
  twist_with_covariance.twist = geometry_msgs::build<geometry_msgs::msg::Twist>()
                                  .linear(create_vector3(1.0, 2.0, 3.0))
                                  .angular(create_vector3(0.1, 0.2, 0.3));

  // getTwist
  {
    auto t_out = autoware_utils_geometry::create_twist(velocity, angular);
    EXPECT_DOUBLE_EQ(t_out.linear.x, twist.linear.x);
    EXPECT_DOUBLE_EQ(t_out.linear.y, twist.linear.y);
    EXPECT_DOUBLE_EQ(t_out.linear.z, twist.linear.z);
    EXPECT_DOUBLE_EQ(t_out.angular.x, twist.angular.x);
    EXPECT_DOUBLE_EQ(t_out.angular.y, twist.angular.y);
    EXPECT_DOUBLE_EQ(t_out.angular.z, twist.angular.z);
  }
}

TEST(geometry, getTwistNorm)
{
  using autoware_utils_geometry::create_vector3;
  geometry_msgs::msg::TwistWithCovariance twist_with_covariance;
  twist_with_covariance.twist = geometry_msgs::build<geometry_msgs::msg::Twist>()
                                  .linear(create_vector3(3.0, 4.0, 0.0))
                                  .angular(create_vector3(0.1, 0.2, 0.3));
  EXPECT_NEAR(autoware_utils_geometry::calc_norm(twist_with_covariance.twist.linear), 5.0, epsilon);
}

TEST(geometry, is_twist_covariance_valid)
{
  using autoware_utils_geometry::create_vector3;
  geometry_msgs::msg::TwistWithCovariance twist_with_covariance;
  twist_with_covariance.twist = geometry_msgs::build<geometry_msgs::msg::Twist>()
                                  .linear(create_vector3(1.0, 2.0, 3.0))
                                  .angular(create_vector3(0.1, 0.2, 0.3));

  EXPECT_EQ(autoware_utils_geometry::is_twist_covariance_valid(twist_with_covariance), false);

  twist_with_covariance.covariance.at(0) = 1.0;
  EXPECT_EQ(autoware_utils_geometry::is_twist_covariance_valid(twist_with_covariance), true);
}

TEST(geometry, intersect)
{
  using autoware_utils_geometry::create_point;
  using autoware_utils_geometry::intersect;

  {  // Normally crossing
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(-1.0, 0.0, 0.0);
    const auto p4 = create_point(1.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_TRUE(result);
    EXPECT_NEAR(result->x, 0.0, epsilon);
    EXPECT_NEAR(result->y, 0.0, epsilon);
    EXPECT_NEAR(result->z, 0.0, epsilon);
  }

  {  // No crossing
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(1.0, 0.0, 0.0);
    const auto p4 = create_point(3.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // One segment is the point on the other's segment
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(0.0, 0.0, 0.0);
    const auto p4 = create_point(0.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // One segment is the point not on the other's segment
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(1.0, 0.0, 0.0);
    const auto p4 = create_point(1.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // Both segments are the points which are the same position
    const auto p1 = create_point(0.0, 0.0, 0.0);
    const auto p2 = create_point(0.0, 0.0, 0.0);
    const auto p3 = create_point(0.0, 0.0, 0.0);
    const auto p4 = create_point(0.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // Both segments are the points which are different position
    const auto p1 = create_point(0.0, 1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(1.0, 0.0, 0.0);
    const auto p4 = create_point(1.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // Segments are the same
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(0.0, -1.0, 0.0);
    const auto p4 = create_point(0.0, 1.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_FALSE(result);
  }

  {  // One's edge is on the other's segment
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(0.0, 0.0, 0.0);
    const auto p4 = create_point(1.0, 0.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_TRUE(result);
    EXPECT_NEAR(result->x, 0.0, epsilon);
    EXPECT_NEAR(result->y, 0.0, epsilon);
    EXPECT_NEAR(result->z, 0.0, epsilon);
  }

  {  // One's edge is the same as the other's edge.
    const auto p1 = create_point(0.0, -1.0, 0.0);
    const auto p2 = create_point(0.0, 1.0, 0.0);
    const auto p3 = create_point(0.0, -1.0, 0.0);
    const auto p4 = create_point(2.0, -1.0, 0.0);
    const auto result = intersect(p1, p2, p3, p4);

    EXPECT_TRUE(result);
    EXPECT_NEAR(result->x, 0.0, epsilon);
    EXPECT_NEAR(result->y, -1.0, epsilon);
    EXPECT_NEAR(result->z, 0.0, epsilon);
  }
}

TEST(
  geometry,
  DISABLED_intersectPolygon)  // GJK give different result for edge test (point sharing and edge
                              // sharing) compared to SAT and boost::geometry::intersect
{
  {  // 2 triangles with intersection
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(2, 0);
    poly2.outer().emplace_back(1, 1);
    poly2.outer().emplace_back(1, 0);
    poly2.outer().emplace_back(0, 1);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_TRUE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_TRUE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
  {  // 2 triangles with no intersection (but they share an edge)
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(0, 0);
    poly2.outer().emplace_back(0, 0);
    poly2.outer().emplace_back(2, 0);
    poly2.outer().emplace_back(2, 2);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_FALSE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_FALSE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
  {  // 2 triangles with no intersection (but they share a point)
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(0, 0);
    poly2.outer().emplace_back(4, 4);
    poly2.outer().emplace_back(4, 2);
    poly2.outer().emplace_back(2, 2);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_FALSE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_FALSE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
  {  // 2 triangles sharing a point and then with very small intersection
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(0, 0);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(4, 0);
    poly2.outer().emplace_back(0, 4);
    poly2.outer().emplace_back(2, 2);
    poly2.outer().emplace_back(4, 4);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_FALSE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    poly1.outer()[1].y() += 1e-12;
    EXPECT_TRUE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_TRUE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
  {  // 2 triangles with no intersection and no touching
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(0, 0);
    poly2.outer().emplace_back(4, 4);
    poly2.outer().emplace_back(5, 5);
    poly2.outer().emplace_back(3, 5);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_FALSE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_FALSE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
  {  // triangle and quadrilateral with intersection
    autoware_utils_geometry::Polygon2d poly1;
    autoware_utils_geometry::Polygon2d poly2;
    poly1.outer().emplace_back(4, 11);
    poly1.outer().emplace_back(4, 5);
    poly1.outer().emplace_back(9, 9);
    poly2.outer().emplace_back(5, 7);
    poly2.outer().emplace_back(7, 3);
    poly2.outer().emplace_back(10, 2);
    poly2.outer().emplace_back(12, 7);
    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);
    EXPECT_TRUE(autoware_utils_geometry::intersects_convex(poly1, poly2));
    EXPECT_TRUE(autoware_utils_geometry::sat::intersects(poly1, poly2));
  }
}

TEST(geometry, intersectPolygonRand)
{
  std::vector<autoware_utils_geometry::Polygon2d> polygons;
  constexpr auto polygons_nb = 100;
  constexpr auto max_vertices = 10;
  constexpr auto max_values = 1000;

  autoware_utils_system::StopWatch<std::chrono::nanoseconds, std::chrono::nanoseconds> sw;

  for (auto vertices = 3UL; vertices < max_vertices; ++vertices) {
    double ground_truth_intersect_ns = 0.0;
    double ground_truth_no_intersect_ns = 0.0;
    double gjk_intersect_ns = 0.0;
    double gjk_no_intersect_ns = 0.0;
    double sat_intersect_ns = 0.0;
    double sat_no_intersect_ns = 0.0;
    int intersect_count = 0;
    polygons.clear();

    for (auto i = 0; i < polygons_nb; ++i) {
      polygons.push_back(autoware_utils_geometry::random_convex_polygon(vertices, max_values));
    }

    for (auto i = 0UL; i < polygons.size(); ++i) {
      for (auto j = 0UL; j < polygons.size(); ++j) {
        sw.tic();
        const auto ground_truth = boost::geometry::intersects(polygons[i], polygons[j]);
        if (ground_truth) {
          ++intersect_count;
          ground_truth_intersect_ns += sw.toc();
        } else {
          ground_truth_no_intersect_ns += sw.toc();
        }

        sw.tic();
        const auto gjk = autoware_utils_geometry::intersects_convex(polygons[i], polygons[j]);
        if (gjk) {
          gjk_intersect_ns += sw.toc();
        } else {
          gjk_no_intersect_ns += sw.toc();
        }

        sw.tic();
        const auto sat = autoware_utils_geometry::sat::intersects(polygons[i], polygons[j]);
        if (sat) {
          sat_intersect_ns += sw.toc();
        } else {
          sat_no_intersect_ns += sw.toc();
        }

        EXPECT_EQ(ground_truth, gjk);
        EXPECT_EQ(ground_truth, sat);

        if (ground_truth != gjk) {
          std::cout << "Failed for the 2 polygons with GJK: ";
          std::cout << boost::geometry::wkt(polygons[i]) << boost::geometry::wkt(polygons[j])
                    << std::endl;
        }

        if (ground_truth != sat) {
          std::cout << "Failed for the 2 polygons with SAT: ";
          std::cout << boost::geometry::wkt(polygons[i]) << boost::geometry::wkt(polygons[j])
                    << std::endl;
        }
      }
    }

    std::printf(
      "polygons_nb = %d, vertices = %ld, %d / %d pairs with intersects\n", polygons_nb, vertices,
      intersect_count, polygons_nb * polygons_nb);

    std::printf(
      "\tIntersect:\n\t\tBoost::geometry = %2.2f ms\n\t\tGJK = %2.2f ms\n\t\tSAT = %2.2f ms\n",
      ground_truth_intersect_ns / 1e6, gjk_intersect_ns / 1e6, sat_intersect_ns / 1e6);

    std::printf(
      "\tNo Intersect:\n\t\tBoost::geometry = %2.2f ms\n\t\tGJK = %2.2f ms\n\t\tSAT = %2.2f ms\n",
      ground_truth_no_intersect_ns / 1e6, gjk_no_intersect_ns / 1e6, sat_no_intersect_ns / 1e6);

    std::printf(
      "\tTotal:\n\t\tBoost::geometry = %2.2f ms\n\t\tGJK = %2.2f ms\n\t\tSAT = %2.2f ms\n",
      (ground_truth_no_intersect_ns + ground_truth_intersect_ns) / 1e6,
      (gjk_no_intersect_ns + gjk_intersect_ns) / 1e6,
      (sat_no_intersect_ns + sat_intersect_ns) / 1e6);
  }
}

double calculate_total_polygon_area(
  const std::vector<autoware_utils_geometry::Polygon2d> & polygons)
{
  double totalArea = 0.0;
  for (const auto & polygon : polygons) {
    totalArea += boost::geometry::area(polygon);
  }
  return totalArea;
}

TEST(geometry, PolygonTriangulation)
{
  using autoware_utils_geometry::Polygon2d;
  using autoware_utils_geometry::triangulate;

  {  // concave polygon
    Polygon2d poly;

    poly.outer().emplace_back(0.0, 0.0);
    poly.outer().emplace_back(4.0, 0.0);
    poly.outer().emplace_back(4.0, 4.0);
    poly.outer().emplace_back(2.0, 2.0);
    poly.outer().emplace_back(0.0, 4.0);
    boost::geometry::correct(poly);

    const auto triangles = triangulate(poly);

    const auto triangle_area = calculate_total_polygon_area(triangles);
    const auto poly_area = boost::geometry::area(poly);
    EXPECT_NEAR(triangle_area, poly_area, epsilon);
  }

  {  // concave polygon with empty inners
    Polygon2d poly;

    poly.outer().emplace_back(0.0, 0.0);
    poly.outer().emplace_back(4.0, 0.0);
    poly.outer().emplace_back(4.0, 4.0);
    poly.outer().emplace_back(2.0, 2.0);
    poly.outer().emplace_back(0.0, 4.0);
    boost::geometry::correct(poly);

    poly.inners().emplace_back();

    const auto triangles = triangulate(poly);

    const auto triangle_area = calculate_total_polygon_area(triangles);
    const auto poly_area = boost::geometry::area(poly);
    EXPECT_NEAR(triangle_area, poly_area, epsilon);
  }

  {  // concave polygon with hole
    Polygon2d poly;

    poly.outer().emplace_back(0.0, 0.0);
    poly.outer().emplace_back(4.0, 0.0);
    poly.outer().emplace_back(4.0, 4.0);
    poly.outer().emplace_back(2.0, 2.0);
    poly.outer().emplace_back(0.0, 4.0);

    poly.inners().emplace_back();
    poly.inners().back().emplace_back(1.0, 1.0);
    poly.inners().back().emplace_back(1.5, 1.0);
    poly.inners().back().emplace_back(1.5, 1.5);
    poly.inners().back().emplace_back(1.0, 1.5);
    boost::geometry::correct(poly);

    const auto triangles = triangulate(poly);

    const auto triangle_area = calculate_total_polygon_area(triangles);
    const auto poly_area = boost::geometry::area(poly);
    EXPECT_NEAR(triangle_area, poly_area, epsilon);
  }

  {  // concave polygon with one empty inner followed by one hole
    Polygon2d poly;

    poly.outer().emplace_back(0.0, 0.0);
    poly.outer().emplace_back(4.0, 0.0);
    poly.outer().emplace_back(4.0, 4.0);
    poly.outer().emplace_back(2.0, 2.0);
    poly.outer().emplace_back(0.0, 4.0);

    poly.inners().emplace_back();
    poly.inners().emplace_back();
    poly.inners().back().emplace_back(1.0, 1.0);
    poly.inners().back().emplace_back(1.5, 1.0);
    poly.inners().back().emplace_back(1.5, 1.5);
    poly.inners().back().emplace_back(1.0, 1.5);
    boost::geometry::correct(poly);

    const auto triangles = triangulate(poly);

    const auto triangle_area = calculate_total_polygon_area(triangles);
    const auto poly_area = boost::geometry::area(poly);
    EXPECT_NEAR(triangle_area, poly_area, epsilon);
  }
}

TEST(geometry, intersectPolygonWithHoles)
{
  using autoware_utils_geometry::Polygon2d;
  using autoware_utils_geometry::triangulate;

  {  // quadrilateral inside the hole of another quadrilateral (not intersecting)
    Polygon2d outerConcave;
    Polygon2d innerConcave;

    outerConcave.outer().emplace_back(0.0, 0.0);
    outerConcave.outer().emplace_back(4.0, 0.0);
    outerConcave.outer().emplace_back(4.0, 4.0);
    outerConcave.outer().emplace_back(2.0, 2.0);
    outerConcave.outer().emplace_back(0.0, 4.0);

    outerConcave.inners().emplace_back();
    outerConcave.inners().back().emplace_back(1.0, 1.0);
    outerConcave.inners().back().emplace_back(3.0, 1.0);
    outerConcave.inners().back().emplace_back(3.0, 3.0);
    outerConcave.inners().back().emplace_back(1.0, 3.0);

    innerConcave.outer().emplace_back(1.5, 1.5);
    innerConcave.outer().emplace_back(2.5, 1.5);
    innerConcave.outer().emplace_back(2.5, 2.5);
    innerConcave.outer().emplace_back(1.5, 2.5);

    const auto triangles1 = triangulate(outerConcave);
    const auto triangles2 = triangulate(innerConcave);

    const bool gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const bool sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_FALSE(gjk_intersect);
    EXPECT_FALSE(sat_intersect);
  }

  {  // quadrilateral inside the hole of another quadrilateral (intersecting)
    Polygon2d outerConcave;
    Polygon2d intersectingInnerConcave;

    outerConcave.outer().emplace_back(0.0, 0.0);
    outerConcave.outer().emplace_back(4.0, 0.0);
    outerConcave.outer().emplace_back(4.0, 4.0);
    outerConcave.outer().emplace_back(2.0, 2.0);
    outerConcave.outer().emplace_back(0.0, 4.0);

    outerConcave.inners().emplace_back();
    outerConcave.inners().back().emplace_back(1.0, 1.0);
    outerConcave.inners().back().emplace_back(3.0, 1.0);
    outerConcave.inners().back().emplace_back(3.0, 3.0);
    outerConcave.inners().back().emplace_back(1.0, 3.0);

    intersectingInnerConcave.outer().emplace_back(0.5, 0.5);
    intersectingInnerConcave.outer().emplace_back(2.5, 0.5);
    intersectingInnerConcave.outer().emplace_back(2.5, 2.0);
    intersectingInnerConcave.outer().emplace_back(0.5, 2.0);

    const auto triangles1 = triangulate(outerConcave);
    const auto triangles2 = triangulate(intersectingInnerConcave);
    const auto gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const auto sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_TRUE(gjk_intersect);
    EXPECT_TRUE(sat_intersect);
  }
}

TEST(
  geometry,
  DISABLED_intersectConcavePolygon)  // GJK give different result for edge test (point sharing and
                                     // edge sharing) compared to SAT and boost::geometry::intersect
{
  using autoware_utils_geometry::Polygon2d;
  using autoware_utils_geometry::triangulate;

  {  // 2 Concave quadrilateral with intersection
    Polygon2d poly1;
    Polygon2d poly2;
    poly1.outer().emplace_back(4, 11);
    poly1.outer().emplace_back(4, 5);
    poly1.outer().emplace_back(9, 9);
    poly1.outer().emplace_back(2, 2);
    poly2.outer().emplace_back(5, 7);
    poly2.outer().emplace_back(7, 3);
    poly2.outer().emplace_back(9, 6);
    poly2.outer().emplace_back(12, 7);

    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);

    const auto triangles1 = triangulate(poly1);
    const auto triangles2 = triangulate(poly2);

    const auto gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const auto sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_TRUE(gjk_intersect);
    EXPECT_TRUE(sat_intersect);
  }

  {  // 2 concave polygons with no intersection (but they share an edge)
    Polygon2d poly1;
    Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(2, 0);
    poly1.outer().emplace_back(0, 0);

    poly2.outer().emplace_back(0, 0);
    poly2.outer().emplace_back(2, 0);
    poly2.outer().emplace_back(2, -2);
    poly2.outer().emplace_back(0, -2);

    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);

    const auto triangles1 = triangulate(poly1);
    const auto triangles2 = triangulate(poly2);

    const auto gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const auto sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_FALSE(gjk_intersect);
    EXPECT_FALSE(sat_intersect);
  }

  {  // 2 concave polygons with no intersection (but they share a point)
    Polygon2d poly1;
    Polygon2d poly2;
    poly1.outer().emplace_back(0, 2);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(0, 0);

    poly2.outer().emplace_back(4, 4);
    poly2.outer().emplace_back(4, 2);
    poly2.outer().emplace_back(2, 2);
    poly2.outer().emplace_back(2, 4);

    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);

    auto triangles1 = triangulate(poly1);
    auto triangles2 = triangulate(poly2);

    const auto gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const auto sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_FALSE(gjk_intersect);
    EXPECT_FALSE(sat_intersect);
  }

  {  // 2 concave polygons sharing a point and then with very small intersection
    Polygon2d poly1;
    Polygon2d poly2;
    poly1.outer().emplace_back(0, 0);
    poly1.outer().emplace_back(2, 2);
    poly1.outer().emplace_back(4, 0);
    poly1.outer().emplace_back(2, -2);

    poly2.outer().emplace_back(0, 4);
    poly2.outer().emplace_back(2, 2);
    poly2.outer().emplace_back(4, 4);
    poly2.outer().emplace_back(2, 6);

    boost::geometry::correct(poly1);
    boost::geometry::correct(poly2);

    const auto triangles1 = triangulate(poly1);
    const auto triangles2 = triangulate(poly2);

    const auto gjk_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::intersects_convex);
    const auto sat_intersect = autoware_utils_geometry::test_intersection(
      triangles1, triangles2, autoware_utils_geometry::sat::intersects);

    EXPECT_FALSE(gjk_intersect);
    EXPECT_FALSE(sat_intersect);

    // small intersection test
    poly1.outer()[1].y() += 1e-12;
    {
      const auto triangles = triangulate(poly1);
      const bool gjk_intersect = autoware_utils_geometry::test_intersection(
        triangles, triangles2, autoware_utils_geometry::intersects_convex);
      const bool sat_intersect = autoware_utils_geometry::test_intersection(
        triangles, triangles2, autoware_utils_geometry::sat::intersects);
      EXPECT_TRUE(gjk_intersect);
      EXPECT_TRUE(sat_intersect);
    }
  }
}

TEST(geometry, intersectConcavePolygonRand)
{
  std::vector<autoware_utils_geometry::Polygon2d> polygons;
  std::vector<std::vector<autoware_utils_geometry::Polygon2d>> triangulations;
  constexpr auto polygons_nb = 100;
  constexpr auto max_vertices = 10;
  constexpr auto max_values = 1000;

  autoware_utils_system::StopWatch<std::chrono::nanoseconds, std::chrono::nanoseconds> sw;

  for (auto vertices = 4UL; vertices < max_vertices; ++vertices) {
    double ground_truth_intersect_ns = 0.0;
    double ground_truth_no_intersect_ns = 0.0;
    double gjk_intersect_ns = 0.0;
    double gjk_no_intersect_ns = 0.0;
    double sat_intersect_ns = 0.0;
    double sat_no_intersect_ns = 0.0;
    double triangulation_ns = 0.0;
    int intersect_count = 0;
    polygons.clear();
    triangulations.clear();

    for (auto i = 0; i < polygons_nb; ++i) {
      auto polygon_opt = autoware_utils_geometry::random_concave_polygon(vertices, max_values);
      if (polygon_opt.has_value()) {
        polygons.push_back(polygon_opt.value());
      }
    }

    for (const auto & polygon : polygons) {
      sw.tic();
      std::vector<autoware_utils_geometry::Polygon2d> triangles =
        autoware_utils_geometry::triangulate(polygon);
      triangulation_ns += sw.toc();
      triangulations.push_back(triangles);
    }

    for (auto i = 0UL; i < polygons.size(); ++i) {
      for (auto j = 0UL; j < polygons.size(); ++j) {
        sw.tic();
        const auto ground_truth = boost::geometry::intersects(polygons[i], polygons[j]);
        if (ground_truth) {
          ++intersect_count;
          ground_truth_intersect_ns += sw.toc();
        } else {
          ground_truth_no_intersect_ns += sw.toc();
        }

        sw.tic();
        bool gjk_intersect = autoware_utils_geometry::test_intersection(
          triangulations[i], triangulations[j], autoware_utils_geometry::intersects_convex);
        if (!gjk_intersect) {
          gjk_no_intersect_ns += sw.toc();
        } else {
          gjk_intersect_ns += sw.toc();
        }

        sw.tic();
        bool sat_intersect = autoware_utils_geometry::test_intersection(
          triangulations[i], triangulations[j], autoware_utils_geometry::sat::intersects);
        if (!sat_intersect) {
          sat_no_intersect_ns += sw.toc();
        } else {
          sat_intersect_ns += sw.toc();
        }

        EXPECT_EQ(ground_truth, gjk_intersect);
        EXPECT_EQ(ground_truth, sat_intersect);

        if (ground_truth != gjk_intersect) {
          std::cout << "Failed for the 2 polygons with GJK: ";
          std::cout << boost::geometry::wkt(polygons[i]) << boost::geometry::wkt(polygons[j])
                    << std::endl;
        }

        if (ground_truth != sat_intersect) {
          std::cout << "Failed for the 2 polygons with SAT: ";
          std::cout << boost::geometry::wkt(polygons[i]) << boost::geometry::wkt(polygons[j])
                    << std::endl;
        }
      }
    }

    std::printf(
      "polygons_nb = %d, vertices = %ld, %d / %d pairs with intersects\n", polygons_nb, vertices,
      intersect_count, polygons_nb * polygons_nb);

    std::printf(
      "\tIntersect:\n\t\tBoost::geometry = %2.2f ms\n\t\tGJK = %2.2f ms\n\t\tSAT = %2.2f ms\n",
      ground_truth_intersect_ns / 1e6, gjk_intersect_ns / 1e6, sat_intersect_ns / 1e6);

    std::printf(
      "\tNo Intersect:\n\t\tBoost::geometry = %2.2f ms\n\t\tGJK = %2.2f ms\n\t\tSAT = %2.2f ms\n",
      ground_truth_no_intersect_ns / 1e6, gjk_no_intersect_ns / 1e6, sat_no_intersect_ns / 1e6);

    std::printf("\tTotal:\n\t\tTriangulation = %2.2f ms\n", triangulation_ns / 1e6);
  }
}
