/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include "workflow/ievent.h"
#include <memory>
#include <vector>
#include <util/ixmlnode.h>

namespace workflow {

using EventList = std::vector<std::unique_ptr<IEvent>>;

class EventEngine {
 public:
  EventEngine() = default;
  virtual ~EventEngine();

  EventEngine(const EventEngine& engine);
  [[nodiscard]] bool operator == (const EventEngine& engine) const;


  [[nodiscard]] EventList& Events() {return event_list_;}
  [[nodiscard]] const EventList& Events() const {return event_list_;}


  virtual void Init();
  virtual void Tick();
  virtual void Exit();

  virtual void SaveXml(util::xml::IXmlNode& root) const;
  virtual void ReadXml(const util::xml::IXmlNode& root);
  void Clear();
 protected:
  bool initialized_ = false;
 private:
  EventList event_list_;
};

}  // namespace workflow
