#include <wx/wx.h>
#include <wx/filepicker.h>

#include <iostream>
#include <fstream>
#include <curl/curl.h>
#include <thread>
#include <filesystem>
#include <future>



/*
    This is a simple installer for Windows. It will download a binary file from a URL and install it to a specified location.
    The installer will also create a desktop shortcut to the installed file.
*/


class BinaryFileDownloader  // This class is used to download a binary file from a URL, it its incredibly janky and I would not recommend using it.
{
public:
    BinaryFileDownloader(wxGauge* progressGauge)
        : progressGauge_(progressGauge)
    {
    }

    bool DownloadFile(const std::string& url, const std::string& destination)
    {
        FILE* file = fopen(destination.c_str(), "wb");
        if (!file)
        {
            std::cerr << "Failed to open destination file: " << destination << std::endl; // wtf this is not a console app lmao, idk why this is here
            return false;
        }

        CURL* curl = curl_easy_init();
        if (!curl)
        {
            std::cerr << "Failed to initialize libcurl" << std::endl;
            fclose(file);
            return false;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // goofy cURL stuff
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, ProgressCallback);
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, this);

        CURLcode res = curl_easy_perform(curl);
        fclose(file);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
        {
            std::cerr << "Failed to download file: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        std::cout << "File downloaded successfully!" << std::endl;
        return true;
    }

    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userdata)
    {
        FILE* file = static_cast<FILE*>(userdata);
        size_t written = fwrite(contents, size, nmemb, file);  // write the file probably
        return written;
    }

    static int ProgressCallback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
    {
        BinaryFileDownloader* downloader = static_cast<BinaryFileDownloader*>(clientp);
        if (downloader->progressGauge_)
        {
            if (dltotal > 0)
            {
                double progress = static_cast<double>(dlnow) / static_cast<double>(dltotal) * 100.0;
                downloader->progressGauge_->SetValue(static_cast<int>(progress));  // update the progress bar
            }
        }
        return 0;
    }

private:
    wxGauge* progressGauge_;  // why is this here lmao
};

enum
{
    ID_Hello = 1,
    ID_Install_Button = 2,
    ID_Set_Location = 3,
    ID_Accept_CheckBox = 4,
    ID_UPDATE_PROGRESS = 5,
    ID_DOWNLOAD_COMPLETE = 6,
    ID_DOWNLOAD_ERROR = 7,
    ID_ENABLE_INSTALL_BUTTON = 8,
    ID_DIRPICKERCTRL = 9,

};

