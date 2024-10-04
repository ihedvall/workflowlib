/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflow/eventengine.h"
#include <algorithm>
#include <ranges>

using namespace util::xml;

namespace workflow {

EventEngine::EventEngine() {
  AddDefaultEvents();
}

EventEngine::~EventEngine() {
  EventEngine::Exit();
}

EventEngine::EventEngine(const EventEngine& engine)
: initialized_(false) {
  AddDefaultEvents();
  for (const auto& itr : engine.event_list_) {
    const auto* event = itr.second.get();
    if (event == nullptr) {
      continue;
    }
    AddEvent(*event);
  }

}

bool EventEngine::operator==(const EventEngine& engine) const {
  const auto event_equal = std::ranges::equal(
      event_list_, engine.event_list_,
      [] (const auto& itr1, const auto& itr2) {
        const auto* event1 = itr1.second.get();
        const auto* event2 = itr2.second.get();
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
    AddDefaultEvents();
    return;
  }
  IXmlNode::ChildList list;
  event_root->GetChildList(list);
  event_list_.clear();
  AddDefaultEvents();
  for (const auto* item : list) {
    if (item == nullptr || !item->IsTagName("Event")) {
      continue;
    }
    Event temp;
    temp.ReadXml(*item);
    AddEvent(temp);
  }
}

void EventEngine::Clear() {
  initialized_ = false;
  event_list_.clear();
}

const Event* EventEngine::GetEvent(const std::string& name) const {
  const auto itr = event_list_.find(name);
  return itr == event_list_.cend() ? nullptr : itr->second.get();
}

Event* EventEngine::GetEvent(const std::string& name) {
  auto itr = event_list_.find(name);
  return itr == event_list_.end() ? nullptr : itr->second.get();
}

void EventEngine::AddEvent(const Event& event) {
  auto temp = std::make_unique<Event>(event);
  auto itr = event_list_.find(event.Name());
  if (itr == event_list_.end()) {
    event_list_.emplace(temp->Name(), std::move(temp));
  } else {
    itr->second = std::move(temp);
  }
}

void EventEngine::DeleteEvent(const Event* event) {
  if (event == nullptr) {
    return;
  }
  auto itr = event_list_.find(event->Name());
  if (itr != event_list_.end()) {
    event_list_.erase(itr);
  }
}

void EventEngine::DetachWorkflows() {
  for (auto& itr : event_list_) {
    if (itr.second) {
      itr.second->DetachWorkflows();
    }
  }
}

std::unique_ptr<Event> EventEngine::MakeEvent(const Event& source) {
  return std::make_unique<Event>(source);
}

void EventEngine::AddDefaultEvents() {
 Event init_event;
 init_event.Name("InitEvent"),
 init_event.Description("First event in the workflow server");
 init_event.Type(EventType::Init);
 AddEvent(init_event);

 Event exit_event;
 exit_event.Name("ExitEvent"),
 exit_event.Description("Last event in the workflow server");
 exit_event.Type(EventType::Exit);
 AddEvent(exit_event);

 Event cyclic_event;
 cyclic_event.Name("CyclicEvent_1s"),
 cyclic_event.Description("Event that happens cyclic each second.");
 cyclic_event.Type(EventType::Cyclic);
 cyclic_event.Period(1000);
 AddEvent(cyclic_event);

 Event periodic_event;
 periodic_event.Name("PeriodicEvent_100Hz"),
 periodic_event.Description("Event that happens periodically (exactly 10Hz).");
 periodic_event.Type(EventType::Periodic);
 periodic_event.Period(100);
 AddEvent(periodic_event);
}


}  // namespace workflow