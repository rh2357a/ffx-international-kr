#include "main_frame.h"

#include "patch_data.h"

#include "ffxiso/extract.h"
#include "ffxiso/import.h"
#include "utils/xdelta3/xdelta3_wrapper.h"
#include "utils/binfile.h"

#include <wx/wx.h>

#include <span>
#include <filesystem>
#include <format>

// clang-format off

const std::vector<uint8_t> game_id_korea{
    // "SLPM_675.13"
    0x53, 0x4c, 0x50, 0x4d, 0x5f, 0x36, 0x37, 0x35, 0x2e, 0x31, 0x33,
};

const std::vector<uint8_t> game_id_japan{
    // "SLPS_250.50"
    0x53, 0x4c, 0x50, 0x53, 0x5f, 0x32, 0x35, 0x30, 0x2e, 0x35, 0x30,
};

// clang-format on

const std::filesystem::path &temp_dir("~ffxkr");

void ffx::MainFrame::OnTypeRadioBox(wxCommandEvent &event)
{
	int selected = m_typeRadioBox->GetSelection();

	if (selected == 1)
	{
		m_jpnVoiceOptionCheckBox->Disable();
		m_openJpnIsoButton->Disable();
		m_jpnIsoWarnText->Disable();
		m_jpnIsoWarn2Text->Disable();
	}
	else
	{
		m_jpnVoiceOptionCheckBox->Enable();
		m_openJpnIsoButton->Enable();
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
	if (!binfile::has_bytes(path, 0x082881, game_id_korea))
	{
		wxMessageBox(wxT("파이널 판타지 10 인터내셔널 ISO이 아닙니다!"), wxT("오류"), wxICON_ERROR);
		return;
	}

	m_baseIsoText->SetValue(path);
}

void ffx::MainFrame::OnJpnVoiceOptionCheckBox(wxCommandEvent &event)
{
	if (m_jpnVoiceOptionCheckBox->IsChecked())
		m_jpnVoiceIsoPanel->Enable();
	else
		m_jpnVoiceIsoPanel->Disable();
}

void ffx::MainFrame::OnOpenJpnIsoButtonClick(wxCommandEvent &event)
{
	wxFileDialog openDlg(this, wxT("파이널 판타지 10 오리지널 ISO 열기..."), "", "", "iso 파일|*.iso", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openDlg.ShowModal() == wxID_CANCEL)
		return;

	const auto &path = openDlg.GetPath().ToStdString();
	if (!binfile::has_bytes(path, 0x082881, game_id_japan))
	{
		wxMessageBox(wxT("파이널 판타지 10 오리지널 ISO이 아닙니다!"), wxT("오류"), wxICON_ERROR);
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
		m_applyGauge->SetValue(0);
		m_workspacePanel->Disable();
		return;
	}

	if (what == ApplyPatchThreadEvent::CLEANUP)
	{
		m_applyGauge->SetValue(0);
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
	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::STARTUP);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());

	if (!std::filesystem::exists(basePath))
	{
		ShowErrorMessageBox(wxT("지정된 경로에 파이널 판타지 10 인터내셔널 ISO 파일이 없습니다."));
		Cleanup();
		return 0;
	}

	if (!binfile::has_bytes(basePath, 0x82881, game_id_korea))
	{
		ShowErrorMessageBox(wxT("파이널 판타지 10 인터내셔널 ISO이 아닙니다!"));
		Cleanup();
		return 0;
	}

	if (selectedVersion == 0 && jpnVoiceEnabled)
	{
		if (!std::filesystem::exists(jpnPath))
		{
			ShowErrorMessageBox(wxT("지정된 경로에 파이널 판타지 10 오리지널 ISO 파일이 없습니다."));
			Cleanup();
			return 0;
		}

		if (!binfile::has_bytes(jpnPath, 0x82881, game_id_japan))
		{
			ShowErrorMessageBox(wxT("파이널 판타지 10 오리지널 ISO이 아닙니다!"));
			Cleanup();
			return 0;
		}
	}

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	UpdateGauge(1);
	std::filesystem::remove_all(temp_dir);
	std::filesystem::create_directory(temp_dir);

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// xdelta 패치
	UpdateGauge(2);
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
	UpdateGauge(3);
	if (selectedVersion == 0 && jpnVoiceEnabled)
	{
		const auto &workspacePath = temp_dir / "work";
		ffxiso::extract(tempPatchPath, workspacePath);
		if (std::filesystem::exists(tempPatchPath))
			std::filesystem::remove(tempPatchPath);

		for (int i = 17; i <= 101; i++)
		{
			if (TestDestroy())
			{
				Cleanup();
				return 0;
			}

			UpdateGauge(3 + i - 16);
			const auto &ext = i == 17 ? ".bin" : ".vs";
			const auto &newFilePath = workspacePath / "files" / std::format("file_{:05}{}", i + 1, ext);
			if (std::filesystem::exists(newFilePath))
				std::filesystem::remove(newFilePath);
			const auto &bytes = ffxiso::get_file_bytes(jpnPath, i);
			if (bytes.size() > 0)
				binfile::write_byte_to_file(newFilePath, bytes);
		}

		if (TestDestroy())
		{
			Cleanup();
			return 0;
		}
		UpdateGauge(85);
		ffxiso::import(workspacePath, tempPatchPath);
	}

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// 완성 ISO 교체
	UpdateGauge(86);
	if (std::filesystem::exists(basePath))
		std::filesystem::remove(basePath);
	std::filesystem::rename(tempPatchPath, basePath);

	if (TestDestroy())
	{
		Cleanup();
		return 0;
	}

	// 임시 폴더 제거
	UpdateGauge(87);
	std::filesystem::remove_all(temp_dir);

	Cleanup();
	ShowMessageBox(wxT("패치가 완료되었습니다!"));

	return 0;
}

void ffx::ApplyPatchThread::UpdateGauge(int value)
{
	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::UPDATE_GAUGE);
	evt.SetInt(value);
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

void ffx::ApplyPatchThread::Cleanup()
{
	if (std::filesystem::exists(temp_dir))
		std::filesystem::remove_all(temp_dir);

	wxThreadEvent evt(wxEVT_THREAD);
	evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::CLEANUP);
	wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}
