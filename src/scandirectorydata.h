/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/itask.h"

namespace workflow {

class ScanDirectoryData : public ITask {
 public:
  ScanDirectoryData();
  explicit ScanDirectoryData(const ITask& source);
  void Init() override;
  void Tick() override;
 private:

  void ParseArguments();
};

}  // namespace workflow
