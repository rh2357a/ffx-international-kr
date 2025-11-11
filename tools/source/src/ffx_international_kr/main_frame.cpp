#include "main_frame.h"

#include "patch_data.h"

#include "ffxiso/extract.h"
#include "ffxiso/import.h"
#include "utils/xdelta3/xdelta3_wrapper.h"
#include "utils/json.hpp"
#include "utils/binfile.h"

#include <wx/wx.h>

#include <span>
#include <vector>
#include <map>
#include <filesystem>
#include <format>

// clang-format off

const std::vector<uint8_t> GAME_ID_KOREA{
    // "SLPM_675.13"
    0x53, 0x4c, 0x50, 0x4d, 0x5f, 0x36, 0x37, 0x35, 0x2e, 0x31, 0x33,
};

const std::vector<uint8_t> GAME_ID_JAPAN{
    // "SLPS_250.50"
    0x53, 0x4c, 0x50, 0x53, 0x5f, 0x32, 0x35, 0x30, 0x2e, 0x35, 0x30,
};

const std::vector<uint8_t> PATCH_CHECK_DATA{
    0x5f, 0xaf, 0x18, 0x00, 0x60, 0xfc, 0x00, 0x00,
};

const std::map<int, int> MOVIE_DATA{
	// mov
	{16163, 15778}, {16165, 15780}, {16167, 15782}, {16169, 15784},
	{16171, 15786}, {16179, 15794}, {16183, 15798}, {16185, 15800},
	{16187, 15802}, {16189, 15804}, {16191, 15806}, {16193, 15808},
	{16195, 15810}, {16197, 15812}, {16199, 15814}, {16205, 15820},
	{16207, 15822}, {16209, 15824}, {16211, 15826}, {16213, 15828},
	{16215, 15830}, {16217, 15832}, {16219, 15834}, {16221, 15836},
	{16223, 15838}, {16225, 15840}, {16227, 15842}, {16229, 15844},
	{16231, 15846}, {16233, 15848}, {16235, 15850}, {16237, 15852},
	{16239, 15854}, {16243, 15858}, {16245, 15860}, {16247, 15862},
	{16249, 15864}, {16253, 15868}, {16257, 15872}, {16259, 15874},
	{16261, 15876}, {16263, 15878}, {16265, 15880}, {16284, 15899},
	{16286, 15901}, {16288, 15903}, {16290, 15905}, {16292, 15907},
	{16294, 15909}, {16296, 15911}, {16298, 15913}, {16300, 15915},
	{16302, 15917},
};

// clang-format on

const std::filesystem::path &temp_dir("~ffxkr");

bool isValidPath(const wxString &wxstr)
{
	std::wstring wstr = wxstr.ToStdWstring();
	std::string str = wxstr.ToStdString();
	wxString restored = wxString::From8BitData(str.c_str());
	return restored == wxstr;
}

void ffx::MainFrame::OnTypeRadioBox(wxCommandEvent &event)
{
	int selected = m_typeRadioBox->GetSelection();

	if (selected == 1)
	{
		m_jpnVoiceOptionCheckBox->Disable();
		m_jpnIsoWarnText->Disable();
		m_jpnIsoWarn2Text->Disable();
	}
	else
	{
		m_jpnVoiceOptionCheckBox->Enable();
		m_jpnIsoWarnText->Enable();
		m_jpnIsoWarn2Text->Enable();
	}
}

void ffx::MainFrame::OnOpenBaseIsoButtonClick(wxCommandEvent &event)
{
	wxFileDialog openDlg(this, wxT("파이널 판타지 10 인터내셔널 ISO 열기..."), "", "", "iso 파일|*.iso", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openDlg.ShowModal() == wxID_CANCEL)
		return;

	const auto &path = openDlg.GetPath().ToStdString();
	if (!isValidPath(openDlg.GetPath()))
	{
		wxMessageBox(wxT("올바르지 않은 경로입니다.\n한글, 특수 문자를 제외하여 주십시오."), wxT("오류"), wxICON_ERROR);
		return;
	}

	if (!binfile::has_bytes(path, 0x82881, GAME_ID_KOREA))
	{
		wxMessageBox(wxT("파이널 판타지 10 인터내셔널 ISO 파일이 아닙니다!"), wxT("오류"), wxICON_ERROR);
		return;
	}

	const auto &unusedAlbhedFile = ffxiso::get_file_bytes(path, 460);
	if (!binfile::has_bytes(unusedAlbhedFile, 0, PATCH_CHECK_DATA))
	{
		wxMessageBox(wxT("이미 패치되어 있거나, 이전 버전이 패치되어 있습니다.\n원본 ISO 파일이 필요합니다!"), wxT("오류"), wxICON_ERROR);
		return;
	}

	m_baseIsoText->SetValue(path);
}

