//
// Created by ManTkai on 2023/2/5.
//

#ifndef EXTERNALRENDER_RENDER_H
#define EXTERNALRENDER_RENDER_H

#include <windows.h>
#include <d3d11.h>
#include <string_view>
#include "imgui/imgui.h"

namespace Render {
    extern ID3D11Device *pd3dDevice;
    extern ID3D11DeviceContext *pd3dDeviceContext;
    extern IDXGISwapChain *pSwapChain;
    extern ID3D11RenderTargetView *pMainRenderTargetView;

    extern ImFont *pRenderFont;
    extern ImDrawList *pIdl;

    bool CreateDeviceD3D(HWND hWnd);

    void CreateRenderTarget();

    void CleanupRenderTarget();

    void CleanupDeviceD3D();
}

namespace RenderUtil {

    struct Block {
        float x;
        float y;
        float width;
        float height;

        Block(float x, float y, float width, float height);
    };

    void RenderTextFilled(std::string_view text, float x, float y);

}

#endif //EXTERNALRENDER_RENDER_H
