/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#pragma once
#include <wx/wx.h>
#include <wx/treelist.h>
#include <workflow/ievent.h>

namespace workflow::gui {

class TemplatePanel : public wxPanel {
 public:
  explicit TemplatePanel(wxWindow *parent);


  void Update() override;

  void OnUpdateNewTemplate(wxUpdateUIEvent& event);
  void OnUpdateSingleTemplate(wxUpdateUIEvent& event);
  void OnUpdateMultipleTemplate(wxUpdateUIEvent& event);
  void OnNewTemplate(wxCommandEvent& event);
  void OnEditTemplate(wxCommandEvent& event);
  void OnCopyTemplate(wxCommandEvent& event);
  void OnRenameTemplate(wxCommandEvent& event);
  void OnDeleteTemplate(wxCommandEvent& event);
  void OnRightClick(wxTreeListEvent& event);
  void OnDoubleClick(wxTreeListEvent& event);
 private:
  wxTreeListCtrl *list_ = nullptr;
  wxImageList image_list_;

  void RedrawTemplateList();
  [[nodiscard]] IRunner* GetSelectedTemplate();
  void SelectItem(const std::string& name);

  wxDECLARE_EVENT_TABLE();
};



}  // namespace workflow::gui
