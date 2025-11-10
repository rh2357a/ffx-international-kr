#include "ffx_international_kr/main_frame.h"
#include <wx/wx.h>

class App : public wxApp
{
private:
	ffx::MainFrame *m_mainFrame;

public:
	virtual bool OnInit();
};

bool App::OnInit()
{
	m_mainFrame = new ffx::MainFrame(nullptr);
	m_mainFrame->Show(true);
	return true;
}

wxIMPLEMENT_APP(App);
