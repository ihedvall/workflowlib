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
  for (const auto& itr : engine.event_list_) {
    const auto* event = itr.second.get();
    if (event == nullptr) {
      continue;
    }
    auto temp = std::make_unique<IEvent>(*event);
    event_list_.emplace(event->Name(), std::move(temp));
  }
}

bool EventEngine::operator==(const EventEngine& engine) const {
  const auto event_equal = std::ranges::equal(
      event_list_, engine.event_list_,
      [] (const auto& itr1, const auto& itr2) {
        const auto* event1 = itr1.second.get();
        const auto* event2 = itr1.second.get();
        if (event1 == nullptr && event2 == nullptr) return true;
        return event1 != nullptr && event2 != nullptr && (*event1 == *event2);
      });
  return event_equal;
}

void EventEngine::Init() {
  if (initialized_ ) {
    return;
  }

  for (auto& itr : event_list_) {
    auto* event = itr.second.get();
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
    auto* event = itr.second.get();
    if (event != nullptr) {
      event->Exit();
    }
  }

  initialized_ = false;
}

void EventEngine::Tick() {
  for (auto& itr : event_list_) {
    auto* event = itr.second.get();
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
  for ( const auto& itr : event_list_) {
    auto* event = itr.second.get();
    if (event == nullptr) {
      continue;
    }
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
    event_list_.emplace(event->Name(), std::move(event));
  }
}

void EventEngine::Clear() {
  initialized_ = false;
  event_list_.clear();
}

const IEvent* EventEngine::GetEvent(const std::string& name) const {
  const auto itr = event_list_.find(name);
  return itr == event_list_.cend() ? nullptr : itr->second.get();
}

IEvent* EventEngine::GetEvent(const std::string& name) {
  auto itr = event_list_.find(name);
  return itr == event_list_.end() ? nullptr : itr->second.get();
}

void EventEngine::AddEvent(const IEvent& event) {
  auto temp = std::make_unique<IEvent>(event);
  event_list_.emplace(temp->Name(), std::move(temp));
}

void EventEngine::DeleteEvent(const IEvent* event) {
  if (event == nullptr) {
    return;
  }
  auto itr = event_list_.find(event->Name());
  if (itr != event_list_.end()) {
    event_list_.erase(itr);
  }
}

}  // namespace workflow