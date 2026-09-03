///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "gui_base.h"

///////////////////////////////////////////////////////////////////////////

MainFrameBase::MainFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	this->SetBackgroundColour( wxColour( 240, 240, 240 ) );

	wxBoxSizer* mainBSizer;
	mainBSizer = new wxBoxSizer( wxVERTICAL );

	m_mainPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* mainPanelBSizer;
	mainPanelBSizer = new wxBoxSizer( wxVERTICAL );

	m_workspacePanel = new wxPanel( m_mainPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* workspaceBSizer;
	workspaceBSizer = new wxBoxSizer( wxVERTICAL );

	wxStaticBoxSizer* baseIsoSbSizer;
	baseIsoSbSizer = new wxStaticBoxSizer( new wxStaticBox( m_workspacePanel, wxID_ANY, wxT("FFX 인터내셔널 ISO 파일 선택 (패치 대상)") ), wxVERTICAL );

	wxBoxSizer* baseIsoBSizer;
	baseIsoBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_baseIsoText = new wxTextCtrl( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	baseIsoBSizer->Add( m_baseIsoText, 12, wxALIGN_CENTER_VERTICAL, 5 );


	baseIsoBSizer->Add( 8, 0, 0, wxEXPAND, 5 );

	m_openBaseIsoButton = new wxButton( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("열기"), wxDefaultPosition, wxDefaultSize, 0 );
	baseIsoBSizer->Add( m_openBaseIsoButton, 3, wxALIGN_CENTER_VERTICAL, 5 );


	baseIsoSbSizer->Add( baseIsoBSizer, 1, wxALL|wxEXPAND, 5 );

	m_baseIsoWarnText = new wxStaticText( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("※ 게임 ID 'SLPM-67513'인 파일이 필요합니다."), wxDefaultPosition, wxDefaultSize, 0 );
	m_baseIsoWarnText->Wrap( -1 );
	baseIsoSbSizer->Add( m_baseIsoWarnText, 0, wxALL, 5 );


	workspaceBSizer->Add( baseIsoSbSizer, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* jpnVoiceOptionSbSizer;
	jpnVoiceOptionSbSizer = new wxStaticBoxSizer( new wxStaticBox( m_workspacePanel, wxID_ANY, wxT("일어판 FF10 오리지널 ISO 파일 선택") ), wxVERTICAL );

	wxBoxSizer* jpnVoiceOptionBSizer;
	jpnVoiceOptionBSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* jpnIsoBSizer;
	jpnIsoBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_jpnIsoText = new wxTextCtrl( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	jpnIsoBSizer->Add( m_jpnIsoText, 12, wxALIGN_CENTER_VERTICAL, 5 );


	jpnIsoBSizer->Add( 8, 0, 0, wxEXPAND, 5 );

	m_openJpnIsoButton = new wxButton( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("열기"), wxDefaultPosition, wxDefaultSize, 0 );
	jpnIsoBSizer->Add( m_openJpnIsoButton, 3, wxALIGN_CENTER_VERTICAL, 5 );


	jpnVoiceOptionBSizer->Add( jpnIsoBSizer, 1, wxEXPAND, 5 );


	jpnVoiceOptionSbSizer->Add( jpnVoiceOptionBSizer, 1, wxALL|wxEXPAND, 5 );

	m_jpnIsoWarnText = new wxStaticText( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("※ 게임 ID 'SLPS-25050'인 파일이 필요합니다."), wxDefaultPosition, wxDefaultSize, 0 );
	m_jpnIsoWarnText->Wrap( -1 );
	jpnVoiceOptionSbSizer->Add( m_jpnIsoWarnText, 0, wxALL, 5 );


	workspaceBSizer->Add( jpnVoiceOptionSbSizer, 0, wxALL|wxEXPAND, 5 );

	m_jpnIsoWarn2Text = new wxStaticText( m_workspacePanel, wxID_ANY, wxT("※ 베타 버전 주의\n　- 자막과 음성이 다를 수 있습니다.\n　- 일부 장면에서 영문 음성이 나올 수 있습니다.\n　- 한국어 노래가 일본어 버전으로 바뀝니다."), wxDefaultPosition, wxDefaultSize, 0 );
	m_jpnIsoWarn2Text->Wrap( -1 );
	workspaceBSizer->Add( m_jpnIsoWarn2Text, 0, wxALL, 5 );


	m_workspacePanel->SetSizer( workspaceBSizer );
	m_workspacePanel->Layout();
	workspaceBSizer->Fit( m_workspacePanel );
	mainPanelBSizer->Add( m_workspacePanel, 0, wxALL|wxEXPAND, 0 );

	wxBoxSizer* applyBSizer;
	applyBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_applyButton = new wxButton( m_mainPanel, wxID_ANY, wxT("패치"), wxDefaultPosition, wxSize( -1,-1 ), 0 );
	applyBSizer->Add( m_applyButton, 2, wxALL|wxEXPAND, 4 );

	wxBoxSizer* applyGagueBSizer;
	applyGagueBSizer = new wxBoxSizer( wxVERTICAL );

	m_applyGauge = new wxGauge( m_mainPanel, wxID_ANY, 88, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	m_applyGauge->SetValue( 0 );
	applyGagueBSizer->Add( m_applyGauge, 1, wxALL|wxEXPAND, 0 );


	applyGagueBSizer->Add( 0, 6, 0, wxEXPAND, 0 );

	m_applyPathProgressText = new wxStaticText( m_mainPanel, wxID_ANY, wxT("준비"), wxDefaultPosition, wxDefaultSize, 0 );
	m_applyPathProgressText->Wrap( -1 );
	applyGagueBSizer->Add( m_applyPathProgressText, 0, wxALL, 0 );


	applyBSizer->Add( applyGagueBSizer, 7, wxALL|wxEXPAND, 4 );


	mainPanelBSizer->Add( applyBSizer, 1, wxALL|wxEXPAND, 0 );


	m_mainPanel->SetSizer( mainPanelBSizer );
	m_mainPanel->Layout();
	mainPanelBSizer->Fit( m_mainPanel );
	mainBSizer->Add( m_mainPanel, 1, wxALL|wxEXPAND, 5 );


	this->SetSizer( mainBSizer );
	this->Layout();

	this->Centre( wxBOTH );

	// Connect Events
	this->Connect( wxEVT_CLOSE_WINDOW, wxCloseEventHandler( MainFrameBase::OnFrameClose ) );
	m_openBaseIsoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnOpenBaseIsoButtonClick ), NULL, this );
	m_openJpnIsoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnOpenJpnIsoButtonClick ), NULL, this );
	m_applyButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnApplyButtonClick ), NULL, this );
}

MainFrameBase::~MainFrameBase()
{
}
