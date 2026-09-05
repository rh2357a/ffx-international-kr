///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/panel.h>
#include <wx/gauge.h>
#include <wx/frame.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class MainFrameBase
///////////////////////////////////////////////////////////////////////////////
class MainFrameBase : public wxFrame
{
	private:

	protected:
		wxPanel* m_mainPanel;
		wxPanel* m_workspacePanel;
		wxStaticText* m_baseIsoLabelText;
		wxTextCtrl* m_baseIsoText;
		wxButton* m_openBaseIsoButton;
		wxStaticText* m_targetIsoLabelText;
		wxTextCtrl* m_targetIsoText;
		wxButton* m_targetIsoButton;
		wxStaticText* m_baseIsoWarnText;
		wxCheckBox* m_jpnVoiceCheckBox;
		wxTextCtrl* m_jpnIsoText;
		wxButton* m_openJpnIsoButton;
		wxStaticText* m_jpnMusicType1LabelText;
		wxChoice* m_jpnMusicType1Choice;
		wxStaticText* m_jpnMusicType2LabelText;
		wxChoice* m_jpnMusicType2Choice;
		wxStaticText* m_jpnIsoWarnText;
		wxStaticText* m_jpnIsoWarn2Text;
		wxButton* m_applyButton;
		wxGauge* m_applyGauge;
		wxStaticText* m_applyPathProgressText;

		// Virtual event handlers, override them in your derived class
		virtual void OnFrameClose( wxCloseEvent& event ) { event.Skip(); }
		virtual void OnOpenBaseIsoButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnTargetIsoButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnJpnVoiceCheckBox( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnOpenJpnIsoButtonClick( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnApplyButtonClick( wxCommandEvent& event ) { event.Skip(); }


	public:

		MainFrameBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("{title}"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 560,453 ), long style = wxCAPTION|wxCLOSE_BOX|wxICONIZE|wxSYSTEM_MENU|wxTAB_TRAVERSAL );

		~MainFrameBase();

};

