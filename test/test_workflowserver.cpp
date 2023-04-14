/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */
#include <gtest/gtest.h>
#include "workflow/workflowserver.h"
#include <util/stringutil.h>

namespace workflow::test {

TEST(WorkflowServer, Compare) {
  WorkflowServer orig;
  WorkflowServer copy(orig);
  EXPECT_TRUE(copy == orig);
  copy.Name("olle");
  EXPECT_FALSE(copy == orig);

  copy = orig;
  EXPECT_TRUE(copy == orig);

  copy.SetApplicationProperty("Pelle", "Olle Troll");
  copy.SetApplicationProperty("Petter/Kalle", "Nisse Hult");
  copy.SetApplicationProperty("Boolean", "true");
  copy.SetApplicationProperty("Integer", std::to_string(123));
  copy.SetApplicationProperty("Float", util::string::DoubleToString(1.0/3));

  EXPECT_FALSE(copy == orig);
  const auto val1 = copy.GetApplicationProperty<std::string>("Pelle");
  const auto val2 = copy.GetApplicationProperty<std::string>("Petter/Kalle");
  const auto val3 = copy.GetApplicationProperty<bool>("Boolean");
  const auto val4 = copy.GetApplicationProperty<int>("Integer");
  const auto val5 = copy.GetApplicationProperty<double>("Float");

  EXPECT_STREQ(val1.c_str(), "Olle Troll");
  EXPECT_STREQ(val2.c_str(), "Nisse Hult");
  EXPECT_TRUE(val3);
  EXPECT_EQ(val4, 123);
  EXPECT_DOUBLE_EQ(val5, 1.0/3);

  WorkflowServer copy1 = copy;
  EXPECT_TRUE(copy1 == copy);
  EXPECT_FALSE(copy1 == orig);
}

}