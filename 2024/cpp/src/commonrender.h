#pragma once

#include <span>

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

SDL_GPUBuffer* createGPUUniformBuffer(uint32_t size, const char* debugName);
SDL_GPUBuffer* createGPUWriteBuffer(uint32_t size, const char* debugName);

bool uploadGPUBufferOneTimeInInit(SDL_GPUBuffer* dstGpuBuffer, uint8_t* data, uint32_t size);
bool downloadGPUBuffer(uint8_t* dstData, SDL_GPUBuffer* srcGpuBuffer, uint32_t size);

SDL_GPUComputePipeline* createComputePipeline(const uint32_t* code, uint32_t codeSize,
    uint32_t workgroupSize, uint32_t bufferAmount, uint32_t uniformBufferAmount);