class NewFrame : public wxFrame
{
public:
    NewFrame();
    void OnHello(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnInstallButtonClick(wxCommandEvent& event);
    void OnSetButtonClick(wxCommandEvent& event);
    void OnCheckBoxClicked(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnDownloadComplete(wxThreadEvent& event);
    void OnDirPickerChanged(wxFileDirPickerEvent& event);

private:
    wxButton* install_button;
    wxButton* set_location;
    wxTextCtrl* textCtrl;
    wxTextCtrl* statusCtrl;
    wxCheckBox* accept_checkbox;
    wxStaticText* label;
    wxGauge* gauge;
    wxDirPickerCtrl* dirPickerCtrl;
    std::thread downloadThread;
    std::string downloadUrl;
    // sizers should be here
};

class WindowsInstallerApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

private:
    NewFrame* frame;
};

wxIMPLEMENT_APP(WindowsInstallerApp); // this defines the main entry point for the application

bool WindowsInstallerApp::OnInit()
{
    frame = new NewFrame();
    frame->Show(true); // show the window
    return true;
}


NewFrame::NewFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World", wxDefaultPosition, wxSize(700, 500), wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX)) // create the window and set its size and title

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
    SetStatusText("Installer Ready!");

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxHSCROLL);
    textCtrl->SetBackgroundColour(*wxWHITE);
    textCtrl->SetForegroundColour(*wxBLACK);
    textCtrl->SetFont(wxFont(wxFontInfo(10)));


    // This is a shitty way to do this but I don't care
    wxString licenseText = "Copyright 2023 Ethan James \n\n"
        "Permission is hereby granted, free of charge, to any person obtaining a copy of this software "
        "and \nassociated documentation files(the “Software”), to deal in the Software without restriction, "
        "\nincluding without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, "
        "\nand/or sell copies of the Software, and to permit persons to whom the Software is furnished to do \n"
        "so, subject to the following conditions : \n"
        "The above copyright notice and this permission notice shall be included in all copies or substantial \n"
        "portions of the Software. \n\n"
        "THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR \n"
        "IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS \n"
        "FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS \n"
        "OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, \n"
        "WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN \n"
        "CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.";
    textCtrl->SetValue(licenseText);

    mainSizer->Add(textCtrl, 1, wxEXPAND | wxALL, 20);

    accept_checkbox = new wxCheckBox(this, ID_Accept_CheckBox, "I accept the license agreement");
    mainSizer->Add(accept_checkbox, 0, wxALIGN_LEFT | wxALL, 10);

    gauge = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL);
    mainSizer->Add(gauge, 0, wxEXPAND | wxALL, 20);
    gauge->SetValue(0);

    install_button = new wxButton(this, ID_Install_Button, "Install", wxPoint(20, 20));
    // set_location = new wxButton(this, ID_Set_Location, "Set Location", wxPoint(20, 20));

    /* idk why i did this with a textctrl and not a statictext
    wxString locationLabel = "Install Location: ";
    locationLabelCtrl = new wxTextCtrl(this, wxID_ANY, locationLabel, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    locationLabelCtrl->SetBackgroundColour(*wxWHITE);
    locationLabelCtrl->SetForegroundColour(*wxBLACK);
    locationLabelCtrl->SetFont(wxFont(wxFontInfo(10)));
    mainSizer->Add(locationLabelCtrl, 0, wxEXPAND | wxALL, 20);
    */

    wxBoxSizer* horizontalSizer = new wxBoxSizer(wxHORIZONTAL); // create a sizer to hold the buttons, not sure why the variable isnt with the rest of the variables in the class definition
    label = new wxStaticText(this, wxID_ANY, "Select an install folder: ");
    horizontalSizer->Add(label, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    dirPickerCtrl = new wxDirPickerCtrl(this, ID_DIRPICKERCTRL, wxEmptyString, wxDirSelectorPromptStr, wxDefaultPosition, wxDefaultSize, wxDIRP_DIR_MUST_EXIST | wxDIRP_USE_TEXTCTRL);
    horizontalSizer->Add(dirPickerCtrl, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

    // Add the horizontal sizer to the main sizer
    mainSizer->Add(horizontalSizer, 0, wxALIGN_CENTER | wxALL, 20);

    install_button->Enable(false);
    dirPickerCtrl->Enable(false);

    mainSizer->AddStretchSpacer();
    mainSizer->Add(install_button, 0, wxALIGN_CENTER | wxALL, 10);
    // mainSizer->Add(set_location, 0, wxALIGN_CENTER | wxALL, 5);

    SetSizerAndFit(mainSizer);


    // Connect event handlers
    Bind(wxEVT_MENU, &NewFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &NewFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &NewFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_BUTTON, &NewFrame::OnInstallButtonClick, this, ID_Install_Button);
    Bind(wxEVT_BUTTON, &NewFrame::OnSetButtonClick, this, ID_Set_Location);
    Bind(wxEVT_CHECKBOX, &NewFrame::OnCheckBoxClicked, this, ID_Accept_CheckBox);
    Bind(wxEVT_THREAD, &NewFrame::OnDownloadComplete, this, ID_DOWNLOAD_COMPLETE);
    Bind(wxEVT_DIRPICKER_CHANGED, &NewFrame::OnDirPickerChanged, this, ID_DIRPICKERCTRL);

        

    wxWindow::SetInitialSize(wxSize(700, 500));
}

void NewFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a simple windows application installer, written by Sadan#9264",
        "About This App", wxOK | wxICON_INFORMATION);
}

void NewFrame::OnHello(wxCommandEvent& event)
{
    // wxLogMessage("Hello world from wxWidgets!");
}

void NewFrame::OnInstallButtonClick(wxCommandEvent& event)
{
    if (!accept_checkbox->IsChecked())
    {
        wxMessageBox("Please accept the license agreement.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    install_button->Enable(false);
    accept_checkbox->Enable(false); // disable the checkbox and stuff
    dirPickerCtrl->Enable(false);

    
    downloadThread = std::thread([this]() {
         BinaryFileDownloader downloader(gauge);
         std::string url = "https://releases.ubuntu.com/22.04.2/ubuntu-22.04.2-desktop-amd64.iso";
         std::string destination = downloadUrl + "\\ubuntu-22.04.2-desktop-amd64.iso";
         bool success = downloader.DownloadFile(url, destination);

         // Notify the main thread when the download is complete.
         wxThreadEvent* event = new wxThreadEvent(wxEVT_THREAD, ID_DOWNLOAD_COMPLETE);
         event->SetString(success ? "Download successful" : "Download failed");
         wxQueueEvent(this, event);
     });
    
}

void NewFrame::OnDownloadComplete(wxThreadEvent& event)
{
    // Cleanup the download thread
    if (downloadThread.joinable()) {
        downloadThread.join();
    }
}


void NewFrame::OnSetButtonClick(wxCommandEvent& event)
{
    
}

void NewFrame::OnCheckBoxClicked(wxCommandEvent& event)
{
    if (accept_checkbox->IsChecked())
    {
		install_button->Enable(true);
        dirPickerCtrl->Enable(true);
	}
    else
    {
		install_button->Enable(false);
        dirPickerCtrl->Enable(false);
	}
}

void NewFrame::OnDirPickerChanged(wxFileDirPickerEvent& event)
{
    downloadUrl = event.GetPath();
	// locationLabelCtrl->SetValue(path);
}

int WindowsInstallerApp::OnExit()
{
    delete frame; // frame is really a NewFrame
    return wxApp::OnExit();
}


void NewFrame::OnExit(wxCommandEvent& event)
{
    Close(true); // true is to force the frame to close
}
