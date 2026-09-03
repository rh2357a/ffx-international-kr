#include "main_frame.h"

#include <ffxiso/extract.h>
#include <ffxiso/import.h>
#include <utils/binfile.h>
#include <utils/json.hpp>
#include <wx/wx.h>
#include <xdelta3_wrapper.h>

#include <filesystem>
#include <format>
#include <vector>

#include "embed.h"
#include "japanese_voice.h"
#include "resources.h"

// clang-format off

const std::filesystem::path TEMP_DIR("~ffxkr");

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

ffx::MainFrame::MainFrame(wxWindow *parent) : MainFrameBase(parent)
{
    SetTitle(wxT("PS2 파이널 판타지 10 인터내셔널 한국어 패치 (일본어 음성) " APP_VERSION));
    Bind(wxEVT_THREAD, &MainFrame::OnThreadUpdate, this);
}

void ffx::MainFrame::OnOpenBaseIsoButtonClick(wxCommandEvent &event)
{
    wxFileDialog openDlg(this, wxT("파이널 판타지 10 인터내셔널 ISO 열기..."), "", "", wxT("ISO 파일|*.iso"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDlg.ShowModal() == wxID_CANCEL)
        return;

    std::string path = openDlg.GetPath().utf8_string();

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

    m_baseIsoText->SetValue(wxString::FromUTF8(path));
}

void ffx::MainFrame::OnOpenJpnIsoButtonClick(wxCommandEvent &event)
{
    wxFileDialog openDlg(this, wxT("파이널 판타지 10 오리지널 ISO 열기..."), "", "", wxT("ISO 파일|*.iso"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (openDlg.ShowModal() == wxID_CANCEL)
        return;

    std::string path = openDlg.GetPath().utf8_string();

    if (!binfile::has_bytes(path, 0x82881, GAME_ID_JAPAN))
    {
        wxMessageBox(wxT("파이널 판타지 10 오리지널 ISO 파일이 아닙니다!"), wxT("오류"), wxICON_ERROR);
        return;
    }

    m_jpnIsoText->SetValue(wxString::FromUTF8(path));
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
                wxCriticalSectionLocker enter(applyPatchThreadLock);
                if (applyPatchThread == nullptr)
                    break;

                wxThread::This()->Sleep(1);
            }
        }
    }
    else
    {
        m_applyButton->SetLabel(wxT("취소"));

        applyPatchThread = new ApplyPatchThread(this);
        applyPatchThread->basePath = m_baseIsoText->GetValue().utf8_string();
        applyPatchThread->jpnPath = m_jpnIsoText->GetValue().utf8_string();
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
        int range = /* 언패킹 */ 1 +
                    /* 패치 */ 1 +
                    /* 음성 교체 시작 */ 1 +
                    /* 음성 교체 */ 86 +
                    /* 영상 교체 */ MOVIE_DATA.size() +
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
        const auto message = event.GetString();
        wxMessageBox(message, wxT("알림"), wxICON_INFORMATION);
        return;
    }

    if (what == ApplyPatchThreadEvent::ERROR_MESSAGE_BOX)
    {
        const auto message = event.GetString();
        wxMessageBox(message, wxT("오류"), wxICON_ERROR);
        return;
    }

    if (what == ApplyPatchThreadEvent::UPDATE_GAUGE)
    {
        const int value = event.GetInt();
        m_applyGauge->SetValue(value);

        const auto message = event.GetString();
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

    if (TestDestroy())
    {
        Cleanup();
        return 0;
    }

    UpdateGauge(++progress, wxT("패치 작업 폴더 생성..."));
    std::filesystem::remove_all(TEMP_DIR);
    std::filesystem::create_directory(TEMP_DIR);

    if (TestDestroy())
    {
        Cleanup();
        return 0;
    }

    // xdelta 패치
    UpdateGauge(++progress, wxT("패치 데이터 적용..."));
    const auto xdeltaPath = TEMP_DIR / "patch_data";
    binfile::write_byte_to_file(xdeltaPath, embed::kJpVoicePatchData);

    const auto tempBasePath = TEMP_DIR / "temp_iso";
    std::filesystem::copy(basePath, tempBasePath);

    const auto tempPatchPath = TEMP_DIR / "temp_patch_iso";
    xd3_main_exec({"-d", "-f", "-n", "-s", tempBasePath.string(), xdeltaPath.string(), tempPatchPath.string()});
    std::filesystem::remove(tempBasePath);

    if (TestDestroy())
    {
        Cleanup();
        return 0;
    }

    // 일어 음성 패치
    UpdateGauge(++progress, wxT("일어 음성 교체..."));
    const auto workspacePath = TEMP_DIR / "work";
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

        const auto ext = i == 17 ? ".bin" : ".vs";
        const auto newFilePath = workspacePath / "files" / std::format("file_{:05}{}", i + 1, ext);
        if (std::filesystem::exists(newFilePath))
            std::filesystem::remove(newFilePath);

        const auto &bytes = ffxiso::get_file_bytes(jpnPath, i);
        if (bytes.size() > 0)
            binfile::write_byte_to_file(newFilePath, bytes);
    }

    // 기타 음성 교체
    {
        const auto bytes = ffxiso::get_file_bytes(jpnPath, 479);
        const auto newFilePath = workspacePath / "files" / "file_00556.bin";
        std::filesystem::remove(newFilePath);
        binfile::write_byte_to_file(newFilePath, bytes);
    }

    std::ifstream json_file(workspacePath / "files.json");
    nlohmann::json files_json = nlohmann::json::parse(json_file);

    if (TestDestroy())
    {
        Cleanup();
        return 0;
    }

    UpdateGauge(++progress, wxT("일어 음성 교체... (9514)"));
    {
        const auto voiceFilePath = workspacePath / "files" / files_json.at(9514).at("filename").get<std::string>();
        const auto original = binfile::read_all_bytes(voiceFilePath);
        const auto japanese = ffxiso::get_file_bytes(jpnPath, 9193);
        const auto patched = japanese_voice::replace_9514(original, japanese);

        std::ofstream voiceFile;
        voiceFile.exceptions(std::ios::failbit | std::ios::badbit);
        voiceFile.open(voiceFilePath, std::ios::binary | std::ios::trunc);
        voiceFile.write(reinterpret_cast<const char *>(patched.data()),
            static_cast<std::streamsize>(patched.size()));
        voiceFile.close();
    }

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

    json_file.close();
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
    if (std::filesystem::exists(TEMP_DIR))
        std::filesystem::remove_all(TEMP_DIR);

    if (isCancel)
        ShowMessageBox(wxT("패치가 취소되었습니다."));

    wxThreadEvent evt(wxEVT_THREAD);
    evt.SetPayload<ApplyPatchThreadEvent>(ApplyPatchThreadEvent::CLEANUP);
    wxQueueEvent(m_pHandler->GetEventHandler(), evt.Clone());
}
