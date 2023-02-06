//
// Created by ManTkai on 2023/2/4.
//
#include <dwmapi.h>

#include "Window.h"
#include "Render.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

namespace Window {
    Window::wnd_pec target_rect;
    Window::wnd_pec overlay_rect;

    rapidjson::Document document;

    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

        switch (msg) {
            case WM_SIZE:
                if (Render::pd3dDevice != nullptr && wParam != SIZE_MINIMIZED) {
                    Render::CleanupRenderTarget();
                    Render::pSwapChain->ResizeBuffers(0, (UINT) LOWORD(lParam), (UINT) HIWORD(lParam),
                                                      DXGI_FORMAT_UNKNOWN, 0);
                    Render::CreateRenderTarget();
                }
                return 0;
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
            case WM_DESTROY:
                ::PostQuitMessage(0);
                return 0;
            default:
                return ::DefWindowProc(hWnd, msg, wParam, lParam);
        }
    }

    void CreateExternalWindow(std::mutex &documentLock) {
        ImGui_ImplWin32_EnableDpiAwareness();
        HWND targetWindowHwnd = FindWindowA(WINDOW_CLASS_NAME, nullptr);
        if (targetWindowHwnd == nullptr) {
            printf("No Window ClassName=%s\n", WINDOW_CLASS_NAME);
            return;
        }
        GetWindowRect(targetWindowHwnd, &target_rect);
        printf("targetWindow:height=%d width=%d\n", target_rect.height(), target_rect.width());

        WNDCLASSEX wc = {
                sizeof(WNDCLASSEX),
                CS_CLASSDC,
                WndProc,
                0L,
                0L,
                GetModuleHandle(nullptr),
                nullptr,
                nullptr,
                nullptr,
                nullptr,
                L" ",
                nullptr};
        ::RegisterClassEx(&wc);

        HWND overlayWindowHwnd = CreateWindowEx(
                WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
                wc.lpszClassName,
                L" ",
                WS_POPUP | WS_VISIBLE,
                Window::target_rect.left,
                Window::target_rect.top,
                Window::target_rect.width(),
                Window::target_rect.height(),
                nullptr,
                nullptr,
                GetModuleHandle(nullptr),
                nullptr);

        GetWindowRect(overlayWindowHwnd, &overlay_rect);

        MARGINS m = {Window::overlay_rect.left, Window::overlay_rect.top, Window::overlay_rect.width(),
                     Window::overlay_rect.height()};
        DwmExtendFrameIntoClientArea(overlayWindowHwnd, &m);
        SetWindowLongA(overlayWindowHwnd, GWL_EXSTYLE, WS_EX_TRANSPARENT | WS_EX_LAYERED);

        if (!Render::CreateDeviceD3D(overlayWindowHwnd)) {
            Render::CleanupDeviceD3D();
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
            return;
        }
        ::ShowWindow(overlayWindowHwnd, SW_SHOW);
        ::UpdateWindow(overlayWindowHwnd);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsLight();

        ImGuiIO &io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.WantSaveIniSettings = false;
        Render::pRenderFont = io.Fonts->AddFontFromFileTTF(R"(C:\Windows\Fonts\msyh.ttc)", 18.0f, nullptr,
                                                           io.Fonts->GetGlyphRangesChineseFull());

        ImGui_ImplWin32_Init(overlayWindowHwnd);
        ImGui_ImplDX11_Init(Render::pd3dDevice, Render::pd3dDeviceContext);

        const ImVec4 clear_color = ImVec4(255.00f, 255.00f, 255.00f, 0.00f);
        bool bRender = false;

        while (!bRender) {
            GetWindowRect(targetWindowHwnd, &target_rect);
            MoveWindow(overlayWindowHwnd, target_rect.left, target_rect.top, target_rect.width(), target_rect.height(),
                       true);

            MSG msg;
            while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                if (msg.message == WM_QUIT) {
                    bRender = true;
                }
            }

            if (bRender)
                break;

            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
#ifdef _DEBUG
            ImGui::Begin("Test");
            {
                ImGui::Text("FPS: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                            ImGui::GetIO().Framerate);
            }
            ImGui::End();
#endif
            Render::pIdl = ImGui::GetForegroundDrawList();
            {
                std::lock_guard<std::mutex> documentLock_guard(documentLock);
                if (document.IsObject()) {
                    for (int i = 0; i < document["size"].GetInt(); ++i) {
                        auto nameTage = document["list"][i].GetObj();
                        RenderUtil::RenderTextFilled(nameTage["name"].GetString(), nameTage["x"].GetFloat(),
                                                     nameTage["y"].GetFloat());
                    }
                }
            }

            ImGui::EndFrame();
            ImGui::Render();

            const float clear_color_with_alpha[4] = {clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                     clear_color.z * clear_color.w, clear_color.w};
            Render::pd3dDeviceContext->OMSetRenderTargets(1, &Render::pMainRenderTargetView, nullptr);
            Render::pd3dDeviceContext->ClearRenderTargetView(Render::pMainRenderTargetView, clear_color_with_alpha);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            Render::pSwapChain->Present(1, 0);

        }

        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        Render::CleanupDeviceD3D();
        ::DestroyWindow(overlayWindowHwnd);
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }
}