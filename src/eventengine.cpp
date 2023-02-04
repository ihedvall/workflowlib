/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflow/eventengine.h"
#include <algorithm>
#include <ranges>


using namespace util::xml;

namespace workflow {

EventEngine::~EventEngine() {
  EventEngine::Exit();
}

EventEngine::EventEngine(const EventEngine& engine)
: initialized_(false) {
  for (const auto& event : engine.event_list_) {
    if (!event) {
      continue;
    }
    auto temp = std::make_unique<IEvent>(*event);
    event_list_.push_back(std::move(temp));
  }
}

bool EventEngine::operator==(const EventEngine& engine) const {
  const auto event_equal = std::ranges::equal(
      event_list_, engine.event_list_,
      [] (const auto& event1, const auto& event2) {
        if (!event1 && !event2) return true;
        return event1 && event2 && (*event1 == *event2);
      });
  return event_equal;
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

void EventEngine::SaveXml(util::xml::IXmlNode& root) const {
  if (event_list_.empty()) {
    return;
  }

  auto& event_root = root.AddNode("EventList");
  for ( const auto& event : event_list_) {
    event->SaveXml(event_root);
  }
}

void EventEngine::ReadXml(const util::xml::IXmlNode& root) {
  const auto* event_root = root.GetNode("EventList");
  if (event_root == nullptr) {
    return;
  }
  IXmlNode::ChildList list;
  event_root->GetChildList(list);
  event_list_.clear();
  for (const auto* item : list) {
    if (item == nullptr || !item->IsTagName("Event")) {
      continue;
    }
    auto event = std::make_unique<IEvent>();
    event->ReadXml(*item);
    event_list_.push_back(std::move(event));
  }
}

void EventEngine::Clear() {
  initialized_ = false;
  event_list_.clear();
}



}  // namespace workflow