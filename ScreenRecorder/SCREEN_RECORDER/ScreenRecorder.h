#ifndef SCREEN_RECORDER_H
#define SCREEN_RECORDER_H

#include "RECORDER/IRecorder.h"

#include <d3d11.h>     // We use DirectX 11 because the Desktop Duplication API is only available with it.
#include <dxgi1_2.h>
#include <vector>

// We need this library to use D3D11 functions
#pragma comment (lib, "d3d11.lib")

// Class that captures the screen
class ScreenRecorder : public IRecorder {
public:
    // Constructor
    ScreenRecorder();

    // Destructor
    ~ScreenRecorder();

    // Initializes capturing interfaces (D3D11 + Duplication)
    int Initialize() override;

    // Starts screen capturing
    void StartThread() override;

    // Ends screen capturing
    void EndRequest() override;

private:
    // Functions

    // Captures one frame and writes it into vLastScreen
    bool GetFrame(std::vector<BYTE>& vFrameData);

private:
    // Variables

    // D3D11 interfaces
    ID3D11Device*           pDevice;
    ID3D11DeviceContext*    pContext;
    IDXGIOutputDuplication* pDeskDupl;

    // Screen dimensions
    int                     iWidth;
    int                     iHeight;
};

#endif // SCREEN_RECORDER_H