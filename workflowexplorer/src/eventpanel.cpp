/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "eventpanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

#include "eventdialog.h"
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

wxBEGIN_EVENT_TABLE(EventPanel, wxPanel)
    EVT_UPDATE_UI(kIdNewEvent, EventPanel::OnUpdateNewEvent)
    EVT_MENU(kIdNewEvent, EventPanel::OnNewEvent)
    EVT_UPDATE_UI(kIdEditEvent, EventPanel::OnUpdateSingleEvent)
    EVT_MENU(kIdEditEvent, EventPanel::OnEditEvent)
    EVT_UPDATE_UI(kIdCopyEvent, EventPanel::OnUpdateSingleEvent)
    EVT_MENU(kIdCopyEvent, EventPanel::OnCopyEvent)
    EVT_UPDATE_UI(kIdRenameEvent, EventPanel::OnUpdateSingleEvent)
    EVT_MENU(kIdRenameEvent, EventPanel::OnRenameEvent)
    EVT_UPDATE_UI(kIdDeleteEvent, EventPanel::OnUpdateMultipleEvent)
    EVT_MENU(kIdDeleteEvent, EventPanel::OnDeleteEvent)
    EVT_TREELIST_ITEM_CONTEXT_MENU(kIdEventList, EventPanel::OnRightClick)
    EVT_TREELIST_ITEM_ACTIVATED(kIdEventList, EventPanel::OnDoubleClick)
wxEND_EVENT_TABLE()

EventPanel::EventPanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16, 16, false, 5) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_ = new wxTreeListCtrl(this, kIdEventList, wxDefaultPosition,
                             {800, 600}, wxTL_MULTIPLE);
  list_->SetImageList(&image_list_);

  list_->AppendColumn(L"Name");
  list_->AppendColumn(L"Description");
  list_->AppendColumn(L"Type");

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);
}

void EventPanel::RedrawEventList() {
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
  const auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return;
  }
  const auto& event_list = engine->Events();
  for (const auto& itr : event_list) {
    const auto* event = itr.second.get();
    if (event == nullptr) {
      continue;
    }
    const auto& name = event->Name();
    const auto item = list_->AppendItem(root_item,
                                        wxString::FromUTF8(name),
                                        kEventBmp,kEventBmp,
                                                 nullptr);
    list_->SetItemText(item, 1,
                       wxString::FromUTF8(event->Description()));
    list_->SetItemText(item, 2,
                       wxString::FromUTF8(event->EventTypeAsString()));
    const auto selected_itr = selected_list.find(name);
    if (selected_itr != selected_list.cend()) {
      list_->Select(item);
    }
  }
}

void EventPanel::Update() {
  wxWindow::Update();
  RedrawEventList();
}

IEvent* EventPanel::GetSelectedEvent() {
  if (list_ == nullptr) {
    return nullptr;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> event_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      event_list.insert(name);
    }
  }
  if (event_list.size() != 1) {
    return nullptr;
  }
  const auto& name = *event_list.cbegin();

  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return nullptr;
  }
  return engine->GetEvent(name);
}

void EventPanel::SelectItem(const std::string& event) {
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
      if (event == name) {
        list_->Select(item);
      } else {
        list_->Unselect(item);
      }
    }
  }
}

void EventPanel::OnUpdateNewEvent(wxUpdateUIEvent &event) {
  event.Enable(list_ != nullptr);
}

void EventPanel::OnUpdateSingleEvent(wxUpdateUIEvent &event) {
  event.Enable(GetSelectedEvent() != nullptr);
}

void EventPanel::OnUpdateMultipleEvent(wxUpdateUIEvent &event) {
  if (list_ == nullptr) {
    event.Enable(false);
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  event.Enable(!selection_list.empty());
}

void EventPanel::OnNewEvent(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }
  IEvent new_event;
  EventDialog dialog(this, new_event);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  const auto& name = new_event.Name();
  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return;
  }
  const auto exist = engine->GetEvent(name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new event needs to be unique." << std::endl;
    msg << "Name: " << name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Event",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }

  engine->AddEvent(new_event);
  RedrawEventList();
  SelectItem(name);
}

void EventPanel::OnEditEvent(wxCommandEvent&) {
  auto* selected = GetSelectedEvent();
  if ( selected == nullptr) {
    return;
  }
  EventDialog dialog(this, *selected);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  RedrawEventList();
}

void EventPanel::OnCopyEvent(wxCommandEvent&) {
  auto* original = GetSelectedEvent();
  if (original == nullptr) {
    return;
  }
  IEvent new_event(*original);
  new_event.Name("");
  EventDialog dialog(this, new_event);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return;
  }
  const auto& new_name = new_event.Name();
  const auto exist = engine->GetEvent(new_name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new event needs to be unique." << std::endl;
    msg << "Name: " << new_name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Event",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  engine->AddEvent(new_event);
  RedrawEventList();
  SelectItem(new_name);
}

void EventPanel::OnRenameEvent(wxCommandEvent&) {
  const auto* selected = GetSelectedEvent();
  if (selected == nullptr) {
    return;
  }
  std::ostringstream msg;
  msg << "Rename the event: " << selected->Name();

  const auto new_name = wxGetTextFromUser(
      wxString::FromUTF8(msg.str()),
      wxString("Rename Event"),
      wxString::FromUTF8(selected->Name()),
      this);
  if (new_name.IsEmpty()) {
    return;
  }
  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return;
  }
  const auto* exist = engine->GetEvent(new_name.ToStdString());
  if (exist != nullptr && exist != selected) {
    std::ostringstream err;
    err << "The name of the event needs to be unique." << std::endl;
    err << "Name: " << new_name << std::endl;
    wxMessageBox(err.str(), "Fail to Rename Event",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }

  IEvent new_event(*selected);

  new_event.Name(new_name.ToStdString());
  engine->DeleteEvent(selected);
  engine->AddEvent(new_event);
  RedrawEventList();
  SelectItem(new_name.ToStdString());
}

void EventPanel::OnDeleteEvent(wxCommandEvent&) {
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
  msg << "Do you want to delete the following events?" << std::endl;
  for (const auto& event_name : del_list) {
    msg << event_name << std::endl;
  }
  const auto ret = wxMessageBox(wxString::FromUTF8(msg.str()),
                                wxString("Delete Events"),
                                wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTRE | wxICON_QUESTION,
                                this);
  if (ret != wxYES) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  auto* engine = server.GetEventEngine();
  if (engine == nullptr) {
    return;
  }
  for (const auto& item_name : del_list) {
    const auto* del = engine->GetEvent(item_name);
    engine->DeleteEvent(del);
  }

  RedrawEventList();
}

void EventPanel::OnRightClick(wxTreeListEvent&) {
  wxMenu menu;
  menu.Append(kIdOpenLogFile, L"Open Log File");
  menu.AppendSeparator();
  menu.Append(kIdNewEvent, wxGetStockLabel(wxID_NEW));
  menu.Append(kIdEditEvent, wxGetStockLabel(wxID_EDIT));
  menu.Append(kIdCopyEvent, wxGetStockLabel(wxID_COPY));
  menu.Append(kIdRenameEvent, wxString("Rename"));
  menu.Append(kIdDeleteEvent, wxGetStockLabel(wxID_DELETE));
  PopupMenu(&menu);
}

void EventPanel::OnDoubleClick(wxTreeListEvent& event) {
  const auto* selected = GetSelectedEvent();
  if (selected != nullptr) {
    OnEditEvent(event);
  }
}

}  // namespace workflow::gui