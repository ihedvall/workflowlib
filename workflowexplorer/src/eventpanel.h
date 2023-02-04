/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>

namespace workflow::gui {

class EventPanel : public wxPanel {
 public:
  explicit EventPanel(wxWindow *parent);
  void RedrawEventList();

  void Update() override;

  void OnUpdateNewEvent(wxUpdateUIEvent& event);
  void OnNewEvent(wxCommandEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  wxDECLARE_EVENT_TABLE();
};


}  // namespace workflow::gui
