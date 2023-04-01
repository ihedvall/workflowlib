/*
* Copyright 2023 Ingemar Hedvall
* SPDX-License-Identifier: MIT
*/

#include "workflowpanel.h"
#include "workflowexplorerid.h"
#include <wx/treelist.h>
#include <wx/withimages.h>

#include "workflowdialog.h"
#include "runnerdialog.h"
#include "workflowexplorer.h"
#include <utility>
#include <set>
#include <vector>

namespace {

constexpr int kWorkflowBmp = 0;
constexpr int kRunnerBmp = 1;
constexpr int kEventBmp = 2;
constexpr int kParameterBmp = 3;
constexpr int kDeviceBmp = 4;

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(WorkflowPanel, wxPanel)
    EVT_UPDATE_UI(kIdNewWorkflow, WorkflowPanel::OnUpdateNewWorkflow)
    EVT_MENU(kIdNewWorkflow, WorkflowPanel::OnNewWorkflow)
    EVT_UPDATE_UI(kIdEditWorkflow, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdEditWorkflow, WorkflowPanel::OnEditWorkflow)
    EVT_UPDATE_UI(kIdCopyWorkflow, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdCopyWorkflow, WorkflowPanel::OnCopyWorkflow)
    EVT_UPDATE_UI(kIdRenameWorkflow, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdRenameWorkflow, WorkflowPanel::OnRenameWorkflow)
    EVT_UPDATE_UI(kIdDeleteWorkflow, WorkflowPanel::OnUpdateMultipleWorkflow)
    EVT_MENU(kIdDeleteWorkflow, WorkflowPanel::OnDeleteWorkflow)
    EVT_UPDATE_UI(kIdUpWorkflow, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdUpWorkflow, WorkflowPanel::OnUpWorkflow)
    EVT_UPDATE_UI(kIdDownWorkflow, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdDownWorkflow, WorkflowPanel::OnDownWorkflow)
    EVT_UPDATE_UI(kIdNewRunner, WorkflowPanel::OnUpdateSingleWorkflow)
    EVT_MENU(kIdNewRunner, WorkflowPanel::OnNewRunner)
    EVT_UPDATE_UI(kIdEditRunner, WorkflowPanel::OnUpdateSingleRunner)
    EVT_MENU(kIdEditRunner, WorkflowPanel::OnEditRunner)
    EVT_UPDATE_UI(kIdCopyRunner, WorkflowPanel::OnUpdateSingleRunner)
    EVT_MENU(kIdCopyRunner, WorkflowPanel::OnCopyRunner)
    EVT_UPDATE_UI(kIdRenameRunner, WorkflowPanel::OnUpdateSingleRunner)
    EVT_MENU(kIdRenameRunner, WorkflowPanel::OnRenameRunner)
    EVT_UPDATE_UI(kIdDeleteRunner, WorkflowPanel::OnUpdateMultipleRunner)
    EVT_MENU(kIdDeleteRunner, WorkflowPanel::OnDeleteRunner)
    EVT_UPDATE_UI(kIdUpRunner, WorkflowPanel::OnUpdateSingleRunner)
    EVT_MENU(kIdUpRunner, WorkflowPanel::OnUpRunner)
    EVT_UPDATE_UI(kIdDownRunner, WorkflowPanel::OnUpdateSingleRunner)
    EVT_MENU(kIdDownRunner, WorkflowPanel::OnDownRunner)
    EVT_TREELIST_ITEM_CONTEXT_MENU(kIdWorkflowList, WorkflowPanel::OnRightClick)
    EVT_TREELIST_ITEM_ACTIVATED(kIdWorkflowList, WorkflowPanel::OnDoubleClick)
wxEND_EVENT_TABLE()

WorkflowPanel::WorkflowPanel(wxWindow *parent)
: wxPanel(parent),
  image_list_(16, 16, false, 5) {
  image_list_.Add(wxBitmap("TREE_LIST", wxBITMAP_TYPE_BMP_RESOURCE));
  list_ = new wxTreeListCtrl(this, kIdWorkflowList, wxDefaultPosition,
                             {800, 600}, wxTL_MULTIPLE);
  list_->SetImageList(&image_list_);

  list_->AppendColumn(L"Workflow/Task");
  list_->AppendColumn(L"Description");
  list_->AppendColumn(L"Event");
  list_->AppendColumn(L"Type");
  list_->AppendColumn(L"Arguments");
  list_->AppendColumn(L"Documentation");

  auto *main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(list_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetSizerAndFit(main_sizer);
  RedrawWorkflowList();
}

void WorkflowPanel::RedrawWorkflowList() {
  wxBusyCursor wait;
  if (list_ == nullptr) {
    return;
  }
  using wr_key = std::pair<std::string, std::string>; // Workflow - Runner
  std::set<std::string> expanded_list;
  std::set<wr_key> selected_list;

  const auto root_item = list_->GetRootItem();
  for ( wxTreeListItem item = list_->GetFirstItem();
                       item.IsOk();
                       item = list_->GetNextItem(item) ) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      // Workflow
      const auto workflow_name = list_->GetItemText(item).ToStdString();
      if (list_->IsExpanded(item)) {
        expanded_list.insert(workflow_name);
      }
      if (list_->IsSelected(item)) {
        selected_list.emplace(workflow_name, "");
      }
    } else {
      // Runner
      if (list_->IsSelected(item)) {
        const auto workflow_name = list_->GetItemText(parent_item).ToStdString();
        const auto runner_name = list_->GetItemText(item).ToStdString();
        selected_list.emplace(workflow_name, runner_name);
      }
    }
  }

