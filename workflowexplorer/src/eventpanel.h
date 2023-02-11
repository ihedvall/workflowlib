/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>
#include <workflow/ievent.h>

namespace workflow::gui {

class EventPanel : public wxPanel {
 public:
  explicit EventPanel(wxWindow *parent);


  void Update() override;

  void OnUpdateNewEvent(wxUpdateUIEvent& event);
  void OnUpdateSingleEvent(wxUpdateUIEvent& event);
  void OnUpdateMultipleEvent(wxUpdateUIEvent& event);
  void OnNewEvent(wxCommandEvent& event);
  void OnEditEvent(wxCommandEvent& event);
  void OnCopyEvent(wxCommandEvent& event);
  void OnRenameEvent(wxCommandEvent& event);
  void OnDeleteEvent(wxCommandEvent& event);
  void OnRightClick(wxTreeListEvent& event);
  void OnDoubleClick(wxTreeListEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  void RedrawEventList();
  [[nodiscard]] IEvent* GetSelectedEvent();
  void SelectItem(const std::string& event);

  wxDECLARE_EVENT_TABLE();
};


}  // namespace workflow::gui
