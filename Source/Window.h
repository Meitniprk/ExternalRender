//
// Created by ManTkai on 2023/2/4.
//
#ifndef EXTERNALRENDER_WINDOW_H
#define EXTERNALRENDER_WINDOW_H

#include <windows.h>
#include <string_view>
#include <mutex>

#include "rapidjson/document.h"

#define WINDOW_CLASS_NAME "Qt5152QWindowIcon"

namespace Window {

    struct wnd_pec : public RECT {
        int width() {
            return right - left;
        }

        int height() {
            return bottom - top;
        }
    };

    extern Window::wnd_pec target_rect;
    extern Window::wnd_pec overlay_rect;

    extern rapidjson::Document document;

    void CreateExternalWindow(std::mutex &documentLock);
}

#endif //EXTERNALRENDER_WINDOW_H