  list_->DeleteAllItems();

  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto& workflow_list = server.Workflows();
  for (const auto& workflow : workflow_list) {
    if (!workflow) continue;
    const auto& workflow_name = workflow->Name();
    const auto workflow_item = list_->AppendItem(root_item,
                      wxString::FromUTF8(workflow_name),
                      kWorkflowBmp,kWorkflowBmp,
                      nullptr);
    list_->SetItemText(workflow_item, 1,
                       wxString::FromUTF8(workflow->Description()));
    list_->SetItemText(workflow_item, 2,
                       wxString::FromUTF8(workflow->StartEvent()));

    const auto& runner_list = workflow->Runners();
    for (const auto& runner : runner_list) {
      if (!runner) continue;
      const auto& runner_name = runner->Name();
      const auto runner_item = list_->AppendItem(workflow_item,
                                  wxString::FromUTF8(runner_name),
                                  kRunnerBmp,kRunnerBmp,
                                                   nullptr);
      list_->SetItemText(runner_item, 1,
                         wxString::FromUTF8(runner->Description()));
      list_->SetItemText(runner_item, 3,
                         wxString::FromUTF8(runner->TypeAsString()));
      list_->SetItemText(runner_item, 4,
                         wxString::FromUTF8(runner->Arguments()));
      list_->SetItemText(runner_item, 5,
                         wxString::FromUTF8(runner->Documentation()));

      const wr_key runner_key(workflow_name, runner_name);
      const auto itr1 = selected_list.find(runner_key);
      if (itr1 != selected_list.cend()) {
        list_->Select(runner_item);
        list_->Expand(workflow_item);
      }
    }
    const wr_key workflow_key(workflow_name, "");
    const auto itr2 = selected_list.find(workflow_key);
    if (itr2 != selected_list.cend()) {
      list_->Select(workflow_item);
    }
    if (expanded_list.find(workflow_name) != expanded_list.cend()) {
      list_->Expand(workflow_item);
    }
  }
}

void WorkflowPanel::Update() {
  wxWindow::Update();
  RedrawWorkflowList();
}

IWorkflow* WorkflowPanel::GetSelectedWorkflow() {
  if (list_ == nullptr) {
    return nullptr;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> workflow_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      workflow_list.insert(name);
    } else {
      const auto name = list_->GetItemText(parent_item).ToStdString();
      workflow_list.insert(name);
    }
  }
  if (workflow_list.size() != 1) {
    return nullptr;
  }
  const auto& name = *workflow_list.cbegin();

  auto& app = wxGetApp();
  auto& server = app.Server();
  return server.GetWorkflow(name);
}

IRunner* WorkflowPanel::GetSelectedRunner() {
  auto* workflow = GetSelectedWorkflow();
  if (workflow == nullptr || list_ == nullptr) {
    return nullptr;
  }

  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> runner_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item != root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      runner_list.insert(name);
    }
  }
  if (runner_list.size() != 1) {
    return nullptr;
  }
  const auto& name = *runner_list.cbegin();
  return workflow->GetRunner(name);
}

