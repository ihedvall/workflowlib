/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <workflow/ievent.h>

namespace workflow::gui {

class EventDialog : public wxDialog {
 public:
  EventDialog(wxWindow* parent, IEvent& event );

  bool TransferDataToWindow() override;
  bool TransferDataFromWindow() override;
 private:
  IEvent& event_;
  wxString name_;
  wxString description_;
  wxString type_;
  uint64_t period_ = 1000;

  wxDECLARE_EVENT_TABLE();
};

}  // namespace workflow::gui
