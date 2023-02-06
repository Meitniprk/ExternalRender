//
// Created by ManTkai on 2023/2/5.
//

#include <memory>
#include "Render.h"
#include "Window.h"

namespace Render {
    ID3D11Device *pd3dDevice = nullptr;
    ID3D11DeviceContext *pd3dDeviceContext = nullptr;
    IDXGISwapChain *pSwapChain = nullptr;
    ID3D11RenderTargetView *pMainRenderTargetView = nullptr;
    ImFont *pRenderFont = nullptr;
    ImDrawList *pIdl = nullptr;

    bool CreateDeviceD3D(HWND overlayWindowHwnd) {
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;

        sd.BufferDesc.Width = Window::overlay_rect.width();
        sd.BufferDesc.Height = Window::overlay_rect.height();
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = overlayWindowHwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        const UINT createDeviceFlags = 0;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
        if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                          featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice,
                                          &featureLevel, &pd3dDeviceContext) != S_OK)
            return false;
        CreateRenderTarget();
        return true;
    }

    void CreateRenderTarget() {
        ID3D11Texture2D *pBackBuffer;
        pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        if (pBackBuffer != nullptr) {
            pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pMainRenderTargetView);
            pBackBuffer->Release();
        }
    }

    void CleanupRenderTarget() {

        if (pMainRenderTargetView) {
            pMainRenderTargetView->Release();
            pMainRenderTargetView = nullptr;
        }
    }

    void CleanupDeviceD3D() {
        CleanupRenderTarget();
        if (pSwapChain) {
            pSwapChain->Release();
            pSwapChain = nullptr;
        }

        if (pd3dDeviceContext) {
            pd3dDeviceContext->Release();
            pd3dDeviceContext = nullptr;
        }

        if (pd3dDevice) {
            pd3dDevice->Release();
            pd3dDevice = nullptr;
        }
    }
}

namespace RenderUtil {
    Block::Block(float x, float y, float width, float height) : x(x), y(y), width(width), height(height) {}

    void RenderTextFilled(std::string_view text, float x, float y) {
        ImVec2 textSize = ImGui::CalcTextSize(text.data());
        std::unique_ptr<Block> textBlock = std::make_unique<Block>(x, y, textSize.x + 2, textSize.y + 2);
        Render::pIdl->AddRectFilled(ImVec2(textBlock->x - textSize.x / 2.0f, textBlock->y),
                                    ImVec2(textBlock->x - textSize.x / 2.0f + textBlock->width,
                                           textBlock->y + textBlock->height), IM_COL32(255, 153, 204, 180), 0);
        Render::pIdl->AddText(Render::pRenderFont, 18, ImVec2(textBlock->x - textSize.x / 2.0f, textBlock->y),
                              IM_COL32(255, 255, 250, 255), text.data());
    }
}