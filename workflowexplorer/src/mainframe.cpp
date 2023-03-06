/*
 * Copyright 2022 Ingemar Hedvall
 * SPDX-License-Identifier: MIT
 */
#include "mainframe.h"

#include <wx/aboutdlg.h>
#include <wx/choicdlg.h>
#include <wx/config.h>

#include <string>

#include "workflowpanel.h"
#include "eventpanel.h"
#include "parameterpanel.h"
#include "templatepanel.h"
#include "workflowexplorer.h"
#include "workflowexplorerid.h"

namespace {

constexpr int kPageWorkflow = 0;
constexpr int kPageEvent = 1;
constexpr int kPageDevice = 2;
constexpr int kPageParameter = 3;
constexpr int kPageTemplate = 4;

constexpr int kBmpWorkflowHot = 0;
constexpr int kBmpWorkflowDisabled = 1;
constexpr int kBmpEventHot = 2;
constexpr int kBmpEventDisabled = 3;
constexpr int kBmpDeviceHot = 4;
constexpr int kBmpDeviceDisabled = 5;
constexpr int kBmpParameterHot = 6;
constexpr int kBmpParameterDisabled = 7;
constexpr int kBmpTemplateHot = 8;
constexpr int kBmpTemplateDisabled = 9;

}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
  EVT_MENU(wxID_EXIT, MainFrame::OnExit)
  EVT_CLOSE(MainFrame::OnClose)
  EVT_NOTEBOOK_PAGE_CHANGED(kIdNotebook, MainFrame::OnPageChange)

  EVT_UPDATE_UI(kIdNewWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdNewWorkflow, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdEditWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdEditWorkflow, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdCopyWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdCopyWorkflow, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdRenameWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdRenameWorkflow, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdDeleteWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdDeleteWorkflow, MainFrame::OnWorkflow)

  EVT_UPDATE_UI(kIdNewRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdNewRunner, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdEditRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdEditRunner, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdCopyRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdCopyRunner, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdRenameRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdRenameRunner, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdDeleteRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdDeleteRunner, MainFrame::OnWorkflow)

  EVT_UPDATE_UI(kIdUpWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdUpWorkflow, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdDownWorkflow, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdDownWorkflow, MainFrame::OnWorkflow)

  EVT_UPDATE_UI(kIdUpRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdUpRunner, MainFrame::OnWorkflow)
  EVT_UPDATE_UI(kIdDownRunner, MainFrame::OnUpdateWorkflow)
  EVT_MENU(kIdDownRunner, MainFrame::OnWorkflow)

  EVT_UPDATE_UI(kIdNewEvent, MainFrame::OnUpdateEvent)
  EVT_MENU(kIdNewEvent, MainFrame::OnEvent)
  EVT_UPDATE_UI(kIdEditEvent, MainFrame::OnUpdateEvent)
  EVT_MENU(kIdEditEvent, MainFrame::OnEvent)
  EVT_UPDATE_UI(kIdCopyEvent, MainFrame::OnUpdateEvent)
  EVT_MENU(kIdCopyEvent, MainFrame::OnEvent)
  EVT_UPDATE_UI(kIdRenameEvent, MainFrame::OnUpdateEvent)
  EVT_MENU(kIdRenameEvent, MainFrame::OnEvent)
  EVT_UPDATE_UI(kIdDeleteEvent, MainFrame::OnUpdateEvent)
  EVT_MENU(kIdDeleteEvent, MainFrame::OnEvent)

  EVT_UPDATE_UI(kIdNewTemplate, MainFrame::OnUpdateTemplate)
  EVT_MENU(kIdNewTemplate, MainFrame::OnTemplate)
  EVT_UPDATE_UI(kIdEditTemplate, MainFrame::OnUpdateTemplate)
  EVT_MENU(kIdEditTemplate, MainFrame::OnTemplate)
  EVT_UPDATE_UI(kIdCopyTemplate, MainFrame::OnUpdateTemplate)
  EVT_MENU(kIdCopyTemplate, MainFrame::OnTemplate)
  EVT_UPDATE_UI(kIdRenameTemplate, MainFrame::OnUpdateTemplate)
  EVT_MENU(kIdRenameTemplate, MainFrame::OnTemplate)
  EVT_UPDATE_UI(kIdDeleteTemplate, MainFrame::OnUpdateTemplate)
  EVT_MENU(kIdDeleteTemplate, MainFrame::OnTemplate)

  EVT_UPDATE_UI(kIdNewParameter, MainFrame::OnUpdateParameter)
  EVT_MENU(kIdNewParameter, MainFrame::OnParameter)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& start_pos, const wxSize& start_size, bool maximized)
    : wxFrame(nullptr, wxID_ANY, title, start_pos, start_size),
    image_list_(32,32,false,10) {
  wxTopLevelWindowMSW::Maximize(maximized);
  SetIcon(wxIcon("APP_ICON", wxBITMAP_TYPE_ICO_RESOURCE));
  image_list_.Add(wxBitmap("NOTEBOOK_LIST", wxBITMAP_TYPE_BMP_RESOURCE));

  // FILE
  auto *menu_file = new wxMenu;
  menu_file->Append(wxID_OPEN);
  menu_file->Append(wxID_SAVE);
  menu_file->Append(wxID_SAVEAS);
  menu_file->Append(wxID_EXIT);

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

  auto *menu_task = new wxMenu;
  menu_task->Append(kIdUpRunner, wxGetStockLabel(wxID_UP));
  menu_task->Append(kIdDownRunner, wxGetStockLabel(wxID_DOWN));
  menu_task->AppendSeparator();
  menu_task->Append(kIdNewRunner, wxGetStockLabel(wxID_NEW));
  menu_task->Append(kIdEditRunner, wxGetStockLabel(wxID_EDIT));
  menu_task->Append(kIdCopyRunner, wxGetStockLabel(wxID_COPY));
  menu_task->Append(kIdRenameRunner, wxString("Rename"));
  menu_task->Append(kIdDeleteRunner, wxGetStockLabel(wxID_DELETE));

  auto *menu_event = new wxMenu;
  menu_event->Append(kIdNewEvent, wxGetStockLabel(wxID_NEW));
  menu_event->Append(kIdEditEvent, wxGetStockLabel(wxID_EDIT));
  menu_event->Append(kIdCopyEvent, wxGetStockLabel(wxID_COPY));
  menu_event->Append(kIdRenameEvent, wxString("Rename"));
  menu_event->Append(kIdDeleteEvent, wxGetStockLabel(wxID_DELETE));

  // Parameter
  auto *menu_parameter = new wxMenu;
  menu_parameter->Append(kIdNewParameter, wxGetStockLabel(wxID_NEW));
  menu_parameter->Append(kIdEditParameter, wxGetStockLabel(wxID_EDIT));
  menu_parameter->Append(kIdDeleteParameter, wxGetStockLabel(wxID_DELETE));

  auto *menu_template = new wxMenu;
  menu_template->Append(kIdNewTemplate, wxGetStockLabel(wxID_NEW));
  menu_template->Append(kIdEditTemplate, wxGetStockLabel(wxID_EDIT));
  menu_template->Append(kIdCopyTemplate, wxGetStockLabel(wxID_COPY));
  menu_template->Append(kIdRenameTemplate, wxString("Rename"));
  menu_template->Append(kIdDeleteTemplate, wxGetStockLabel(wxID_DELETE));

  // ABOUT
  auto *menu_about = new wxMenu;
  menu_about->Append(kIdOpenLogFile, L"Open Log File");
  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT));

  auto *menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, wxGetStockLabel(wxID_FILE));
  menu_bar->Append(menu_workflow, L"Workflow");
  menu_bar->Append(menu_task, L"Task");
  menu_bar->Append(menu_event, L"Event");
  menu_bar->Append(menu_parameter, L"Parameter");
  menu_bar->Append(menu_template, L"Template Task");
  menu_bar->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar);

  notebook_ = new wxNotebook(this, kIdNotebook);
  notebook_->SetImageList(&image_list_);

  auto* workflow_tab = new WorkflowPanel(notebook_);
  auto* event_tab = new EventPanel(notebook_);
  auto* device_tab = new ParameterPanel(notebook_);
  auto* parameter_tab = new ParameterPanel(notebook_);
  auto* template_tab = new TemplatePanel(notebook_);

  notebook_->InsertPage(kPageWorkflow, workflow_tab, L"Workflows",
                        false, kBmpWorkflowDisabled);
  notebook_->InsertPage(kPageEvent, event_tab, L"Events",
                     false, kBmpEventDisabled);
  notebook_->InsertPage(kPageDevice, device_tab, L"Devices",
                        false, kBmpDeviceDisabled);
  notebook_->InsertPage(kPageParameter, parameter_tab, L"Parameters",
                        false, kBmpParameterDisabled);
  notebook_->InsertPage(kPageTemplate, template_tab, L"Template Tasks",
                        false, kBmpTemplateDisabled);

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(notebook_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetContainingSizer(main_sizer);

  notebook_->SetSelection(kPageWorkflow);
  notebook_->SetPageImage(kPageWorkflow,kBmpWorkflowHot);
}

