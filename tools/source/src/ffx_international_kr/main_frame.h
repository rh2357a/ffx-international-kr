#ifndef _FFX_MAIN_FRAME_H_
#define _FFX_MAIN_FRAME_H_

#include <wx/wx.h>

#include <filesystem>

#include "gui_base.h"

// NOTE: `std::thread`로 변경 필요
// https://docs.wxwidgets.org/3.2/classwx_thread.html

namespace ffx
{
    class ApplyPatchThread;

    class MainFrame : public MainFrameBase
    {
    public:
        ApplyPatchThread *applyPatchThread = nullptr;
        wxCriticalSection applyPatchThreadLock;

    public:
        MainFrame(wxWindow *parent);

    private:
        void OnThreadUpdate(wxThreadEvent &event);

    private:
        void OnFrameClose(wxCloseEvent &event) override;
        void OnOpenBaseIsoButtonClick(wxCommandEvent &event) override;
        void OnTargetIsoButtonClick(wxCommandEvent &event) override;
        void OnJpnVoiceCheckBox(wxCommandEvent &event) override;
        void OnOpenJpnIsoButtonClick(wxCommandEvent &event) override;
        void OnApplyButtonClick(wxCommandEvent &event) override;
    };

    class ApplyPatchThread : public wxThread
    {
    private:
        MainFrame *m_pHandler;

    public:
        std::filesystem::path basePath, targetPath, jpnPath;
        bool isJpnVoiceEnabled = false;

    public:
        ApplyPatchThread(MainFrame *handler)
            : wxThread(wxTHREAD_DETACHED),
              m_pHandler(handler) {}
        ~ApplyPatchThread();

    protected:
        ExitCode Entry() override;

    private:
        void UpdateGauge(int value, const wxString &message);
        void ShowMessageBox(const wxString &message);
        void ShowErrorMessageBox(const wxString &message);
        void Cleanup(bool isCancel = true);
    };

    enum ApplyPatchThreadEvent
    {
        UPDATE_GAUGE,
        MESSAGE_BOX,
        ERROR_MESSAGE_BOX,
        STARTUP,
        CLEANUP,
    };
}

#endif
