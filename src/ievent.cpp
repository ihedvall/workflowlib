/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/ievent.h"

namespace workflow {
IEvent::IEvent() {
  DataType(ParameterDataType::BooleanType);
}

IEvent::~IEvent() {
  runner_.reset();
}

void IEvent::AddRunner(std::unique_ptr<IRunner>& runner) {
  runner_ = std::move(runner);
}

}  // namespace workflow