void WorkflowPanel::SelectItem(const std::string& workflow,
                                 const std::string& runner) {
  if (list_ == nullptr) {
    return ;
  }
  const auto select_workflow = runner.empty();
  const auto root_item = list_->GetRootItem();
  for ( wxTreeListItem item = list_->GetFirstItem();
       item.IsOk();
       item = list_->GetNextItem(item) ) {
    const auto parent_item = list_->GetItemParent(item);
    const auto name = list_->GetItemText(item);
    if (parent_item == root_item) {
      // Workflow
      if (name == workflow) {
        list_->Expand(item);
        if (select_workflow) {
          list_->Select(item);
        }
      } else {
        list_->Unselect(item);
      }
    } else if (parent_item != root_item) {
      // Runner
      if (name == runner) {
        list_->Select(item);
      } else {
        list_->Unselect(item);
      }
    }
  }
}

void WorkflowPanel::OnUpdateNewWorkflow(wxUpdateUIEvent &event) {
  event.Enable(list_ != nullptr);
}

void WorkflowPanel::OnUpdateSingleWorkflow(wxUpdateUIEvent &event) {
  event.Enable(GetSelectedWorkflow() != nullptr);
}

void WorkflowPanel::OnUpdateMultipleWorkflow(wxUpdateUIEvent &event) {
  if (list_ == nullptr) {
    event.Enable(false);
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> workflow_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      workflow_list.insert(name);
    } else {
      const auto name = list_->GetItemText(parent_item).ToStdString();
      workflow_list.insert(name);
    }
  }
  event.Enable(!workflow_list.empty());
}

void WorkflowPanel::OnUpdateSingleRunner(wxUpdateUIEvent &event) {
  event.Enable(GetSelectedRunner() != nullptr &&
               GetSelectedWorkflow() != nullptr);
}

void WorkflowPanel::OnUpdateMultipleRunner(wxUpdateUIEvent &event) {
  if (list_ == nullptr) {
    event.Enable(false);
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::vector<std::pair<std::string, std::string>> runner_list; // Task-Workflow

  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item != root_item) {
      const auto workflow_name = list_->GetItemText(item).ToStdString();
      const auto runner_name = list_->GetItemText(item).ToStdString();
      runner_list.emplace_back(runner_name, workflow_name);
    }
  }
  event.Enable(!runner_list.empty());
}

