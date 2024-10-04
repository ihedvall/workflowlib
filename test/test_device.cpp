/*
* Copyright 2024 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include <gtest/gtest.h>
#include "workflow/device.h"
#include <util/ixmlfile.h>

using namespace util::xml;

namespace workflow::test {
TEST(Device, TestProperties) {
  Device device;

  device.Name("Device1");
  EXPECT_STREQ(device.Name().c_str(), "Device1");

  device.Description("Description1");
  EXPECT_STREQ(device.Description().c_str(), "Description1");

  device.Identity(11);
  EXPECT_EQ(device.Identity(), 11);

  device.Protocol("Protocol1");
  EXPECT_STREQ(device.Protocol().c_str(), "Protocol1");

  device.Bus("Bus1");
  EXPECT_STREQ(device.Bus().c_str(), "Bus1");
}

TEST(Device, TestStorage) {
  Device orig;
  orig.Name("Device1");
  orig.Description("Description1");
  orig.Identity(11);
  orig.Protocol("Protocol1");
  orig.Bus("Bus1");

  auto orig_file = CreateXmlFile();
  ASSERT_TRUE(orig_file);
  auto& root_node = orig_file->RootName("DeviceList");
  orig.SaveXml(root_node);

  const std::string xml_string = orig_file->WriteString();

  std::cout << "ORIG XML" << std::endl;
  std::cout << xml_string << std::endl;

  auto dest_file = CreateXmlFile();
  dest_file->ParseString(xml_string);
  EXPECT_STREQ(dest_file->RootName().c_str(), "DeviceList");

  const auto* dest_node = dest_file->GetNode("Device");
  ASSERT_TRUE(dest_node != nullptr);

  Device dest;
  dest.ReadXml(*dest_node);

  EXPECT_STREQ(dest.Name().c_str(), "Device1");
  EXPECT_STREQ(dest.Description().c_str(), "Description1");
  EXPECT_EQ(dest.Identity(), 11);
  EXPECT_STREQ(dest.Protocol().c_str(), "Protocol1");
  EXPECT_STREQ(dest.Bus().c_str(), "Bus1");

}
} // end namespace workflow::test