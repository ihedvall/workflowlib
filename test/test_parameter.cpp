/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include "workflow/iparameter.h"
#include <cmath>

namespace workflow::test {

TEST(IParameter, TestProperties)
{
  IParameter par;
  par.Name("Olle");
  par.DisplayName("Olle Display Name");
  par.Description("Olle Description");
  par.Unit("ms");
  par.Device("Olle Device");
  par.Identity("Olle Identity");
  par.Signal("Olle Signal");
  par.DataType(ParameterDataType::ByteArrayType);


  std::cout << "IParameter Size: " << sizeof(par) << std::endl;
  std::cout << "Name: " << par.Name() << std::endl;
  std::cout << "Display Name: " << par.DisplayName() << std::endl;
  std::cout << "Description: " << par.Description() << std::endl;
  std::cout << "Unit: " << par.Unit() << std::endl;
  std::cout << "Device: " << par.Device() << std::endl;
  std::cout << "Identity: " << par.Identity() << std::endl;
  std::cout << "Signal: " << par.Signal() << std::endl;
  std::cout << "Data Type: " << par.DataTypeAsString() << std::endl;

  par.Valid(true);
  EXPECT_TRUE(par.Valid());
  par.Valid(false);
  EXPECT_FALSE(par.Valid());

}

TEST(IParameter, TestFloat) {
  IParameter par;
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
    EXPECT_TRUE(isnan(output));
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

TEST(IParameter, TestInteger) {
  IParameter par;
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

TEST(IParameter, TestUnsigned) {
  IParameter par;
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

TEST(IParameter, TestBoolean) {
  IParameter par;
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

TEST(IParameter, TestEnumerate) {
  IParameter par;
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

TEST(IParameter, TestString) {
  IParameter par;
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

TEST(IParameter, TestByteArray) {
  IParameter par;
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
