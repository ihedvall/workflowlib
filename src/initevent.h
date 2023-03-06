/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once

#include "workflow/ievent.h"

namespace workflow {

class InitEvent : public IEvent {
 public:
  InitEvent();
  explicit InitEvent(const IEvent& source);
  void Init() override;
  void Tick() override;
};

}  // namespace workflow
