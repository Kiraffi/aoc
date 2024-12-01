#pragma once

#ifndef SDL_GPU_SHADERSTAGE_COMPUTE
#define SDL_GPU_SHADERSTAGE_COMPUTE (SDL_GPUShaderStage)2
#endif //SDL_GPU_SHADERSTAGE_COMPUTE

struct SDL_GPUBuffer;
struct SDL_GPUDevice;
struct SDL_GPUCommandBuffer;

// functions on aoc dailies
const char* getTitle();
bool initData();
void deinitData();
bool renderFrame(SDL_GPUCommandBuffer* cmd, int index);



// functions provided by commonrender
SDL_GPUDevice* getGpuDevice();
SDL_GPUBuffer* createGPUWriteBuffer(uint32_t size, const char* debugName);

bool uploadGPUBufferOneTimeInInit(SDL_GPUBuffer* dstGpuBuffer, uint8_t* data, uint32_t size);
bool downloadGPUBuffer(uint8_t* dstData, SDL_GPUBuffer* srcGpuBuffer, uint32_t size);
