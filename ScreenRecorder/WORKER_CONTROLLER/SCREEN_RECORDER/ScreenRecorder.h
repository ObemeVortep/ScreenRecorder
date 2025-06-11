#ifndef SCREEN_RECORDER_H
#define SCREEN_RECORDER_H

#include "INTERFACES/IWorker.h"

#include "TYPES/DirectXShared.h"
#include "TYPES/SharedDX11On12Texture2D.h"

#include <d3d11.h>     // We use DirectX 11 because the Desktop Duplication API is only available with it.
#include <dxgi1_2.h>
#include <vector>

// For ComPtr smart pointer wrapper
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

#define FRAMES_PER_SECOND   60      // Frames per second, that we recording
#define WIDTH               1920    // Metrics
#define HEIGHT              1080    // Metrics

// Class that captures the screen
class ScreenRecorder : public IWorker {
public:
    // Constructor
    ScreenRecorder(std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared, std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared, std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D);

    // Destructor
    ~ScreenRecorder();

    // Initializes capturing interfaces (D3D11 + Duplication)
    int Initialize() override;

    // Starts screen capturing
    void StartThread() override;

    // Ends screen capturing
    void EndRequest() override;

private:
    // Initialize shared D11On12 Texture2D 
    int InitializeSharedTex();

private:
    // Functions

    // Captures one frame and writes it into vLastScreen
    bool GetFrame(std::vector<BYTE>& vFrameData);

private:
    // DirectX structures, that are shared with other modules for optimizing performance
    std::shared_ptr<DIRECTX11ON12_SHARED> spDirectX11On12Shared;
    std::shared_ptr<DIRECTX11_SHARED> spDirectX11Shared;

private:
    // Connectors between RECORDER -> HANDLER
    // ScreenRecorder -> FrameHandler
    std::shared_ptr<SharedDX11On12Texture2D> spSharedDX11On12Texture2D;

private:
    // Variables

    // D3D11 interfaces
    ComPtr<IDXGIOutputDuplication> cpDeskDupl;

    // Screen dimensions
    int iWidth;
    int iHeight;

    // Used in GetFrame()
    // Shared D11On12 Texture2D, that we use for CopyResource()
    ComPtr<ID3D11Texture2D> SharedTex;

};

#endif // SCREEN_RECORDER_H