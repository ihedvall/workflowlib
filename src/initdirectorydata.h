/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/irunner.h"

namespace workflow {

class InitDirectoryData : public IRunner {
 public:
  InitDirectoryData();
  explicit InitDirectoryData(const IRunner& source);
  void Init() override;
  void Tick() override;
 private:
  size_t data_slot_ = 0;
  std::string root_dir_;
  std::string include_filter_;
  std::string exclude_filter_;

  void ParseArguments();
};

}  // namespace workflow
