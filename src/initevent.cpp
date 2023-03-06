/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "initevent.h"

namespace workflow {

InitEvent::InitEvent() {
  Name("InitEvent"),
  Description("First event in the workflow server");
  Type(EventType::Internal);
}

InitEvent::InitEvent(const IEvent& source)
: IEvent(source) {
}

void InitEvent::Init() {
  IEvent::Init();
  IEvent::Tick();
}

void InitEvent::Tick() {
}


}  // namespace workflow