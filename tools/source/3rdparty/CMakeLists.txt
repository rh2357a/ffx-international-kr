set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# cmrc
# https://github.com/vector-of-bool/cmrc
# 952ffddba731fc110bd50409e8d2b8a06abbd237
# MIT license
block()
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmrc")
    include(CMakeRC)
endblock()

# wxWidgets
# https://github.com/wxWidgets/wxWidgets
# v3.3.3 (670835aec7e54b9e9d5a50d6a1aea1e8ba0bcdb2)
# LGPL-2.0-or-later WITH WxWindows-exception-3.1
block()
    set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(wxUSE_UNICODE ON CACHE BOOL "" FORCE)
    set(wxUSE_RICHTEXT OFF CACHE BOOL "" FORCE)
    set(wxUSE_HTML OFF CACHE BOOL "" FORCE)
    set(wxUSE_WXHTML_HELP OFF CACHE BOOL "" FORCE)
    set(wxUSE_STC OFF CACHE BOOL "" FORCE)
    set(wxUSE_XRC OFF CACHE BOOL "" FORCE)
    set(wxUSE_WEBVIEW OFF CACHE BOOL "" FORCE)
    set(wxUSE_MEDIACTRL OFF CACHE BOOL "" FORCE)
    set(wxUSE_AUI OFF CACHE BOOL "" FORCE)
    set(wxUSE_RIBBON OFF CACHE BOOL "" FORCE)
    add_subdirectory(wxWidgets)
endblock()

# xdelta
# https://github.com/marco-calautti/xdelta
# ab959ecd9a471a0dec5babc3de9d1e27ca25ffa8
# Apache License 2.0
block()
    set(XD3_ENABLE_SECONDARY_COMPRESSION ON CACHE BOOL "" FORCE)
    set(XD3_ENABLE_ENCODER ON CACHE BOOL "" FORCE)
    set(XD3_ENABLE_VCDIFF_TOOLS ON CACHE BOOL "" FORCE)
    set(XD3_ENABLE_LZMA OFF CACHE BOOL "" FORCE)
    set(XD3_BUILD_AS_EXE OFF CACHE BOOL "" FORCE)
    add_subdirectory(xdelta)
endblock()