void MainFrame::OnClose(wxCloseEvent &event) {
  auto& app = wxGetApp();
  if ( event.CanVeto() && app.IsModified() ) {
    // Call Save As if no file is selected yet
    if (app.ConfigName().empty()) {
      wxCommandEvent dummy_event;
      app.OnSaveAsFile(dummy_event);
      if (app.ConfigName().empty()) {
        event.Veto();
        return;
      }
    } else {
      // As if file should be saved.
      std::ostringstream ask;
      ask << "The configuration file has not been saved." << std::endl;
      ask << "File: " << app.ConfigName() << std::endl;
      ask << "Dou you want save it before closing?";
      wxMessageDialog dialog(this, wxString::FromUTF8(ask.str()),
                             "Save Config File",
                             wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTRE);
      dialog.SetYesNoCancelLabels(wxID_SAVE, wxID_CLOSE, wxID_CANCEL);

      const auto reply = dialog.ShowModal();
      switch (reply) {
        case wxID_YES:
          app.SaveConfigFile();
          break;

        case wxID_NO:
          break;

        case wxID_CANCEL:
        default:
          event.Veto();
          return;
      }
    }
  }
  // If the window is minimized. Do not save as last position

  if (!IsIconized()) {
    bool maximized = IsMaximized();
    wxPoint end_pos = GetPosition();
    wxSize end_size = GetSize();
    auto* app_config = wxConfig::Get();

    if (maximized) {
      app_config->Write("/MainWin/Max",maximized);
    } else {
      app_config->Write("/MainWin/X", end_pos.x);
      app_config->Write("/MainWin/Y", end_pos.y);
      app_config->Write("/MainWin/XWidth", end_size.x);
      app_config->Write("/MainWin/YWidth", end_size.y);
      app_config->Write("/MainWin/Max", maximized);
    }
  }
  Destroy();
}

