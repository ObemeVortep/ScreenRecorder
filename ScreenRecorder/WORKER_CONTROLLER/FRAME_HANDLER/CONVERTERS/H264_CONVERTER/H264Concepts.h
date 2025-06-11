#ifndef H264_CONCEPTS_H
#define H264_CONCEPTS_H

template<class T>
concept NvEncInOutResorceD3D12 = std::same_as<NV_ENC_INPUT_RESOURCE_D3D12, T> || std::same_as<NV_ENC_OUTPUT_RESOURCE_D3D12, T>;

#endif // H264_CONCEPTS_H