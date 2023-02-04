/*
 * Copyright 2023 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <wx/wx.h>
#include <wx/treelist.h>

namespace workflow::gui {

class WorkflowPanel: public wxPanel  {
 public:
  explicit WorkflowPanel(wxWindow *parent);
  void RedrawWorkflowList();

  void Update() override;

  void OnUpdateNewWorkflow(wxUpdateUIEvent& event);
  void OnNewWorkflow(wxCommandEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  wxDECLARE_EVENT_TABLE();
};


}  // namespace workflow::gui
