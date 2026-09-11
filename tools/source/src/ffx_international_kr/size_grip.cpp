#include "size_grip.h"

#include <wx/dcclient.h>
#include <wx/settings.h>
#include <wx/toplevel.h>

#ifdef __WXMSW__
#include <wx/msw/wrapwin.h>

#include <uxtheme.h>
#include <vssym32.h>
#endif

namespace ffx
{
    SizeGrip::SizeGrip(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint &pos,
        const wxSize &size,
        long style)
        : wxWindow(parent, id, pos, size, style | wxBORDER_NONE)
    {
        SetBackgroundColour(parent->GetBackgroundColour());
        SetMinClientSize(GetGripSize());
        SetCursor(wxCursor(wxCURSOR_SIZENWSE));

        Bind(wxEVT_PAINT, &SizeGrip::OnPaint, this);
        Bind(wxEVT_LEFT_DOWN, &SizeGrip::OnLeftDown, this);
        Bind(wxEVT_SYS_COLOUR_CHANGED, &SizeGrip::OnSystemColourChanged, this);
    }

    wxSize SizeGrip::DoGetBestClientSize() const
    {
        return GetGripSize();
    }

    wxSize SizeGrip::GetGripSize() const
    {
#ifdef __WXMSW__
        return {
            ::GetSystemMetrics(SM_CXVSCROLL),
            ::GetSystemMetrics(SM_CYHSCROLL),
        };
#else
        return FromDIP(wxSize(17, 17));
#endif
    }

    void SizeGrip::OnPaint(wxPaintEvent &)
    {
        wxPaintDC dc(this);
        dc.SetBackground(wxBrush(GetBackgroundColour()));
        dc.Clear();

#ifdef __WXMSW__
        const auto size = GetClientSize();
        RECT rect{0, 0, size.x, size.y};
        const auto hwnd = reinterpret_cast<HWND>(GetHandle());
        const auto hdc = reinterpret_cast<HDC>(dc.GetHDC());
        const auto theme = ::OpenThemeData(hwnd, L"Status");

        if (theme)
        {
            if (::IsThemeBackgroundPartiallyTransparent(theme, SP_GRIPPER, 0))
                ::DrawThemeParentBackground(hwnd, hdc, &rect);
            ::DrawThemeBackground(theme, hdc, SP_GRIPPER, 0, &rect, nullptr);
            ::CloseThemeData(theme);
        }
        else
        {
            ::DrawFrameControl(hdc, &rect, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
        }
#else
        const auto size = GetClientSize();
        dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW)));
        for (int inset = 3; inset <= 9; inset += 3)
            dc.DrawLine(size.x - inset, size.y - 1, size.x - 1, size.y - inset);
#endif
    }

    void SizeGrip::OnLeftDown(wxMouseEvent &event)
    {
#ifdef __WXMSW__
        auto *topLevel = wxDynamicCast(wxGetTopLevelParent(this), wxTopLevelWindow);
        if (topLevel && !topLevel->IsMaximized())
        {
            POINT cursor{};
            ::GetCursorPos(&cursor);
            ::ReleaseCapture();
            ::SendMessageW(
                reinterpret_cast<HWND>(topLevel->GetHandle()),
                WM_NCLBUTTONDOWN,
                HTBOTTOMRIGHT,
                MAKELPARAM(cursor.x, cursor.y));
            return;
        }
#endif
        event.Skip();
    }

    void SizeGrip::OnSystemColourChanged(wxSysColourChangedEvent &event)
    {
        if (const auto *parent = GetParent())
            SetBackgroundColour(parent->GetBackgroundColour());
        Refresh();
        event.Skip();
    }
}
