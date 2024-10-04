/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/itask.h"
#include "util/isyslogserver.h"
#include <memory>

namespace workflow {

class SyslogPublisher : public ITask {
 public:
  SyslogPublisher();
  explicit SyslogPublisher(const ITask& source);
  void Init() override;
  void Tick() override;
  void Exit() override;

 private:
  std::string address_ = "127.0.0.1"; ///<  0.0.0.0 accept remote connects
  uint16_t port_ = 42515;
  std::unique_ptr<util::syslog::ISyslogServer> server_;

  void ParseArguments();
};

}  // namespace workflow