void MainFrame::OnAbout(wxCommandEvent&) {

  wxAboutDialogInfo info;
  info.SetName("Workflow Explorer");
  info.SetVersion("1.0");
  info.SetDescription("Workflow Explorer.");

  wxArrayString devs;
  devs.push_back("Ingemar Hedvall");
  info.SetDevelopers(devs);

  info.SetCopyright("(C) 2022 Ingemar Hedvall");
  info.SetLicense("MIT License (https://opensource.org/licenses/MIT)\n"
      "Copyright 2022 Ingemar Hedvall\n"
      "\n"
      "Permission is hereby granted, free of charge, to any person obtaining a copy of this\n"
      "software and associated documentation files (the \"Software\"),\n"
      "to deal in the Software without restriction, including without limitation the rights to use, copy,\n"
      "modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,\n"
      "and to permit persons to whom the Software is furnished to do so, subject to the following conditions:\n"
      "\n"
      "The above copyright notice and this permission notice shall be included in all copies or substantial\n"
      "portions of the Software.\n"
      "\n"
      "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,\n"
      "INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR\n"
      "PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,\n"
      "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR\n"
      "IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE."
  );
  wxAboutBox(info);
}

void MainFrame::OnPageChange(wxBookCtrlEvent &event) {
  if (notebook_ == nullptr) {
    return;
  }
  const auto old_page = event.GetOldSelection();
  switch (old_page) {
    case kPageWorkflow:
      notebook_->SetPageImage(kPageWorkflow, kBmpWorkflowDisabled);
      break;

    case kPageEvent:
      notebook_->SetPageImage(kPageEvent, kBmpEventDisabled);
      break;

    case kPageDevice:
      notebook_->SetPageImage(kPageDevice, kBmpDeviceDisabled);
      break;

    case kPageParameter:
      notebook_->SetPageImage(kPageParameter, kBmpParameterDisabled);
      break;

    case kPageTemplate:
      notebook_->SetPageImage(kPageTemplate, kBmpTemplateDisabled);
      break;

    default:
      break;
  }

  const auto page = event.GetSelection();
  switch (page) {
    case kPageWorkflow:
      notebook_->SetPageImage(kPageWorkflow, kBmpWorkflowHot);
      break;

    case kPageEvent:
      notebook_->SetPageImage(kPageEvent, kBmpEventHot);
      break;

    case kPageDevice:
      notebook_->SetPageImage(kPageDevice, kBmpDeviceHot);
      break;

    case kPageParameter:
      notebook_->SetPageImage(kPageParameter, kBmpParameterHot);
      break;

    case kPageTemplate:
       notebook_->SetPageImage(kPageTemplate, kBmpTemplateHot);
       break;

    default:
      break;
  }

  auto* window = notebook_->GetPage(page);
  if (window != nullptr) {
    window->Update();
  }
}

