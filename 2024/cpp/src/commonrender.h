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
void gpuReadEndBuffers();

struct ComputePipelineInfo
{
    const uint32_t* m_code;
    uint32_t m_codeSize;
    uint32_t m_bufferAmount;
    uint32_t m_uniformBufferAmount;
    uint32_t m_worgroupSizeX;
    uint32_t m_worgroupSizeY;
};

#define BUF_N_SIZE(name) name, sizeof(name)

// functions provided by commonrender
SDL_GPUDevice* getGpuDevice();

SDL_GPUBuffer* createGPUUniformBuffer(uint32_t size, const char* debugName);
SDL_GPUBuffer* createGPUWriteBuffer(uint32_t size, const char* debugName, bool isIndirect = false);

bool uploadGPUBufferOneTimeInInit(SDL_GPUBuffer* dstGpuBuffer, uint8_t* data, uint32_t size);
bool downloadGPUBuffer(uint8_t* dstData, SDL_GPUBuffer* srcGpuBuffer, uint32_t size);

SDL_GPUComputePipeline* createComputePipeline(const ComputePipelineInfo& info);
