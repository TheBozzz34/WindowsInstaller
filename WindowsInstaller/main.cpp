#include <wx/wx.h>

class WindowsInstaller : public wxApp
{
public:
    bool OnInit() override;
};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(WindowsInstaller);

class NewFrame : public wxFrame
{
public:
   NewFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnInstallButtonClick(wxCommandEvent& event);
    void OnSetButtonClick(wxCommandEvent& event);

    wxButton* install_button;
    wxButton* set_location;

    wxTextCtrl* textCtrl;
};

bool WindowsInstaller::OnInit()
{
    NewFrame* frame = new NewFrame();
    frame->Show();
    return true;
}


enum
{
    ID_Hello = 1,
    ID_Install_Button = 2,
    ID_Set_Location = 3

};


NewFrame::NewFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
        "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu* menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
    textCtrl->SetBackgroundColour(*wxWHITE);
    textCtrl->SetForegroundColour(*wxBLACK);
    textCtrl->SetFont(wxFont(wxFontInfo(10)));

    wxString licenseText = "Copyright 2023 Ethan James \n\n Permission is hereby granted, free of charge, to any person obtaining a copy of this software and \n associated documentation files(the “Software”), to deal in the Software without restriction, \n including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, \n and /or sell copies of the Software, and to permit persons to whom the Software is furnished to do \n so, subject to the following conditions : \n The above copyright notice and this permission notice shall be included in all copies or substantial \n portions of the Software. \n\n THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS \n FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS \n OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, \n WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN \n CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";
    textCtrl->SetValue(licenseText);

    mainSizer->Add(textCtrl, 1, wxEXPAND | wxALL, 20);

    install_button = new wxButton(this, ID_Install_Button, "Install", wxPoint(20, 20));
    set_location = new wxButton(this, ID_Set_Location, "Set Location", wxPoint(20, 20));

    mainSizer->AddStretchSpacer(); 
    mainSizer->Add(install_button, 0, wxALIGN_CENTER | wxALL, 10); 
    mainSizer->Add(set_location, 0, wxALIGN_CENTER | wxALL, 5);


    SetSizerAndFit(mainSizer);



    Bind(wxEVT_MENU, &NewFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &NewFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &NewFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_BUTTON, &NewFrame::OnInstallButtonClick, this, ID_Install_Button);
    Bind(wxEVT_BUTTON, &NewFrame::OnSetButtonClick, this, ID_Set_Location);


    wxWindow::SetInitialSize(wxSize(700, 500));
}

void NewFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void NewFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
        "About Hello World", wxOK | wxICON_INFORMATION);
}

void NewFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}

void NewFrame::OnInstallButtonClick(wxCommandEvent& event)
{
    wxMessageBox("Button clicked!", "Button", wxOK | wxICON_INFORMATION);
}

void NewFrame::OnSetButtonClick(wxCommandEvent& event)
{
    wxMessageBox("Button clicked!", "Button", wxOK | wxICON_INFORMATION);
}