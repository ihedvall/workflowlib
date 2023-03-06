/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include "workflow/irunner.h"
#include "util/isyslogserver.h"

namespace workflow {

class SyslogInput : public IRunner {
 public:
  SyslogInput();
  explicit SyslogInput(const IRunner& source);
  void Init() override;
  void Tick() override;
  void Exit() override;

 private:
  size_t data_slot_ = 0;
  std::string address_ = "127.0.0.1"; ///<  0.0.0.0 accept remote connects
  uint16_t port_ = 514;
  std::string type_ = "UDP"; ///< For future use (UDP/TCP or TLS)

  util::syslog::ISyslogServer server_;

  void ParseArguments();

};

}  // namespace workflow