void ffx::MainFrame::OnJpnVoiceOptionCheckBox(wxCommandEvent &event)
{
	if (m_jpnVoiceOptionCheckBox->IsChecked())
	{
		m_jpnIsoText->Enable();
		m_openJpnIsoButton->Enable();
	}
	else
	{
		m_jpnIsoText->Disable();
		m_openJpnIsoButton->Disable();
	}
}

void ffx::MainFrame::OnOpenJpnIsoButtonClick(wxCommandEvent &event)
{
	wxFileDialog openDlg(this, wxT("파이널 판타지 10 오리지널 ISO 열기..."), "", "", "iso 파일|*.iso", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openDlg.ShowModal() == wxID_CANCEL)
		return;

	const auto &path = openDlg.GetPath().ToStdString();
	if (!isValidPath(openDlg.GetPath()))
	{
		wxMessageBox(wxT("올바르지 않은 경로입니다.\n한글, 특수 문자를 제외하여 주십시오."), wxT("오류"), wxICON_ERROR);
		return;
	}

	if (!binfile::has_bytes(path, 0x82881, GAME_ID_JAPAN))
	{
		wxMessageBox(wxT("파이널 판타지 10 오리지널 ISO 파일이 아닙니다!"), wxT("오류"), wxICON_ERROR);
		return;
	}

	m_jpnIsoText->SetValue(path);
}

void ffx::MainFrame::OnApplyButtonClick(wxCommandEvent &event)
{
	if (m_applyButton->GetLabel() == wxT("취소"))
	{
		m_applyButton->Disable();

		if (applyPatchThread)
		{
			applyPatchThread->Delete();

			// 종료 대기...
			while (true)
			{
				{
					wxCriticalSectionLocker enter(applyPatchThreadLock);
					if (applyPatchThread == nullptr)
						break;
				}

				wxThread::This()->Sleep(1);
			}
		}
	}
	else
	{
		m_applyButton->SetLabel(wxT("취소"));

		applyPatchThread = new ApplyPatchThread(this);
		applyPatchThread->basePath = m_baseIsoText->GetValue().ToStdString();
		applyPatchThread->jpnPath = m_jpnIsoText->GetValue().ToStdString();
		applyPatchThread->selectedVersion = m_typeRadioBox->GetSelection();
		applyPatchThread->jpnVoiceEnabled = m_jpnVoiceOptionCheckBox->IsChecked();
		applyPatchThread->Run();
	}
}

void ffx::MainFrame::OnFrameClose(wxCloseEvent &event)
{
	if (applyPatchThread != nullptr && !applyPatchThread->TestDestroy())
	{
		wxMessageBox(wxT("패치가 진행 중입니다!"), wxT("알림"), wxICON_ERROR);
		event.Veto();
		return;
	}

	event.Skip();
}

