/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/itask.h"

namespace workflow {


class InitDirectoryData : public ITask {
 public:
  InitDirectoryData();
  explicit InitDirectoryData(const ITask& source);
  void Init() override;
  void Tick() override;
 private:
  std::string root_dir_;
  std::string include_filter_;
  std::string exclude_filter_;

  void ParseArguments();
};

}  // namespace workflow
