#ifndef NVCOMMON_H
#define NVCOMMON_H

#define NVFAILED(nvStatus) ((nvStatus) != NV_ENC_SUCCESS)

typedef NVENCSTATUS(NVENCAPI* NvEncodeAPICreateInstance_t)(NV_ENCODE_API_FUNCTION_LIST* pFunctionList);

#endif // NVCOMMON_H