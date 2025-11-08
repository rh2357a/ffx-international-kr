#include <wx/wx.h>

#include "ffx_international_kr/gui_base.h"

class App : public wxApp
{
public:
	virtual bool OnInit();
};

wxIMPLEMENT_APP(App);

bool App::OnInit()
{
	main_form *frame = new main_form(nullptr);
	frame->Show(true);
	return true;
}
