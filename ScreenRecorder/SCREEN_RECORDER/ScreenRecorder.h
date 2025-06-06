#ifndef SCREEN_RECORDER_H
#define SCREEN_RECORDER_H

#include <d3d11.h>     // We use DirectX 11 because the Desktop Duplication API is only available with it.
#include <dxgi1_2.h>
#include <vector>

// We need this library to use D3D11 functions
#pragma comment (lib, "d3d11.lib")

// Class that captures the screen
class ScreenRecorder {
public:
    // Constructor
    ScreenRecorder();

    // Destructor
    ~ScreenRecorder();

    // Initializes capturing interfaces (D3D11 + Duplication)
    int InitCapturingInterfaces();

    // Starts screen capturing
    //  bool StartCapture();    TODO

    // Ends screen capturing
    //  bool EndCapture();      TODO

private:
    // Functions

    // Optimizes variables after successful InitCapturingInterfaces()
    void OptimizeVarsAfterInit();

    // Captures one frame and writes it into the buffer
    //  bool GetFrame();        TODO

private:
    // Variables

    // D3D11 interfaces
    ID3D11Device* pDevice;
    IDXGIOutputDuplication* pDeskDupl;

    // Screen dimensions
    int                      iWidth;
    int                      iHeight;

    // Temporary buffer where the captured screenshot is stored after GetFrame()
    std::vector<BYTE>        vLastScreen;
};

#endif // SCREEN_RECORDER_H