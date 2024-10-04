/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <gtest/gtest.h>
#include "workflow/parametercontainer.h"

namespace workflow::test {
TEST(ParameterContainer, TestCaseSensitiveDevice) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(false);
  EXPECT_EQ(rtdb.Devices().size(), 0);
  {
    // Should not add no named items
    const auto *device = rtdb.CreateDevice("");
    EXPECT_TRUE(device == nullptr);
    EXPECT_EQ(rtdb.Devices().size(), 0);
  }

  {
    // Normal device
    auto *device = rtdb.CreateDevice("Device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "Device1");
    EXPECT_EQ(rtdb.Devices().size(), 1);
  }
  {
    // Similar name
    auto *device = rtdb.CreateDevice("device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "device1");
    EXPECT_EQ(rtdb.Devices().size(), 2);
  }
  {
    // Same name
    auto *device = rtdb.CreateDevice("Device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "Device1");
    EXPECT_EQ(rtdb.Devices().size(), 2);
  }

  rtdb.DeleteDevice("device1");
  EXPECT_EQ(rtdb.Devices().size(), 1);
  rtdb.DeleteDevice("device1"); // Should not delete anything
  EXPECT_EQ(rtdb.Devices().size(), 1);

  rtdb.DeleteDevice("Device1");
  EXPECT_EQ(rtdb.Devices().size(), 0);
}

TEST(ParameterContainer, TestNoCaseSensitiveDevice) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(true);
  EXPECT_EQ(rtdb.Devices().size(), 0);
  {
    // Should not add no named items
    const auto *device = rtdb.CreateDevice("");
    EXPECT_TRUE(device == nullptr);
    EXPECT_EQ(rtdb.Devices().size(), 0);
  }

  {
    // Normal device
    auto *device = rtdb.CreateDevice("Device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "Device1");
    EXPECT_EQ(rtdb.Devices().size(), 1);
  }
  {
    // Similar name
    auto *device = rtdb.CreateDevice("device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "Device1");
    EXPECT_EQ(rtdb.Devices().size(), 1);
  }
  {
    // Same name
    auto *device = rtdb.CreateDevice("Device1");
    ASSERT_TRUE(device != nullptr);
    EXPECT_STREQ(device->Name().c_str(), "Device1");
    EXPECT_EQ(rtdb.Devices().size(), 1);
  }

  rtdb.DeleteDevice("device1");
  EXPECT_EQ(rtdb.Devices().size(), 0);

}

TEST(ParameterContainer, TestCaseSensitiveParameter) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(false);
  EXPECT_EQ(rtdb.Parameters().size(), 0);
  {
    // Should not add no named items
    const auto* parameter = rtdb.CreateParameter("", "");
    EXPECT_TRUE(parameter == nullptr);
    EXPECT_EQ(rtdb.Parameters().size(), 0);
  }
  {
    // Normal parameter
    auto* parameter = rtdb.CreateParameter("Device1", "Parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "Device1");
    EXPECT_STREQ(parameter->Name().c_str(), "Parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 1);
  }
  {
    // Similar name
    auto* parameter = rtdb.CreateParameter("device1", "parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "device1");
    EXPECT_STREQ(parameter->Name().c_str(), "parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 2);
  }
  {
    // Same name
    auto* parameter = rtdb.CreateParameter("Device1", "Parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "Device1");
    EXPECT_STREQ(parameter->Name().c_str(), "Parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 2);
  }

  rtdb.DeleteParameter("device1", "parameter1");
  EXPECT_EQ(rtdb.Parameters().size(), 1);
  rtdb.DeleteParameter("device1", "parameter1"); // Should not delete anything
  EXPECT_EQ(rtdb.Parameters().size(), 1);

  rtdb.DeleteParameter("Device1", "Parameter1");
  EXPECT_EQ(rtdb.Parameters().size(), 0);
}

TEST(ParameterContainer, TestNoCaseSensitiveParameter) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(true);
  EXPECT_EQ(rtdb.Parameters().size(), 0);
  {
    // Should not add no named items
    const auto* parameter = rtdb.CreateParameter("", "");
    EXPECT_TRUE(parameter == nullptr);
    EXPECT_EQ(rtdb.Parameters().size(), 0);
  }
  {
    // Normal parameter
    auto* parameter = rtdb.CreateParameter("Device1", "Parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "Device1");
    EXPECT_STREQ(parameter->Name().c_str(), "Parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 1);
  }
  {
    // Similar name
    auto* parameter = rtdb.CreateParameter("device1", "parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "Device1");
    EXPECT_STREQ(parameter->Name().c_str(), "Parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 1);
  }
  {
    // Same name
    auto* parameter = rtdb.CreateParameter("Device1", "Parameter1");
    ASSERT_TRUE(parameter != nullptr);
    EXPECT_STREQ(parameter->Device().c_str(), "Device1");
    EXPECT_STREQ(parameter->Name().c_str(), "Parameter1");
    EXPECT_EQ(rtdb.Parameters().size(), 1);
  }

  rtdb.DeleteParameter("device1", "parameter1");
  EXPECT_EQ(rtdb.Parameters().size(), 0);

}

TEST(ParameterContainer, TestSort) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(true);
  rtdb.CreateDevice("Device2");
  rtdb.CreateDevice("Device1");
  EXPECT_EQ(rtdb.Devices().size(), 2);
  {
    size_t index = 2;
    for (const auto& device : rtdb.Devices() ) {
      ASSERT_TRUE(device);
      std::ostringstream device_name;
      device_name << "Device" << index;
      EXPECT_EQ(device_name.str(), device->Name());
      --index;
    }
  }

  rtdb.CreateParameter("Device2", "Parameter4");
  rtdb.CreateParameter("Device2", "Parameter3");
  rtdb.CreateParameter("Device1", "Parameter2");
  rtdb.CreateParameter("Device1", "Parameter1");
  EXPECT_EQ(rtdb.Parameters().size(), 4);
  {
    size_t index = 4;
    for (const auto& parameter : rtdb.Parameters() ) {
      ASSERT_TRUE(parameter);
      std::ostringstream parameter_name;
      parameter_name << "Parameter" << index;
      EXPECT_EQ(parameter_name.str(), parameter->Name());
      --index;
    }
  }

  rtdb.Sort();
  EXPECT_EQ(rtdb.Devices().size(), 2);
  {
    size_t index = 1;
    for (const auto& device : rtdb.Devices() ) {
      ASSERT_TRUE(device);
      std::ostringstream device_name;
      device_name << "Device" << index;
      EXPECT_EQ(device_name.str(), device->Name());
      ++index;
    }
  }
  {
    size_t index = 1;
    for (const auto& parameter : rtdb.Parameters() ) {
      ASSERT_TRUE(parameter);
      std::ostringstream parameter_name;
      parameter_name << "Parameter" << index;
      EXPECT_EQ(parameter_name.str(), parameter->Name());
      ++index;
      std::cout << parameter->Device() <<  "/" << parameter->Name() << std::endl;
    }
  }
}

TEST(ParameterContainer, TestUnits) {
  ParameterContainer rtdb;
  rtdb.IgnoreCase(true);
  auto* parameter1 = rtdb.CreateParameter("Device1", "Parameter1");
  parameter1->Unit("s"); // Seconds

  auto* parameter2 = rtdb.CreateParameter("Device1", "Parameter2");
  parameter2->Unit("S"); // Siemens

  auto* parameter3 = rtdb.CreateParameter("Device1", "Parameter3");
  parameter3->Unit("ms"); // Milliseconds

  const auto unit_list = rtdb.Units();
  EXPECT_EQ(unit_list.size(), 3);
  for (const auto& unit : unit_list) {
    std::cout << unit << std::endl;
  }
}


} // end namespace