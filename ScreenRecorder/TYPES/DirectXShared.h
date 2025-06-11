#ifndef DIRECTX_SHARED_H
#define DIRECTX_SHARED_H

#include <wrl/client.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d3d11.h>
#include <d3d11_1.h>

typedef struct _DIRECTX12_SHARED {
	Microsoft::WRL::ComPtr<ID3D12Device> cpDevice;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> cpCommandQueue;
} DIRECTX12_SHARED;

typedef struct _DIRECTX11ON12_SHARED {
	Microsoft::WRL::ComPtr<ID3D11Device> cpDevice;
	Microsoft::WRL::ComPtr<ID3D11On12Device> cpDeviceOn12;
	Microsoft::WRL::ComPtr<ID3D11On12Device2> cpDevice2On12;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> cpContext;
} DIRECTX11ON12_SHARED;

typedef struct _DIRECTX11_SHARED {
	Microsoft::WRL::ComPtr<ID3D11Device> cpDevice;
	Microsoft::WRL::ComPtr<ID3D11Device1> cpDevice1;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> cpContext;
} DIRECTX11_SHARED;

#endif // DIRECTX_SHARED_H