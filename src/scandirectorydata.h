/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/irunner.h"

namespace workflow {

class ScanDirectoryData : public IRunner {
 public:
  ScanDirectoryData();
  explicit ScanDirectoryData(const IRunner& source);
  void Init() override;
  void Tick() override;
 private:

  void ParseArguments();
};

}  // namespace workflow
