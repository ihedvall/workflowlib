/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include "workflow/parameter.h"
#include <cmath>
#include <util/ixmlfile.h>

using namespace util::xml;

namespace workflow::test {

TEST(Parameter, TestProperties)
{
  Parameter par;
  par.Name("Olle");
  par.DisplayName("Olle Display Name");
  par.Description("Olle Description");
  par.Unit("ms");
  par.Device("Olle Device");
  par.Identity("Olle Identity");
  par.Signal("Olle Signal");
  par.DataType(ParameterDataType::ByteArrayType);

  const EnumList enum_list = {
      {0,"Default Enum"},
      {1,"First Enum"}
  };
  par.Enums(enum_list);

  EXPECT_STREQ(par.Name().c_str(), "Olle");
  EXPECT_STREQ(par.DisplayName().c_str(), "Olle Display Name");
  EXPECT_STREQ(par.Description().c_str(), "Olle Description");
  EXPECT_STREQ(par.Unit().c_str(), "ms");
  EXPECT_STREQ(par.Device().c_str(), "Olle Device");
  EXPECT_STREQ(par.Identity().c_str(), "Olle Identity");
  EXPECT_STREQ(par.Signal().c_str(), "Olle Signal");
  EXPECT_EQ(par.DataType(), ParameterDataType::ByteArrayType);
  EXPECT_EQ(par.Enums(), enum_list);

  par.Valid(true);
  EXPECT_TRUE(par.Valid());
  par.Valid(false);
  EXPECT_FALSE(par.Valid());

  par.DataType(ParameterDataType::FloatType);
  par.SetValue(true, 1.23);
  EXPECT_TRUE(par.Valid());
  double value = 0.0;
  const bool valid = par.GetValue(value);
  EXPECT_TRUE(valid);
  EXPECT_DOUBLE_EQ(value, 1.23);

}

TEST(Parameter, TestXmlStorage)
{
  Parameter orig;
  orig.Name("Olle");
  orig.DisplayName("Olle Display Name");
  orig.Description("Olle Description");
  orig.Unit("ms");
  orig.Device("Olle Device");
  orig.Identity("Olle Identity");
  orig.Signal("Olle Signal");
  orig.DataType(ParameterDataType::ByteArrayType);

  const EnumList enum_list = {
      {0,"Default Enum"},
      {1,"First Enum"}
  };
  orig.Enums(enum_list);


  auto orig_file = CreateXmlFile();
  ASSERT_TRUE(orig_file);
  auto& root_node = orig_file->RootName("ParameterList");
  orig.SaveXml(root_node);

  const std::string xml_string = orig_file->WriteString();

  std::cout << "ORIG XML" << std::endl;
  std::cout << xml_string << std::endl;

  auto dest_file = CreateXmlFile();
  dest_file->ParseString(xml_string);
  EXPECT_STREQ(dest_file->RootName().c_str(), "ParameterList");

  const auto* dest_node = dest_file->GetNode("Parameter");
  ASSERT_TRUE(dest_node != nullptr);

  Parameter dest;
  dest.ReadXml(*dest_node);

  EXPECT_STREQ(dest.Name().c_str(), "Olle");
  EXPECT_STREQ(dest.DisplayName().c_str(), "Olle Display Name");
  EXPECT_STREQ(dest.Description().c_str(), "Olle Description");
  EXPECT_STREQ(dest.Unit().c_str(), "ms");
  EXPECT_STREQ(dest.Device().c_str(), "Olle Device");
  EXPECT_STREQ(dest.Identity().c_str(), "Olle Identity");
  EXPECT_STREQ(dest.Signal().c_str(), "Olle Signal");
  EXPECT_EQ(dest.DataType(), ParameterDataType::ByteArrayType);
  EXPECT_EQ(dest.Enums(), enum_list);
}

TEST(Parameter, TestFloat) {
  Parameter par;
  par.DataType(ParameterDataType::FloatType);
  {
    constexpr double input = 1.23;
    par.SetValue(true, input);
    double output = 0.0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_DOUBLE_EQ(input, output);
    std::cout << "Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr double input = 1.23;
    par.SetValue(false, input);
    double output = 0.0;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_DOUBLE_EQ(input, output);
  }

  {
    constexpr float input = 1.23;
    par.SetValue(true, input);
    float output = 0.0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_FLOAT_EQ(input, output);
  }

  {
    constexpr float input = 1.0/3.0;
    par.SetValue(true, input);
    float output = 0.0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_FLOAT_EQ(input, output);
  }

  {
    constexpr float input = 1.0/3.0;
    par.SetValue(true, input);
    std::string output;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_GT(output.size(), 0);
    std::cout << "Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr double input = std::numeric_limits<double>::quiet_NaN();
    par.SetValue(false, input);
    double output = 0.0;
    EXPECT_FALSE(par.GetValue(output));
    std::cout << "NaN Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr double input = std::numeric_limits<double>::signaling_NaN();
    par.SetValue(false, input);
    double output = 0.0;
    EXPECT_FALSE(par.GetValue(output));
    std::cout << "NaN Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr float input = std::numeric_limits<float>::quiet_NaN();
    par.SetValue(false, input);
    float output = 0.0;
    EXPECT_FALSE(par.GetValue(output));
    std::cout << "NaN Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr float input = std::numeric_limits<float>::quiet_NaN();
    par.SetValue(false, input);
    double output = 0.0;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_TRUE(std::isnan(output));
    std::cout << "NaN Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr int input = -12;
    par.SetValue(true, input);
    int output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
    std::cout << "Int Float: " << input << "/" << output << std::endl;
  }

  {
    constexpr int input = -12;
    par.SetValue(true, input);
    double output = 0;
    EXPECT_TRUE(par.GetValue(output));
    std::cout << "Int Float: " << input << "/" << output << std::endl;
  }
}

TEST(Parameter, TestInteger) {
  Parameter par;
  par.DataType(ParameterDataType::SignedType);
  {
    constexpr int64_t input = -1111;
    par.SetValue(true, input);
    int64_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
    std::cout << "Integer: " << input << "/" << output << std::endl;
  }

  {
    constexpr int64_t input = 2222;
    par.SetValue(false, input);
    int64_t output = 0;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    constexpr int32_t input = -3333;
    par.SetValue(true, input);
    int32_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }


  {
    constexpr int64_t input = std::numeric_limits<int64_t>::max();
    par.SetValue(true, input);
    int64_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }
  {
    constexpr int64_t input = std::numeric_limits<int64_t>::min();
    par.SetValue(true, input);
    int64_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    constexpr int64_t input = -1111;
    par.SetValue(true, input);
    std::string output;
    EXPECT_TRUE(par.GetValue(output));
    std::cout << "Integer: " << input << "/" << output << std::endl;
  }

}

TEST(Parameter, TestUnsigned) {
  Parameter par;
  par.DataType(ParameterDataType::UnsignedType);
  {
    constexpr uint64_t input = 1111;
    par.SetValue(true, input);
    uint64_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
    std::cout << "Unsigned: " << input << "/" << output << std::endl;
  }

  {
    constexpr uint64_t input = 2222;
    par.SetValue(false, input);
    uint64_t output = 0;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    constexpr uint32_t input = 3333;
    par.SetValue(true, input);
    uint32_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }


  {
    constexpr uint64_t input = std::numeric_limits<uint64_t>::max();
    par.SetValue(true, input);
    uint64_t output = 0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }
  {
    constexpr uint64_t input = std::numeric_limits<uint64_t>::min();
    par.SetValue(true, input);
    uint64_t output = 1;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    constexpr uint64_t input = 1111;
    par.SetValue(true, input);
    std::string output;
    EXPECT_TRUE(par.GetValue(output));
    std::cout << "Unsigned: " << input << "/" << output << std::endl;
  }

}

TEST(Parameter, TestBoolean) {
  Parameter par;
  par.DataType(ParameterDataType::BooleanType);
  {
    constexpr bool input = true;
    par.SetValue(true, input);
    bool output = false;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
    std::cout << "Boolean: " << input << "/" << output << std::endl;
  }

  {
    constexpr bool input = false;
    par.SetValue(false, input);
    bool output = true;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    constexpr bool input = true;
    par.SetValue(true, input);
    std::string output;
    EXPECT_TRUE(par.GetValue(output));
    std::cout << "Boolean: " << input << "/" << output << std::endl;
  }

  {
    const std::string input = "ON";
    par.SetValue(true, input);
    bool output = false;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_TRUE(output);
  }
  {
    const std::string input = "false";
    par.SetValue(true, input);
    bool output = false;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_FALSE(output);
  }
}

TEST(Parameter, TestEnumerate) {
  Parameter par;
  par.DataType(ParameterDataType::EnumType);
  const EnumList enum_list = {
      {0, "Invalid"},
      {1, "ON"},
      {2, "OFF"},
  };
  par.Enums(enum_list);

  {
    for (size_t key = 0; key < enum_list.size(); ++key) {
      par.SetValue(true, key);
      int64_t output1 = -1;
      uint64_t output2 = 11;
      std::string output3;
      EXPECT_TRUE(par.GetValue(output1));
      EXPECT_TRUE(par.GetValue(output2));
      EXPECT_TRUE(par.GetValue(output3));
      EXPECT_EQ(key, static_cast<size_t>(output1));
      EXPECT_EQ(key, output2);
      std::cout << "Enum:  " << key << ":"
                << output1 << ", "
                << output2 << ", "
                << output3 << std::endl;
    }
  }

  {
    for (const auto& itr : enum_list) {
      const std::string& input = itr.second;
      par.SetValue(true, input);
      int64_t output1 = -1;
      uint64_t output2 = 11;
      std::string output3;
      EXPECT_TRUE(par.GetValue(output1));
      EXPECT_TRUE(par.GetValue(output2));
      EXPECT_TRUE(par.GetValue(output3));
      EXPECT_EQ(input, output3);
      std::cout << "Enum:  " << input << ":"
                << output1 << ", "
                << output2 << ", "
                << output3 << std::endl;
    }
  }
  {
    par.SetValue(true, true);
    int64_t output1 = -1;
    uint64_t output2 = 11;
    std::string output3;
    EXPECT_TRUE(par.GetValue(output1));
    EXPECT_TRUE(par.GetValue(output2));
    EXPECT_TRUE(par.GetValue(output3));

    std::cout << "Enum:  " << "True" << ":"
              << output1 << ", "
              << output2 << ", "
              << output3 << std::endl;
  }

  {
    par.SetValue(true, false);
    int64_t output1 = -1;
    uint64_t output2 = 11;
    std::string output3;
    EXPECT_TRUE(par.GetValue(output1));
    EXPECT_TRUE(par.GetValue(output2));
    EXPECT_TRUE(par.GetValue(output3));

    std::cout << "Enum:  " << "False" << ":"
              << output1 << ", "
              << output2 << ", "
              << output3 << std::endl;
  }

  {
    par.SetValue(true, std::string("OFF"));
    int64_t output1 = -1;
    uint64_t output2 = 11;
    std::string output3;
    EXPECT_TRUE(par.GetValue(output1));
    EXPECT_TRUE(par.GetValue(output2));
    EXPECT_TRUE(par.GetValue(output3));

    std::cout << "Enum:  " << "OFF" << ":"
              << output1 << ", "
              << output2 << ", "
              << output3 << std::endl;
  }
}

TEST(Parameter, TestString) {
  Parameter par;
  par.DataType(ParameterDataType::StringType);
  {
    const std::string input = "Olle";
    par.SetValue(true, input);
    std::string output;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(input, output);
    std::cout << "String: " << input << "/" << output << std::endl;
  }
  {
    const std::string input = "Pelle";
    par.SetValue(false, input);
    std::string output;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_EQ(input, output);
  }

  {
    const std::string input = "1.23";
    par.SetValue(true, input);
    double output = 0.0;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_DOUBLE_EQ(1.23, output);
  }

  {
    const std::string input = "True";
    par.SetValue(true, input);
    bool output = false;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_TRUE(output);
  }
}

TEST(Parameter, TestByteArray) {
  Parameter par;
  par.DataType(ParameterDataType::ByteArrayType);
  {
    const ByteArray input = {'A', 'B','C'};
    par.SetValue(true, input);
    ByteArray output;
    EXPECT_TRUE(par.GetValue(output));
    EXPECT_EQ(3, output.size());
    EXPECT_EQ('A', output[0]);
  }

  {
    const ByteArray input = {'A', 'B','C'};
    par.SetValue(false, input);
    std::string output;
    EXPECT_FALSE(par.GetValue(output));
    EXPECT_EQ(3, output.size());
    EXPECT_EQ('A', output[0]);
    std::cout << "Byte Array: " << output << std::endl;
  }
}
}  // namespace mdf::test
