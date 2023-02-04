/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <wx/wx.h>
#include <workflow/iworkflow.h>
namespace workflow::gui {

class WorkflowDialog : public wxDialog {
 public:
  WorkflowDialog(wxWindow* parent, IWorkflow& workflow );

  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  IWorkflow& workflow_;
  wxString name_;
  wxString description_;
  wxString event_;
  wxDECLARE_EVENT_TABLE();
};


}  // namespace workflow::gui
