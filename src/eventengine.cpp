/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflow/eventengine.h"

namespace workflow {
EventEngine::~EventEngine() {
  Stop();
}

void EventEngine::Init() {
  if (initialized_ ) {
    return;
  }

  for (auto& itr : event_list_) {
    auto* event = itr.get();
    if (event != nullptr) {
      event->Init();
    }
  }
  initialized_ = true;
}

void EventEngine::Exit() {
  if (!initialized_ ) {
    return;
  }

  for (auto& itr : event_list_) {
    auto* event = itr.get();
    if (event != nullptr) {
      event->Exit();
    }
  }

  initialized_ = false;
}

void EventEngine::Tick() {
  for (auto& itr : event_list_) {
    auto* event = itr.get();
    if (event != nullptr) {
      event->Tick();
    }
  }
}


}  // namespace workflow