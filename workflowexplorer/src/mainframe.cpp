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
#include "parameterpanel.h"
#include "workflowexplorer.h"
#include "workflowexplorerid.h"

namespace {

constexpr int kPageWorkflow = 0;
//constexpr int kPageEvent = 1;
constexpr int kPageParameter = 1;


}

namespace workflow::gui {

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
  EVT_MENU(wxID_ABOUT, MainFrame::OnAbout)
  EVT_MENU(wxID_EXIT, MainFrame::OnExit)
  EVT_CLOSE(MainFrame::OnClose)
  EVT_NOTEBOOK_PAGE_CHANGED(kIdNotebook, MainFrame::OnPageChange)

  EVT_UPDATE_UI(kIdNewParameter, MainFrame::OnUpdateParameter)
  EVT_MENU(kIdNewParameter, MainFrame::OnParameter)
wxEND_EVENT_TABLE()

MainFrame::MainFrame(const wxString& title, const wxPoint& start_pos, const wxSize& start_size, bool maximized)
    : wxFrame(nullptr, wxID_ANY, title, start_pos, start_size),
    image_list_(32,32,false,6) {
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
  menu_workflow->Append(kIdNewWorkflow, wxGetStockLabel(wxID_NEW));
  menu_workflow->Append(kIdEditWorkflow, wxGetStockLabel(wxID_EDIT));
  menu_workflow->Append(kIdDeleteWorkflow, wxGetStockLabel(wxID_DELETE));

  // Parameter
  auto *menu_parameter = new wxMenu;
  menu_parameter->Append(kIdNewParameter, wxGetStockLabel(wxID_NEW));
  menu_parameter->Append(kIdEditParameter, wxGetStockLabel(wxID_EDIT));
  menu_parameter->Append(kIdDeleteParameter, wxGetStockLabel(wxID_DELETE));

  // ABOUT
  auto *menu_about = new wxMenu;
  menu_about->Append(kIdOpenLogFile, L"Open Log File");
  menu_about->AppendSeparator();
  menu_about->Append(wxID_ABOUT, wxGetStockLabel(wxID_ABOUT));

  auto *menu_bar = new wxMenuBar;
  menu_bar->Append(menu_file, wxGetStockLabel(wxID_FILE));
  menu_bar->Append(menu_workflow, L"Workflow");
  menu_bar->Append(menu_parameter, L"Parameter");
  menu_bar->Append(menu_about, wxGetStockLabel(wxID_HELP));
  wxFrameBase::SetMenuBar(menu_bar);

  notebook_ = new wxNotebook(this, kIdNotebook);
  notebook_->SetImageList(&image_list_);

  auto* workflow_tab = new WorkflowPanel(notebook_);
  auto* parameter_tab = new ParameterPanel(notebook_);
/*
  auto* select_tab = new MeasurementTab(notebook_);
  auto* plot_tab = new PlotTab(notebook_);
*/
  notebook_->AddPage(workflow_tab, L"Workflow", true,3);
  notebook_->AddPage(parameter_tab, L"Parameters", false,4);
/*
  notebook_->AddPage(select_tab, L"Selection", true,4);
  notebook_->AddPage(plot_tab, L"Plot", true,5);
*/

  auto* main_sizer = new wxBoxSizer(wxVERTICAL);
  main_sizer->Add(notebook_, 1, wxALIGN_LEFT | wxALL | wxEXPAND, 0);
  SetContainingSizer(main_sizer);

  auto& app = wxGetApp();

  //if (app.EnvList().empty()) {
    notebook_->SetSelection(kPageWorkflow);
    notebook_->SetPageImage(kPageWorkflow,0);
 // } else {
 //   notebook_->SetSelection(kPageSelection);
 //   notebook_->SetPageImage(kPageSelection,1);
 // }

}

void MainFrame::OnClose(wxCloseEvent &event) {
  auto& app = wxGetApp();
  if ( event.CanVeto() && app.IsModified() ) {
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
  const auto page = event.GetSelection();
  auto* window = notebook_->GetPage(page);
  if (old_page != wxNOT_FOUND) {
    notebook_->SetPageImage(old_page, old_page + 3);
  }
  if (page != wxNOT_FOUND) {
    notebook_->SetPageImage(page, page);
  }
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

void MainFrame::OnUpdateParameter(wxUpdateUIEvent& event) {
  auto* panel = notebook_ ? notebook_->GetCurrentPage() : nullptr;
  if (panel != nullptr && notebook_->GetSelection() == kPageParameter) {
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

void MainFrame::OnExit(wxCommandEvent& event) {
  Close(false);
}

}