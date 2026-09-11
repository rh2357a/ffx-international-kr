#pragma once

#include <wx/window.h>

namespace ffx
{
    class SizeGrip : public wxWindow
    {
    public:
        SizeGrip(
            wxWindow *parent,
            wxWindowID id = wxID_ANY,
            const wxPoint &pos = wxDefaultPosition,
            const wxSize &size = wxDefaultSize,
            long style = 0);

    protected:
        wxSize DoGetBestClientSize() const override;

    private:
        wxSize GetGripSize() const;
        void OnPaint(wxPaintEvent &event);
        void OnLeftDown(wxMouseEvent &event);
        void OnSystemColourChanged(wxSysColourChangedEvent &event);
    };
}
