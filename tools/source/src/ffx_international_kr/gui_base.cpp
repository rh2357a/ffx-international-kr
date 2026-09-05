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
	baseIsoSbSizer = new wxStaticBoxSizer( new wxStaticBox( m_workspacePanel, wxID_ANY, wxT("패치 ISO 파일 선택") ), wxVERTICAL );

	wxBoxSizer* baseIsoBSizer;
	baseIsoBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_baseIsoLabelText = new wxStaticText( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("ISO 위치："), wxDefaultPosition, wxSize( 64,-1 ), 0 );
	m_baseIsoLabelText->Wrap( -1 );
	baseIsoBSizer->Add( m_baseIsoLabelText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_baseIsoText = new wxTextCtrl( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	baseIsoBSizer->Add( m_baseIsoText, 12, wxALIGN_CENTER_VERTICAL, 5 );


	baseIsoBSizer->Add( 8, 0, 0, wxEXPAND, 5 );

	m_openBaseIsoButton = new wxButton( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("찾기"), wxDefaultPosition, wxDefaultSize, 0 );
	baseIsoBSizer->Add( m_openBaseIsoButton, 3, wxALIGN_CENTER_VERTICAL, 5 );


	baseIsoSbSizer->Add( baseIsoBSizer, 1, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );

	wxBoxSizer* targetIsoBSizer;
	targetIsoBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_targetIsoLabelText = new wxStaticText( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("저장 위치："), wxDefaultPosition, wxSize( 64,-1 ), 0 );
	m_targetIsoLabelText->Wrap( -1 );
	targetIsoBSizer->Add( m_targetIsoLabelText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_targetIsoText = new wxTextCtrl( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	targetIsoBSizer->Add( m_targetIsoText, 12, wxALIGN_CENTER_VERTICAL, 5 );


	targetIsoBSizer->Add( 8, 0, 0, wxEXPAND, 5 );

	m_targetIsoButton = new wxButton( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("찾기"), wxDefaultPosition, wxDefaultSize, 0 );
	targetIsoBSizer->Add( m_targetIsoButton, 3, wxALIGN_CENTER_VERTICAL, 5 );


	baseIsoSbSizer->Add( targetIsoBSizer, 1, wxALL|wxEXPAND, 5 );

	m_baseIsoWarnText = new wxStaticText( baseIsoSbSizer->GetStaticBox(), wxID_ANY, wxT("※ 게임 ID: 국내 정발판 (SLPM-67513)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_baseIsoWarnText->Wrap( -1 );
	baseIsoSbSizer->Add( m_baseIsoWarnText, 0, wxALL, 5 );


	workspaceBSizer->Add( baseIsoSbSizer, 0, wxALL|wxEXPAND, 5 );

	wxStaticBoxSizer* jpnVoiceOptionSbSizer;
	jpnVoiceOptionSbSizer = new wxStaticBoxSizer( new wxStaticBox( m_workspacePanel, wxID_ANY, wxT("일본어 음성 옵션 (beta)") ), wxVERTICAL );

	m_jpnVoiceCheckBox = new wxCheckBox( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("활성화 (ISO 파일 필요)"), wxDefaultPosition, wxDefaultSize, 0 );
	jpnVoiceOptionSbSizer->Add( m_jpnVoiceCheckBox, 0, wxALL, 5 );

	wxBoxSizer* jpnVoiceOptionBSizer;
	jpnVoiceOptionBSizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* jpnIsoBSizer;
	jpnIsoBSizer = new wxBoxSizer( wxHORIZONTAL );

	m_jpnIsoText = new wxTextCtrl( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("-"), wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
	m_jpnIsoText->Enable( false );

	jpnIsoBSizer->Add( m_jpnIsoText, 12, wxALIGN_CENTER_VERTICAL, 5 );


	jpnIsoBSizer->Add( 8, 0, 0, wxEXPAND, 5 );

	m_openJpnIsoButton = new wxButton( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("찾기"), wxDefaultPosition, wxDefaultSize, 0 );
	m_openJpnIsoButton->Enable( false );

	jpnIsoBSizer->Add( m_openJpnIsoButton, 3, wxALIGN_CENTER_VERTICAL, 5 );


	jpnVoiceOptionBSizer->Add( jpnIsoBSizer, 1, wxEXPAND, 5 );


	jpnVoiceOptionSbSizer->Add( jpnVoiceOptionBSizer, 1, wxALL|wxEXPAND, 5 );

	wxBoxSizer* jpnMusicType1BSizer;
	jpnMusicType1BSizer = new wxBoxSizer( wxHORIZONTAL );

	m_jpnMusicType1LabelText = new wxStaticText( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("노래 선택："), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_jpnMusicType1LabelText->Wrap( -1 );
	m_jpnMusicType1LabelText->Enable( false );

	jpnMusicType1BSizer->Add( m_jpnMusicType1LabelText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_jpnMusicType1ChoiceChoices[] = { wxT("얼마나 좋을까"), wxT("素敵だね") };
	int m_jpnMusicType1ChoiceNChoices = sizeof( m_jpnMusicType1ChoiceChoices ) / sizeof( wxString );
	m_jpnMusicType1Choice = new wxChoice( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), m_jpnMusicType1ChoiceNChoices, m_jpnMusicType1ChoiceChoices, 0 );
	m_jpnMusicType1Choice->SetSelection( 0 );
	m_jpnMusicType1Choice->Enable( false );

	jpnMusicType1BSizer->Add( m_jpnMusicType1Choice, 0, wxALL, 5 );


	jpnVoiceOptionSbSizer->Add( jpnMusicType1BSizer, 1, wxEXPAND, 5 );

	wxBoxSizer* jpnMusicType2BSizer;
	jpnMusicType2BSizer = new wxBoxSizer( wxHORIZONTAL );

	m_jpnMusicType2LabelText = new wxStaticText( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("노래 선택(엔딩)："), wxDefaultPosition, wxSize( 100,-1 ), 0 );
	m_jpnMusicType2LabelText->Wrap( -1 );
	m_jpnMusicType2LabelText->Enable( false );

	jpnMusicType2BSizer->Add( m_jpnMusicType2LabelText, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	wxString m_jpnMusicType2ChoiceChoices[] = { wxT("얼마나 좋을까"), wxT("素敵だね") };
	int m_jpnMusicType2ChoiceNChoices = sizeof( m_jpnMusicType2ChoiceChoices ) / sizeof( wxString );
	m_jpnMusicType2Choice = new wxChoice( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, wxSize( 160,-1 ), m_jpnMusicType2ChoiceNChoices, m_jpnMusicType2ChoiceChoices, 0 );
	m_jpnMusicType2Choice->SetSelection( 0 );
	m_jpnMusicType2Choice->Enable( false );

	jpnMusicType2BSizer->Add( m_jpnMusicType2Choice, 0, wxALL, 5 );


	jpnVoiceOptionSbSizer->Add( jpnMusicType2BSizer, 1, wxEXPAND, 5 );

	m_jpnIsoWarnText = new wxStaticText( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("※ 게임 ID: 오리지널 일본판 (SLPS-25050)"), wxDefaultPosition, wxDefaultSize, 0 );
	m_jpnIsoWarnText->Wrap( -1 );
	m_jpnIsoWarnText->Enable( false );

	jpnVoiceOptionSbSizer->Add( m_jpnIsoWarnText, 0, wxLEFT|wxRIGHT|wxTOP, 5 );

	m_jpnIsoWarn2Text = new wxStaticText( jpnVoiceOptionSbSizer->GetStaticBox(), wxID_ANY, wxT("※ 베타 버전 주의\n　- 일부 장면에서 영문 음성이 나올 수 있습니다.\n　- 아직 불안정한 부분이 다소 있습니다."), wxDefaultPosition, wxDefaultSize, 0 );
	m_jpnIsoWarn2Text->Wrap( -1 );
	m_jpnIsoWarn2Text->Enable( false );

	jpnVoiceOptionSbSizer->Add( m_jpnIsoWarn2Text, 0, wxALL, 5 );


	workspaceBSizer->Add( jpnVoiceOptionSbSizer, 0, wxALL|wxEXPAND, 5 );


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
	m_targetIsoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnTargetIsoButtonClick ), NULL, this );
	m_jpnVoiceCheckBox->Connect( wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( MainFrameBase::OnJpnVoiceCheckBox ), NULL, this );
	m_openJpnIsoButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnOpenJpnIsoButtonClick ), NULL, this );
	m_applyButton->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( MainFrameBase::OnApplyButtonClick ), NULL, this );
}

MainFrameBase::~MainFrameBase()
{
}
