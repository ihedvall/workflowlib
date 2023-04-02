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
  size_t in_slot_ = 0; ///< Data slot for incoming messages
  size_t out_slot_ = 0; ///< Data slot for outgoing messages
  std::string schedule_name_ = "SyslogSchedule"; ///<  Schedule name
  void ParseArguments();
};

}  // namespace workflow
