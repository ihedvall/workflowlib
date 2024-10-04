/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include "workflow/event.h"
#include <memory>
#include <map>
#include <util/ixmlnode.h>
#include <util/stringutil.h>
namespace workflow {

using EventList = std::map<std::string, std::unique_ptr<Event>,
    util::string::IgnoreCase>;

class EventEngine {
 public:
  EventEngine();
  virtual ~EventEngine();

  EventEngine(const EventEngine& engine);
  [[nodiscard]] bool operator == (const EventEngine& engine) const;


  [[nodiscard]] EventList& Events() {return event_list_;}
  [[nodiscard]] const EventList& Events() const {return event_list_;}
  [[nodiscard]] const Event* GetEvent(const std::string& name) const;
  [[nodiscard]] Event* GetEvent(const std::string& name);
  void AddEvent(const Event& event);
  void DeleteEvent(const Event* event);

  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);
  void Clear();


  void DetachWorkflows();
 protected:

  [[nodiscard]] virtual std::unique_ptr<Event> MakeEvent(const Event&
                                                                source);
  bool initialized_ = false;
 private:
  EventList event_list_;
  void AddDefaultEvents();
};

}  // namespace workflow
