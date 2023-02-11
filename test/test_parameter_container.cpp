/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/
#include <gtest/gtest.h>
#include "workflow/parametercontainer.h"


namespace workflow::test {
TEST(ParameterContainer, TestDefault) {
  ParameterContainer list();
  list.IgnoreCase(false);
  list.UniqueName(false);
  EXPECT_EQ(list.Parameters().size(), 0);
  {
    // Should not add no named items
    auto par = std::make_unique <IParameter>();
    EXPECT_EQ(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 0);
  }
  {
    // Normal parameter
    auto par = std::make_unique <IParameter>();
    par->Name("Olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 1);
  }
  {
    // Similar name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 2);
  }
  {
    // Same name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 3);
  }
  list.DeleteParameter("olle");
  EXPECT_EQ(list.Parameters().size(), 2);
  list.DeleteParameter("olle");
  EXPECT_EQ(list.Parameters().size(), 1);

  list.DeleteParameter("olle");
  EXPECT_EQ(list.Parameters().size(), 1);

  list.DeleteParameter("Olle");
  EXPECT_EQ(list.Parameters().size(), 0);
}

TEST(ParameterContainer, TestUniqueName) {
  ParameterContainer list;
  list.IgnoreCase(false);
  list.UniqueName(true);
  EXPECT_EQ(list.Parameters().size(), 0);
  {
    // Normal parameter
    auto par = std::make_unique <IParameter>();
    par->Name("Olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 1);
  }
  {
    // Similar name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 2);
  }
  {
    // Same name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 2);
  }
  list.DeleteParameter("olle");
  EXPECT_EQ(list.Parameters().size(), 1);
  list.DeleteParameter("olle");
  EXPECT_EQ(list.Parameters().size(), 1);
}

TEST(ParameterContainer, TestIgnoreName) {
  ParameterContainer list;
  list.IgnoreCase(true);
  list.UniqueName(true);
  EXPECT_EQ(list.Parameters().size(), 0);
  {
    // Normal parameter
    auto par = std::make_unique <IParameter>();
    par->Name("Olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 1);
  }
  {
    // Similar name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 1);
  }
  {
    // Same name
    auto par = std::make_unique <IParameter>();
    par->Name("olle");
    EXPECT_NE(list.AddParameter(par), nullptr);
    EXPECT_EQ(list.Parameters().size(), 1);
  }
  list.DeleteParameter("oLLe");
  EXPECT_EQ(list.Parameters().size(), 0);
}

TEST(ParameterContainer, TestByType) {
  ParameterContainer list;
  list.IgnoreCase(true);
  list.UniqueName(true);

  auto olle = std::make_unique <IParameter>();
  olle->Name("Olle");
  list.AddParameter(olle);
  auto pelle = std::make_unique <IParameter>();
  pelle->Name("Pelle");
  list.AddParameter(pelle);

  EXPECT_EQ(list.Parameters().size(), 2);

  std::vector<IParameter*> temp_list;
  list.ParametersByType(temp_list);
  EXPECT_EQ(temp_list.size(), 2);

  EXPECT_NE(list.GetParameter("olle"), nullptr);
}

} // end namespace