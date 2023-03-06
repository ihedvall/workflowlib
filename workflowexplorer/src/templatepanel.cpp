/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
 */

#include "templatepanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

#include "runnerdialog.h"
#include "workflowexplorer.h"
#include <set>

namespace {

constexpr int kWorkflowBmp = 0;
constexpr int kRunnerBmp = 1;
constexpr int kEventBmp = 2;
constexpr int kParameterBmp = 3;
constexpr int kDeviceBmp = 4;

}

namespace workflow::gui {
  wxBEGIN_EVENT_TABLE(TemplatePanel, wxPanel)
  EVT_UPDATE_UI(kIdNewTemplate, TemplatePanel::OnUpdateNewTemplate)
  EVT_MENU(kIdNewTemplate, TemplatePanel::OnNewTemplate)
  EVT_UPDATE_UI(kIdEditTemplate, TemplatePanel::OnUpdateSingleTemplate)
  EVT_MENU(kIdEditTemplate, TemplatePanel::OnEditTemplate)
  EVT_UPDATE_UI(kIdCopyTemplate, TemplatePanel::OnUpdateSingleTemplate)
  EVT_MENU(kIdCopyTemplate, TemplatePanel::OnCopyTemplate)
  EVT_UPDATE_UI(kIdRenameTemplate, TemplatePanel::OnUpdateSingleTemplate)
  EVT_MENU(kIdRenameTemplate, TemplatePanel::OnRenameTemplate)
  EVT_UPDATE_UI(kIdDeleteTemplate, TemplatePanel::OnUpdateMultipleTemplate)
  EVT_MENU(kIdDeleteTemplate, TemplatePanel::OnDeleteTemplate)
  EVT_TREELIST_ITEM_CONTEXT_MENU(kIdTemplateList, TemplatePanel::OnRightClick)
  EVT_TREELIST_ITEM_ACTIVATED(kIdTemplateList, TemplatePanel::OnDoubleClick)
wxEND_EVENT_TABLE()

TemplatePanel::TemplatePanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16, 16, false, 5) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_ = new wxTreeListCtrl(this, kIdTemplateList, wxDefaultPosition,
                             {800, 600}, wxTL_MULTIPLE);
  list_->SetImageList(&image_list_);

  list_->AppendColumn(L"Name");
  list_->AppendColumn(L"Description");
  list_->AppendColumn(L"Type");
  list_->AppendColumn(L"Arguments");

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);
}

void TemplatePanel::RedrawTemplateList() {
  wxBusyCursor wait;
  if (list_ == nullptr) {
    return;
  }
  std::set<std::string> selected_list;
  const auto root_item = list_->GetRootItem();
  for ( wxTreeListItem item = list_->GetFirstItem();
       item.IsOk();
       item = list_->GetNextItem(item) ) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item != root_item) {
      continue;
    }
    if (list_->IsSelected(item)) {
      const auto name = list_->GetItemText(item).ToStdString();
      selected_list.emplace(name);
    }
  }

  list_->DeleteAllItems();

  const auto& app = wxGetApp();
  const auto& server = app.Server();
  const auto& template_list = server.Templates();
  for (const auto& itr : template_list) {
    const auto* runner = itr.second.get();
    if (runner == nullptr) continue;
    const auto& name = runner->Name();
    const auto item = list_->AppendItem(root_item,
                                        wxString::FromUTF8(name),
                                        kRunnerBmp,kRunnerBmp,
                                        nullptr);
    list_->SetItemText(item, 1, wxString::FromUTF8(runner->Description()));
    list_->SetItemText(item, 2, wxString::FromUTF8(runner->TypeAsString()));
    list_->SetItemText(item, 3, wxString::FromUTF8(runner->Arguments()));
    const auto selected_itr = selected_list.find(name);
    if (selected_itr != selected_list.cend()) {
      list_->Select(item);
    }
  }
}

void TemplatePanel::Update() {
  wxWindow::Update();
  RedrawTemplateList();
}

IRunner* TemplatePanel::GetSelectedTemplate() {
  if (list_ == nullptr) {
    return nullptr;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> name_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      name_list.insert(name);
    }
  }
  if (name_list.size() != 1) {
    return nullptr;
  }
  const auto& name = *name_list.cbegin();

  auto& app = wxGetApp();
  auto& server = app.Server();
  return server.GetTemplate(name);
}

void TemplatePanel::SelectItem(const std::string& template_name) {
  if (list_ == nullptr) {
    return ;
  }
  const auto root_item = list_->GetRootItem();
  for ( wxTreeListItem item = list_->GetFirstItem();
       item.IsOk();
       item = list_->GetNextItem(item) ) {
    const auto parent_item = list_->GetItemParent(item);
    const auto name = list_->GetItemText(item).ToStdString();
    if (parent_item == root_item) {
      if (template_name == name) {
        list_->Select(item);
      } else {
        list_->Unselect(item);
      }
    }
  }
}

