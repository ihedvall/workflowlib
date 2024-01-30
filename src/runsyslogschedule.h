/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include "workflow/irunner.h"


namespace workflow {

class RunSyslogSchedule : public IRunner {
 public:
  RunSyslogSchedule();
  explicit RunSyslogSchedule(const IRunner& source);
  void Init() override;
  void Tick() override;

 private:
  std::string schedule_name_ = "SyslogSchedule"; ///<  Schedule name
  void ParseArguments();
};

}  // namespace workflow