void ffx::MainFrame::OnThreadUpdate(wxThreadEvent &event)
{
	const auto &what = event.GetPayload<ApplyPatchThreadEvent>();

	if (what == ApplyPatchThreadEvent::STARTUP)
	{

		bool useJpnVoice = m_typeRadioBox->GetSelection() == 0 && m_jpnVoiceOptionCheckBox->IsChecked();
		int range = /* 언패킹 */ 1 +
					/* 패치 */ 1 +
					/* 음성 교체 시작 */ (useJpnVoice ? 1 : 0) +
					/* 음성 교체 */ (useJpnVoice ? 85 : 0) +
					/* 영상 교체 */ (useJpnVoice ? MOVIE_DATA.size() : 0) +
					/* 리패킹 */ 1 +
					/* 파일 교체 */ 1;
		m_applyGauge->SetRange(range);

		m_applyGauge->SetValue(0);
		m_applyPathProgressText->SetLabel(wxT("준비"));
		m_workspacePanel->Disable();
		return;
	}

	if (what == ApplyPatchThreadEvent::CLEANUP)
	{
		m_applyGauge->SetValue(0);
		m_applyPathProgressText->SetLabel(wxT("준비"));
		m_applyButton->SetLabel(wxT("패치"));
		m_applyButton->Enable();
		m_workspacePanel->Enable();
		return;
	}

	if (what == ApplyPatchThreadEvent::MESSAGE_BOX)
	{
		const auto &message = event.GetString();
		wxMessageBox(message, wxT("알림"), wxICON_INFORMATION);
		return;
	}

	if (what == ApplyPatchThreadEvent::ERROR_MESSAGE_BOX)
	{
		const auto &message = event.GetString();
		wxMessageBox(message, wxT("오류"), wxICON_ERROR);
		return;
	}

	if (what == ApplyPatchThreadEvent::UPDATE_GAUGE)
	{
		const int value = event.GetInt();
		m_applyGauge->SetValue(value);

		const auto &message = event.GetString();
		m_applyPathProgressText->SetLabel(message);
		return;
	}
}

ffx::ApplyPatchThread::~ApplyPatchThread()
{
	wxCriticalSectionLocker enter(m_pHandler->applyPatchThreadLock);
	m_pHandler->applyPatchThread = nullptr;
}

