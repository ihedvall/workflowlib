/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "exitevent.h"

namespace workflow {

ExitEvent::ExitEvent() {
  Name("ExitEvent"),
      Description("Last event in the workflow server");
  Type(EventType::Internal);
}

ExitEvent::ExitEvent(const IEvent& source)
    : IEvent(source) {

}

void ExitEvent::Tick() {
}

void ExitEvent::Exit() {
  IEvent::Tick();
  IEvent::Exit();
}


}  // namespace workflow