void WorkflowPanel::OnNewWorkflow(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();

  IWorkflow workflow(&server);
  WorkflowDialog dialog(this, workflow);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  const auto& name = workflow.Name();
  const auto exist = server.GetWorkflow(name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new workflow needs to be unique." << std::endl;
    msg << "Name: " << name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Workflow",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  server.AddWorkflow(workflow);
  RedrawWorkflowList();
  SelectItem(name, "");
}

void WorkflowPanel::OnEditWorkflow(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();
  if (workflow == nullptr) {
    return;
  }
  WorkflowDialog dialog(this, *workflow);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  RedrawWorkflowList();
}

void WorkflowPanel::OnCopyWorkflow(wxCommandEvent&) {
  auto* original = GetSelectedWorkflow();
  if (original == nullptr) {
    return;
  }
  IWorkflow workflow(*original);
  workflow.Name("");
  WorkflowDialog dialog(this, workflow);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto& new_name = workflow.Name();
  const auto exist = server.GetWorkflow(new_name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new workflow needs to be unique." << std::endl;
    msg << "Name: " << new_name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Workflow",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  server.AddWorkflow(workflow);
  RedrawWorkflowList();
  SelectItem(new_name, "");
}

void WorkflowPanel::OnRenameWorkflow(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();
  if (workflow == nullptr) {
    return;
  }
  std::ostringstream msg;
  msg << "Rename the workflow: " << workflow->Name();

  const auto new_name = wxGetTextFromUser(
      wxString::FromUTF8(msg.str()),
      wxString("Rename Workflow"),
      wxString::FromUTF8(workflow->Name()),
      this);
  if (new_name.IsEmpty()) {
    return;
  }
  auto& app = wxGetApp();
  auto& server = app.Server();
  const auto* exist = server.GetWorkflow(new_name.ToStdString());
  if (exist != nullptr && exist != workflow) {
    std::ostringstream err;
    err << "The name of the workflow needs to be unique." << std::endl;
    err << "Name: " << new_name << std::endl;
    wxMessageBox(err.str(), "Fail to Rename Workflow",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  workflow->Name(new_name.ToStdString());
  RedrawWorkflowList();
  SelectItem(new_name.ToStdString(), "");
}

void WorkflowPanel::OnDeleteWorkflow(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::set<std::string> workflow_list;
  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item == root_item) {
      const auto name = list_->GetItemText(item).ToStdString();
      workflow_list.insert(name);
    } else {
      const auto name = list_->GetItemText(parent_item).ToStdString();
      workflow_list.insert(name);
    }
  }
  if (workflow_list.empty()) {
    return;
  }
  std::ostringstream msg;
  msg << "Do you want to delete the following workflows?" << std::endl;
  for (const auto& workflow_name : workflow_list) {
    msg << workflow_name << std::endl;
  }
  const auto ret = wxMessageBox(wxString::FromUTF8(msg.str()),
         wxString("Delete Workflow"),
         wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT | wxCENTRE | wxICON_QUESTION,
         this);
  if (ret != wxYES) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  for (const auto& item_name : workflow_list) {
    const auto* workflow = server.GetWorkflow(item_name);
    server.DeleteWorkflow(workflow);
  }

  RedrawWorkflowList();
}

void WorkflowPanel::OnNewRunner(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();

  if (workflow == nullptr) {
    return;
  }

  IRunner runner;
  RunnerDialog dialog(this, runner);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  const auto& name = runner.Name();
  const auto exist = workflow->GetRunner(name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new task in the workflow, needs to be unique."
        << std::endl;
    msg << "Name: " << name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Task",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  workflow->AddRunner(runner);
  RedrawWorkflowList();
  SelectItem(workflow->Name(), name);
}

void WorkflowPanel::OnEditRunner(wxCommandEvent&) {
  auto* runner = GetSelectedRunner();
  if (runner == nullptr) {
    return;
  }
  RunnerDialog dialog(this, *runner);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }
  RedrawWorkflowList();
}

void WorkflowPanel::OnCopyRunner(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();
  auto* original = GetSelectedRunner();
  if (original == nullptr || workflow == nullptr) {
    return;
  }
  IRunner runner(*original);
  runner.Name("");
  RunnerDialog dialog(this, runner);
  const auto ret = dialog.ShowModal();
  if (ret != wxID_OK) {
    return;
  }

  const auto& new_name = runner.Name();
  const auto exist = workflow->GetRunner(new_name) != nullptr;
  if (exist) {
    std::ostringstream msg;
    msg << "The name of the new task needs to be unique within the workflow."
        << std::endl;
    msg << "Name: " << new_name << std::endl;
    wxMessageBox(msg.str(), "Fail to Add Runner",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  workflow->AddRunner(runner);
  RedrawWorkflowList();
  SelectItem(workflow->Name(), new_name);
}

void WorkflowPanel::OnRenameRunner(wxCommandEvent&) {
  const auto* workflow = GetSelectedWorkflow();
  auto* runner = GetSelectedRunner();
  if (runner == nullptr || workflow == nullptr) {
    return;
  }
  std::ostringstream msg;
  msg << "Rename the task: " << runner->Name();

  const auto new_name = wxGetTextFromUser(
      wxString::FromUTF8(msg.str()),
      wxString("Rename Task"),
      wxString::FromUTF8(runner->Name()),
      this);
  if (new_name.IsEmpty()) {
    return;
  }
  const auto* exist = workflow->GetRunner(new_name.ToStdString());
  if (exist != nullptr && exist != runner) {
    std::ostringstream err;
    err << "The name of the task needs to be unique witin the workflow."
        << std::endl;
    err << "Name: " << new_name << std::endl;
    wxMessageBox(err.str(), "Fail to Rename Task",
                 wxCENTRE | wxICON_ERROR, this);
    return;
  }
  runner->Name(new_name.ToStdString());
  RedrawWorkflowList();
  SelectItem(workflow->Name(), runner->Name());
}

void WorkflowPanel::OnDeleteRunner(wxCommandEvent&) {
  if (list_ == nullptr) {
    return;
  }
  const auto root_item = list_->GetRootItem();
  wxTreeListItems selection_list;
  list_->GetSelections(selection_list);
  std::vector<std::pair<std::string, std::string>> runner_list; // Task-Workflow

  for (const auto& item : selection_list) {
    const auto parent_item = list_->GetItemParent(item);
    if (parent_item != root_item) {
      const auto workflow_name = list_->GetItemText(parent_item).ToStdString();
      const auto runner_name = list_->GetItemText(item).ToStdString();
      runner_list.emplace_back(workflow_name,runner_name);
    }
  }
  if (runner_list.empty()) {
    return;
  }

  std::ostringstream msg;
  msg << "Do you want to delete the following tasks?" << std::endl;
  for (const auto& item : runner_list) {
    msg << item.first << ":" << item.second << std::endl;
  }
  const auto ret = wxMessageBox(wxString::FromUTF8(msg.str()),
                                wxString("Delete Task"),
                                wxYES_NO | wxCANCEL | wxCANCEL_DEFAULT
                                    | wxCENTRE | wxICON_QUESTION,
                                this);
  if (ret != wxYES) {
    return;
  }

  auto& app = wxGetApp();
  auto& server = app.Server();
  for (const auto& item1 : runner_list) {
    auto* workflow = server.GetWorkflow(item1.first);
    if (workflow == nullptr) {
      continue;
    }
    const auto* runner = workflow->GetRunner(item1.second);
    if (runner != nullptr) {
      workflow->DeleteRunner(runner);
    }
  }

  RedrawWorkflowList();
}

void WorkflowPanel::OnRightClick(wxTreeListEvent&) {
  wxMenu menu;

  // Workflow
  auto *menu_workflow = new wxMenu;
  menu_workflow->Append(kIdUpWorkflow, wxGetStockLabel(wxID_UP));
  menu_workflow->Append(kIdDownWorkflow, wxGetStockLabel(wxID_DOWN));
  menu_workflow->AppendSeparator();
  menu_workflow->Append(kIdNewWorkflow, wxGetStockLabel(wxID_NEW));
  menu_workflow->Append(kIdEditWorkflow, wxGetStockLabel(wxID_EDIT));
  menu_workflow->Append(kIdCopyWorkflow, wxGetStockLabel(wxID_COPY));
  menu_workflow->Append(kIdRenameWorkflow, wxString("Rename"));
  menu_workflow->Append(kIdDeleteWorkflow, wxGetStockLabel(wxID_DELETE));

  auto *menu_runner = new wxMenu;
  menu_runner->Append(kIdUpRunner, wxGetStockLabel(wxID_UP));
  menu_runner->Append(kIdDownRunner, wxGetStockLabel(wxID_DOWN));
  menu_runner->AppendSeparator();
  menu_runner->Append(kIdNewRunner, wxGetStockLabel(wxID_NEW));
  menu_runner->Append(kIdEditRunner, wxGetStockLabel(wxID_EDIT));
  menu_runner->Append(kIdCopyRunner, wxGetStockLabel(wxID_COPY));
  menu_runner->Append(kIdRenameRunner, wxString("Rename"));
  menu_runner->Append(kIdDeleteRunner, wxGetStockLabel(wxID_DELETE));

  menu.Append(kIdOpenLogFile, L"Open Log File");
  menu.AppendSeparator();
  menu.AppendSubMenu(menu_workflow, L"Workflow");
  menu.AppendSubMenu(menu_runner, L"Task");
  PopupMenu(&menu);
}

void WorkflowPanel::OnUpWorkflow(wxCommandEvent&) {
  const auto* workflow = GetSelectedWorkflow();
  if (workflow == nullptr) {
    return;
  }
  auto& app = wxGetApp();
  auto& server = app.Server();
  server.MoveUp(workflow);
  RedrawWorkflowList();
}

void WorkflowPanel::OnDownWorkflow(wxCommandEvent&) {
  const auto* workflow = GetSelectedWorkflow();
  if (workflow == nullptr) {
    return;
  }
  auto& app = wxGetApp();
  auto& server = app.Server();
  server.MoveDown(workflow);
  RedrawWorkflowList();
}

void WorkflowPanel::OnUpRunner(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();
  const auto* runner = GetSelectedRunner();
  if (workflow == nullptr || runner == nullptr) {
    return;
  }
  workflow->MoveUp(runner);
  RedrawWorkflowList();
}

void WorkflowPanel::OnDownRunner(wxCommandEvent&) {
  auto* workflow = GetSelectedWorkflow();
  const auto* runner = GetSelectedRunner();
  if (workflow == nullptr || runner == nullptr) {
    return;
  }
  workflow->MoveDown(runner);
  RedrawWorkflowList();
}

void WorkflowPanel::OnDoubleClick(wxTreeListEvent& event) {
    const auto* runner = GetSelectedRunner();
    if (runner != nullptr) {
      OnEditRunner(event);
      return;
    }
    const auto* workflow = GetSelectedWorkflow();
    if (workflow != nullptr) {
      OnEditWorkflow(event);
    }
}

}  // namespace workflow::gui