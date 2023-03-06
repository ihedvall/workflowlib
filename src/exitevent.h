/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */


#pragma once

#include "workflow/ievent.h"

namespace workflow {

class ExitEvent  : public IEvent {
 public:
  ExitEvent();
  explicit ExitEvent(const IEvent& source);
  void Tick() override;
  void Exit() override;
};

}  // namespace workflow
