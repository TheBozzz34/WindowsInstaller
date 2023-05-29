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
};

bool WindowsInstaller::OnInit()
{
    NewFrame* frame = new NewFrame();
    frame->Show();
    return true;
}


enum
{
    ID_Hello = 1
};

NewFrame::NewFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World")
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

    Bind(wxEVT_MENU, &NewFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &NewFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &NewFrame::OnExit, this, wxID_EXIT);
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