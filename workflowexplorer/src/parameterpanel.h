/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>

namespace workflow::gui {

class ParameterPanel : public wxPanel {
 public:
  explicit ParameterPanel(wxWindow *parent);
  void RedrawParameterList();

  void Update() override;

  void OnUpdateNewParameter(wxUpdateUIEvent& event);
  void OnNewParameter(wxCommandEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  wxDECLARE_EVENT_TABLE();
};

} // end namespace