wxThread::ExitCode ffx::ApplyPatchThread::Entry()
{
	int progress = 0;

	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::STARTUP);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());

	if (!std::filesystem::exists(basePath))
	{
		ShowErrorMessageBox(wxT("지정된 경로에 파이널 판타지 10 인터내셔널 ISO 파일이 없습니다."));
		Cleanup(false);
		return 0;
	}

	if (!binfile::has_bytes(basePath, 0x82881, GAME_ID_KOREA))
	{
		ShowErrorMessageBox(wxT("파이널 판타지 10 인터내셔널 ISO 파일이 아닙니다!"));
		Cleanup(false);
		return 0;
	}

	const auto &unusedAlbhedFile = ffxiso::get_file_bytes(basePath, 460);
	if (!binfile::has_bytes(unusedAlbhedFile, 0, PATCH_CHECK_DATA))
	{
		ShowErrorMessageBox(wxT("이미 패치되어 있거나, 이전 버전이 패치되어 있습니다.\n원본 ISO 파일이 필요합니다!"));
		Cleanup(false);
		return 0;
	}

	if (selectedVersion == 0 && jpnVoiceEnabled)
	{
		if (!std::filesystem::exists(jpnPath))
		{
			ShowErrorMessageBox(wxT("지정된 경로에 파이널 판타지 10 오리지널 ISO 파일이 없습니다."));
			Cleanup(false);
			return 0;
		}

		if (!binfile::has_bytes(jpnPath, 0x82881, GAME_ID_JAPAN))
		{
			ShowErrorMessageBox(wxT("파이널 판타지 10 오리지널 ISO 파일이 아닙니다!"));
			Cleanup(false);
			return 0;
		}
	}

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	UpdateGauge(++progress, wxT("패치 작업 폴더 생성..."));
	std::filesystem::remove_all(temp_dir);
	std::filesystem::create_directory(temp_dir);

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// xdelta 패치
	UpdateGauge(++progress, wxT("패치 데이터 적용..."));
	const auto &xdeltaPath = temp_dir / "patch_data";
	const std::span<const uint8_t> bytes(
		selectedVersion == 0 ? ffx::korean_xdelta_bytes : ffx::multilang_xdelta_bytes,
		selectedVersion == 0 ? ffx::korean_xdelta_bytes_size : ffx::multilang_xdelta_bytes_size);
	binfile::write_byte_to_file(xdeltaPath, bytes);

	const auto &tempPatchPath = temp_dir / "temp_iso";
	xd3_main_exec({"-d", "-n", "-s", basePath.string(), xdeltaPath.string(), tempPatchPath.string()});

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// 일어 음성 패치
	UpdateGauge(++progress, wxT("일어 음성 교체..."));
	if (selectedVersion == 0 && jpnVoiceEnabled)
	{
		const auto &workspacePath = temp_dir / "work";
		ffxiso::extract(tempPatchPath, workspacePath);
		if (std::filesystem::exists(tempPatchPath))
			std::filesystem::remove(tempPatchPath);

		// 음성 파일 교체
		for (int i = 17; i <= 101; i++)
		{
			if (TestDestroy())
			{
				Cleanup();
				return 0;
			}

			UpdateGauge(++progress, wxString::Format(wxT("일어 음성 교체... (%d/%d)"), i - 16, 101 - 16));

			const auto &ext = i == 17 ? ".bin" : ".vs";
			const auto &newFilePath = workspacePath / "files" / std::format("file_{:05}{}", i + 1, ext);
			if (std::filesystem::exists(newFilePath))
				std::filesystem::remove(newFilePath);

			const auto &bytes = ffxiso::get_file_bytes(jpnPath, i);
			if (bytes.size() > 0)
				binfile::write_byte_to_file(newFilePath, bytes);
		}

		std::ifstream json_file(workspacePath / "files.json");
		nlohmann::json files_json = nlohmann::json::parse(json_file);

		// 영상 음성 교체
		int replaceCnt = 0;
		for (const auto &[inter_idx, jpn_idx] : MOVIE_DATA)
		{
			if (TestDestroy())
			{
				Cleanup();
				return 0;
			}

			UpdateGauge(++progress, wxString::Format(wxT("영상 교체... (%d/%d)"), ++replaceCnt, static_cast<int>(MOVIE_DATA.size())));

			auto movFilePath = workspacePath / "files" / files_json[inter_idx]["filename"].get<std::string>();
			if (std::filesystem::exists(movFilePath))
				std::filesystem::remove(movFilePath);

			const auto &movBytes = ffxiso::get_file_bytes(jpnPath, jpn_idx);
			if (movBytes.size() > 0)
				binfile::write_byte_to_file(movFilePath, movBytes);

			// 다음 인덱스의 파일은 영상 데이터

			auto movDataFilePath = workspacePath / "files" / files_json[inter_idx + 1]["filename"].get<std::string>();
			if (std::filesystem::exists(movDataFilePath))
				std::filesystem::remove(movDataFilePath);

			const auto &moveDataBytes = ffxiso::get_file_bytes(jpnPath, jpn_idx + 1);
			if (moveDataBytes.size() > 0)
				binfile::write_byte_to_file(movDataFilePath, moveDataBytes);
		}

		if (TestDestroy())
		{
			Cleanup();
			return 0;
		}

		UpdateGauge(++progress, wxT("ISO 생성 중..."));
		ffxiso::import(workspacePath, tempPatchPath);
	}

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// 완성 ISO 교체
	UpdateGauge(++progress, wxT("ISO 복사 중..."));
	if (std::filesystem::exists(basePath))
		std::filesystem::remove(basePath);
	std::filesystem::rename(tempPatchPath, basePath);

	// MEMO: 패치가 완료되어 취소가 필요 없음
	// if (TestDestroy())
	// {
	// 	Cleanup();
	// 	return 0;
	// }

	Cleanup(false);
	ShowMessageBox(wxT("패치가 완료되었습니다!"));

	return 0;
}

void ffx::ApplyPatchThread::UpdateGauge(int value, const wxString &message)
{
	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::UPDATE_GAUGE);
	evt.SetInt(value);
	evt.SetString(message);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}

void ffx::ApplyPatchThread::ShowMessageBox(const wxString &message)
{
	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::MESSAGE_BOX);
	evt.SetString(message);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}

void ffx::ApplyPatchThread::ShowErrorMessageBox(const wxString &message)
{
	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::ERROR_MESSAGE_BOX);
	evt.SetString(message);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}

void ffx::ApplyPatchThread::Cleanup(bool isCancel)
{
	if (std::filesystem::exists(temp_dir))
		std::filesystem::remove_all(temp_dir);

	if (isCancel)
		ShowMessageBox(wxT("패치가 취소되었습니다."));

	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::CLEANUP);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}
