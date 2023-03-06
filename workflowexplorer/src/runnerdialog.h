/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <workflow/irunner.h>
namespace workflow::gui {

class RunnerDialog : public wxDialog {
 public:
  RunnerDialog(wxWindow* parent, IRunner& runner );

  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  IRunner& runner_;
  wxString name_;
  wxString description_;
  wxString documentation_;

  wxString type_;
  //double period_ = 0.0;
  wxString arguments_;

  void OnNameChange(wxCommandEvent& event);
  wxDECLARE_EVENT_TABLE();
};

}  // namespace workflow::gui