void TemplatePanel::OnUpdateNewTemplate(wxUpdateUIEvent &event) {
  event.Enable(list_ != nullptr);
}

void TemplatePanel::OnUpdateSingleTemplate(wxUpdateUIEvent &event) {
  event.Enable(GetSelectedTemplate() != nullptr);
}

void TemplatePanel::OnUpdateMultipleTemplate(wxUpdateUIEvent &event) {
  if (list_ == nullptr) {
    event.Enable(false);
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  event.Enable(!selection_list.empty());
}

void TemplatePanel::OnNewTemplate(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }
  IRunner new_template;
  RunnerDialog dialog(this, new_template);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  const auto& name = new_template.Name();
  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto exist = server.GetTemplate(name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new template task needs to be unique." << std::endl;
    msg << "Name: " << name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Template Task",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }

  server.AddTemplate(new_template);
  RedrawTemplateList();
  SelectItem(name);
}

void TemplatePanel::OnEditTemplate(wxCommandEvent&) {
  auto* selected = GetSelectedTemplate();
  if ( selected == nullptr) {
    return;
  }
  RunnerDialog dialog(this, *selected);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  RedrawTemplateList();
}

void TemplatePanel::OnCopyTemplate(wxCommandEvent&) {
  auto* original = GetSelectedTemplate();
  if (original == nullptr) {
    return;
  }
  IRunner new_template(*original);
  new_template.Name("");
  RunnerDialog dialog(this, new_template);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto& new_name = new_template.Name();
  const auto exist = server.GetTemplate(new_name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new template task needs to be unique." << std::endl;
    msg << "Name: " << new_name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Template Task",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  server.AddTemplate(new_template);
  RedrawTemplateList();
  SelectItem(new_name);
}

void TemplatePanel::OnRenameTemplate(wxCommandEvent&) {
  const auto* selected = GetSelectedTemplate();
  if (selected == nullptr) {
    return;
  }
  std::ostringstream msg;
  msg << "Rename the template task: " << selected->Name();

  const auto new_name = wxGetTextFromUser(
      wxString::FromUTF8(msg.str()),
      wxString("Rename Template Tas"),
      wxString::FromUTF8(selected->Name()),
      this);
  if (new_name.IsEmpty()) {
    return;
  }
  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto* exist = server.GetTemplate(new_name.ToStdString());
  if (exist != nullptr && exist != selected) {
    std::ostringstream err;
    err << "The name of the template task needs to be unique." << std::endl;
    err << "Name: " << new_name << std::endl;
    wxMessageBox(err.str(), "Fail to Rename Template Task",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }

  IRunner new_template(*selected);

  new_template.Name(new_name.ToStdString());
  server.DeleteTemplate(selected);
  server.AddTemplate(new_template);
  RedrawTemplateList();
  SelectItem(new_name.ToStdString());
}

void TemplatePanel::OnDeleteTemplate(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> del_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      del_list.insert(name);
    }
  }
  if (del_list.empty()) {
    return;
  }
  std::ostringstream msg;
  msg << "Do you want to delete the following template tasks?" << std::endl;
  for (const auto& template_name : del_list) {
    msg << template_name << std::endl;
  }
  const auto ret = wxMessageBox(wxString::FromUTF8(msg.str()),
                                wxString("Delete Template Task"),
                                wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTRE | wxICON_QUESTION,
                                this);
  if (ret != wxYES) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  for (const auto& item_name : del_list) {
    const auto* del = server.GetTemplate(item_name);
    server.DeleteTemplate(del);
  }

  RedrawTemplateList();
}

void TemplatePanel::OnRightClick(wxTreeListEvent&) {
  wxMenu menu;
  menu.Append(kIdOpenLogFile, L"Open Log File");
  menu.AppendSeparator();
  menu.Append(kIdNewTemplate, wxGetStockLabel(wxID_NEW));
  menu.Append(kIdEditTemplate, wxGetStockLabel(wxID_EDIT));
  menu.Append(kIdCopyTemplate, wxGetStockLabel(wxID_COPY));
  menu.Append(kIdRenameTemplate, wxString("Rename"));
  menu.Append(kIdDeleteTemplate, wxGetStockLabel(wxID_DELETE));
  PopupMenu(&menu);
}

void TemplatePanel::OnDoubleClick(wxTreeListEvent& event) {
  const auto* selected = GetSelectedTemplate();
  if (selected != nullptr) {
    OnEditTemplate(event);
  }
}

}  // namespace workflow::gui