void MainFrame::Update() {
  wxWindow::Update();
  const auto& app = wxGetApp();
  std::ostringstream title;
  title << app.GetAppDisplayName();
  const auto name = app.ConfigName();
  if (!name.empty()) {
    title << " - " << name;
  }
  SetTitle(title.str());

  if (notebook_ == nullptr) {
    return;
  }
  const size_t count = notebook_->GetPageCount();
  for (size_t page = 0; page < count; ++page) {
    auto* tab = notebook_->GetPage(page);
    if (tab != nullptr) {
      tab->Update();
    }
  }
}

void MainFrame::OnUpdateWorkflow(wxUpdateUIEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel == nullptr) {
    event.Enable(false);
    return;
  }

  if (notebook_->GetSelection() == kPageWorkflow) {
    panel->ProcessWindowEventLocally(event);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnWorkflow(wxCommandEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel != nullptr && notebook_->GetSelection() == kPageWorkflow) {
    panel->ProcessWindowEventLocally(event);
  }
}

void MainFrame::OnUpdateEvent(wxUpdateUIEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel == nullptr) {
    event.Enable(false);
    return;
  }
  if (notebook_->GetSelection() == kPageEvent) {
    panel->ProcessWindowEventLocally(event);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnEvent(wxCommandEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel != nullptr && notebook_->GetSelection() == kPageEvent) {
    panel->ProcessWindowEventLocally(event);
  }
}

void MainFrame::OnUpdateParameter(wxUpdateUIEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel == nullptr) {
    event.Enable(false);
    return;
  }
  if (notebook_->GetSelection() == kPageParameter) {
    panel->ProcessWindowEventLocally(event);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnParameter(wxCommandEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel != nullptr && notebook_->GetSelection() == kPageParameter) {
    panel->ProcessWindowEventLocally(event);
  }
}

void MainFrame::OnUpdateTemplate(wxUpdateUIEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel == nullptr) {
    event.Enable(false);
    return;
  }
  if (notebook_->GetSelection() == kPageTemplate) {
    panel->ProcessWindowEventLocally(event);
  } else {
    event.Enable(false);
  }
}

void MainFrame::OnTemplate(wxCommandEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel != nullptr && notebook_->GetSelection() == kPageTemplate) {
    panel->ProcessWindowEventLocally(event);
  }
}

void MainFrame::OnExit(wxCommandEvent& event) {
  Close(false);
}

}