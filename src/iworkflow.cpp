/*
* Copyright 2022 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "workflow/iworkflow.h"

namespace workflow {
void IWorkflow::OnStart() {
  start_ = true;
  start_condition_.notify_all();
}

}  // namespace workflow