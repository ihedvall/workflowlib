/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflow/eventengine.h"
#include <algorithm>
#include <ranges>
#include "initevent.h"
#include "exitevent.h"
#include "periodicevent.h"
#include "cyclicEvent.h"

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
    IEvent temp;
    temp.ReadXml(*item);
    AddEvent(temp);
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
  auto itr = event_list_.find(event.Name());
  if (itr == event_list_.end()) {
    event_list_.emplace(temp->Name(), std::move(temp));
  } else {
    itr->second = std::move(temp);
  }
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

void EventEngine::DetachWorkflows() {
  for (auto& itr : event_list_) {
    if (itr.second) {
      itr.second->DetachWorkflows();
    }
  }
}

std::unique_ptr<IEvent> EventEngine::MakeEvent(const IEvent& source) {
  std::unique_ptr<IEvent> event;
  switch (source.Type()) {
    case EventType::Internal:
      if (source.Name() == "InitEvent") {
        auto temp = std::make_unique<InitEvent>(source);
        event = std::move(temp);
      } else if (source.Name() == "ExitEvent") {
        auto temp = std::make_unique<ExitEvent>(source);
        event = std::move(temp);
      } else {
        auto temp = std::make_unique<IEvent>(source);
        event = std::move(temp);
      }
      break;

    case EventType::Periodic:
      if (source.Name() == "PeriodEvent") {
        auto temp = std::make_unique<PeriodicEvent>(source);
        event = std::move(temp);
      } else if (source.Name() == "CyclicEvent") {
        auto temp = std::make_unique<CyclicEvent>(source);
        event = std::move(temp);
      } else {
        auto temp = std::make_unique<IEvent>(source);
        event = std::move(temp);
      }
      break;

    case EventType::External:
    case EventType::Parameter:
    default: {
      auto temp = std::make_unique<IEvent>(source);
      event = std::move(temp);
      break;
    }
  }
  return event;
}

void EventEngine::AddDefaultEvents() {
 InitEvent init_event;
 AddEvent(init_event);

 ExitEvent exit_event;
 AddEvent(exit_event);

 CyclicEvent cyclic_event;
 AddEvent(cyclic_event);

 PeriodicEvent periodic_event;
 AddEvent(periodic_event);
}


}  // namespace workflow