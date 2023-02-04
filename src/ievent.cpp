/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/ievent.h"
#include <util/stringutil.h>

namespace workflow {

IEvent::IEvent() {
  DataType(ParameterDataType::BooleanType);
}

IEvent::~IEvent() {
  runner_.reset();
}
IEvent::IEvent(const IEvent& event)
: IParameter((const IParameter&) event),
  type_(event.type_)
{
  if (event.runner_) {
    runner_ = std::make_unique<IRunner>(*event.runner_);
  }
}

bool IEvent::operator==(const IEvent& event) const {
  if (type_ != event.type_) return false;
  return *static_cast<const IParameter*>(this) ==
            static_cast<const IParameter&>(event);
}

void IEvent::AddRunner(std::unique_ptr<IRunner>& runner) {
  runner_ = std::move(runner);
}

void IEvent::EventTypeAsString(const std::string& type) {
  IEvent temp;
  for (auto index = static_cast<int>(EventType::Internal);
       index <= static_cast<int>(EventType::Parameter);
       ++index) {
    temp.Type(static_cast<EventType>(index));
    const auto type_string = temp.EventTypeAsString();
    if (util::string::IEquals(type, type_string)) {
      Type(temp.Type());
      return;
    }
  }
}

std::string IEvent::EventTypeAsString() const {
  switch (Type()) {
    case EventType::Internal:
      return "Internal";

    case EventType::External:
      return "External";

    case EventType::Periodic:
      return "Periodic";

    case EventType::Parameter:
      return "Parameter";

    default:
      break;
  }
  return {};
}


}  // namespace workflow