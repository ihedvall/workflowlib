/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include "workflow/itask.h"


namespace workflow {

class RunSyslogSchedule : public ITask {
 public:
  RunSyslogSchedule();
  explicit RunSyslogSchedule(const ITask& source);
  void Init() override;
  void Tick() override;

 private:
  std::string schedule_name_ = "SyslogSchedule"; ///<  Schedule name
  void ParseArguments();
};

}  // namespace workflow
