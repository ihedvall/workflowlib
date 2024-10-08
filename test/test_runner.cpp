/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include <gtest/gtest.h>
#include "workflow/event.h"
#include <string_view>
#include <boost/program_options.hpp>

using namespace boost::program_options;

namespace workflow::test {

TEST(IRunner, Properties) {
  ITask task;

  constexpr std::string_view kOrigName = "William";
  task.Name(kOrigName.data());
  EXPECT_STREQ(task.Name().c_str(), kOrigName.data() );

  constexpr std::string_view kOrigDesc = "Adama";
  task.Description(kOrigDesc.data());
  EXPECT_STREQ(task.Description().c_str(), kOrigDesc.data() );

  constexpr std::string_view kOrigDoc = "Human";
  task.Documentation(kOrigDoc.data());
  EXPECT_STREQ(task.Documentation().c_str(), kOrigDoc.data() );

  task.Type(TaskType::PythonTask);
  EXPECT_EQ(task.Type(), TaskType::PythonTask);

  task.Period(1.23); // seconds
  EXPECT_DOUBLE_EQ(task.Period(), 1.23);

}

TEST(IRunner, Boost_program_options) {
  size_t slot = 0;
  std::vector<std::string> root_dir;
  std::string include_filter;
  std::string exclude_filter;
  constexpr std::string_view option1 =
      "-S 11  -E *.eng;*.cpp \"c:/temp files\" pelle";

  options_description desc("Available Arguments");
  desc.add_options()
      ("help,H", "Help")
      ("slot,S", value<size_t>(&slot)->default_value(0), "Data slot for data" )
      ("include-filter,I", value<std::string>(&include_filter),
          "Include filter" )
      ("exclude-filter,E", value<std::string>(&exclude_filter),
          "Exclude filter" )
      ("root-dir,R", value<std::vector<std::string>>(&root_dir),
                       "Root directory" );

  positional_options_description pos;
  pos.add("root-dir", -1);

  const auto arg_list = split_unix(option1.data());
  for (const auto& arg : arg_list) {
    std::cout << arg << std::endl;
  }

  basic_command_line_parser parser(arg_list);
  parser.options(desc);
  parser.positional(pos);

  const auto opt = parser.run();

  variables_map var_list;
  store(opt,var_list);
  notify(var_list);

  EXPECT_FALSE(var_list.empty());

  std::cout << "Size: " << var_list.size() << std::endl;
  std::cout << "Slot: " << slot << std::endl;
  std::cout << "Include: " << include_filter << std::endl;
  std::cout << "Exclude: " << exclude_filter << std::endl;
  std::cout << "Root: " << root_dir[0] << std::endl;
  std::cout << "Root: " << root_dir[1] << std::endl;


  for (const auto& [key, _] : var_list) {
    std::cout << key << " : " << std::endl;
  }

}

}