/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#pragma managed(push, off)
#include <mutex>
#pragma managed(pop)

#include <util/ixmlnode.h>

namespace workflow {
enum class ParameterDataType : int {
  FloatType = 0, ///< 64-bit floating point
  SignedType,
  UnsignedType,
  BooleanType,
  StringType,
  EnumType,
  ByteArrayType,
};

using ByteArray = std::vector<uint8_t>;
using EnumList = std::map<int64_t, std::string>;


class IParameter {
 public:
  IParameter() = default;
  virtual ~IParameter();

  IParameter(const IParameter& parameter);
  [[nodiscard]] bool operator == (const IParameter& parameter) const;

  void Name(const std::string& name) { name_ = name; }
  [[nodiscard]] const std::string& Name() const { return name_; }

  void DisplayName(const std::string& display_name) {
    display_name_ = display_name;
  }
  [[nodiscard]] const std::string& DisplayName() const { return display_name_; }

  void Description(const std::string& description) {
    description_ = description;
  }
  [[nodiscard]] const std::string& Description() const { return description_; }

  void Unit(const std::string& unit) { unit_ = unit; }
  [[nodiscard]] const std::string& Unit() const { return unit_; }

  void Device(const std::string& device) { device_ = device; }
  [[nodiscard]] const std::string& Device() const { return device_; }

  void Identity(const std::string& identity) { identity_ = identity; }
  [[nodiscard]] const std::string& Identity() const { return identity_; }

  void Signal(const std::string& signal) { signal_ = signal; }
  [[nodiscard]] const std::string& Signal() const { return signal_; }

  void DataType(ParameterDataType type) { data_type_ = type; }
  [[nodiscard]] ParameterDataType DataType() const { return data_type_; }
  void DataTypeAsString(const std::string& type);
  [[nodiscard]] std::string DataTypeAsString() const;

  void Enums(const EnumList& enum_list) {enum_list_ = enum_list;}
  [[nodiscard]] const EnumList& Enums() const {return enum_list_;}

  void Valid(bool valid);
  [[nodiscard]] bool Valid() const;

  template <typename T>
  bool GetValue(T& value) const;

  template <typename T>
  void SetValue(bool valid, const T& value);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);

 private:
  std::string name_; ///< Name used internally
  std::string unit_; ///< Unit of measure
  std::string description_; ///< optional description of the parameter
  std::string device_; ///< Test equipment reference
  std::string signal_; ///< Signal or channel name
  std::string identity_; ///< Free of use but normally external ID
  std::string display_name_; ///< Display name used as label

  ParameterDataType data_type_ = ParameterDataType::FloatType;
  EnumList enum_list_;

  // Thread safe handling of valid and value objects
  mutable std::mutex value_lock_;
  bool valid_ = false;
  double value_float_ = 0.0;
  int64_t value_int_ = 0;
  uint64_t value_uint_ = 0;
  std::string value_text_;
  ByteArray value_array_;

};

template <typename T>
bool IParameter::GetValue(T& value) const {
  std::scoped_lock lock(value_lock_);
  switch (data_type_) {
    case ParameterDataType::UnsignedType:
    case ParameterDataType::BooleanType:
      value = static_cast<T>(value_uint_);
      break;

    case ParameterDataType::EnumType:
    case ParameterDataType::SignedType:
      value = static_cast<T>(value_int_);
      break;

    case ParameterDataType::FloatType:
      value = static_cast<T>(value_float_);
      break;

    case ParameterDataType::StringType: {
      try {
        std::istringstream input(value_text_);
        input >> value;
      } catch( const std::exception& ) {
      }
      break;
    }

    case ParameterDataType::ByteArrayType:
    default:
      break;
  }
  return valid_;
}

template <>
bool IParameter::GetValue(bool& value) const;

template <>
bool IParameter::GetValue(std::string& value) const;

template <>
bool IParameter::GetValue(ByteArray& value) const;

template <typename T>
void IParameter::SetValue(bool valid, const T& value) {
  std::scoped_lock lock(value_lock_);
  valid_ = valid;

  switch (data_type_) {
    case ParameterDataType::BooleanType:
    case ParameterDataType::UnsignedType:
      value_uint_ = static_cast<uint64_t>(value);
      break;

    case ParameterDataType::EnumType:
    case ParameterDataType::SignedType:
      value_int_ = static_cast<int64_t>(value);
      break;

    case ParameterDataType::FloatType:
      value_float_ = static_cast<double>(value);
      break;

    case ParameterDataType::StringType: {
      try {
        value_text_ = std::to_string(value);
      } catch( const std::exception& ) {
      }
      break;
    }

    case ParameterDataType::ByteArrayType:
    default:
      break;
  }
}

template <>
void IParameter::SetValue(bool valid, const bool& value);

template <>
void IParameter::SetValue(bool valid, const std::string& value);

template <>
void IParameter::SetValue(bool valid, const ByteArray& value);


}  // namespace workflow
