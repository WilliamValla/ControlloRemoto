#pragma once

namespace crlib
{

enum COLOR_SPACE_FORMAT
{
	COLOR_SPACE_FORMAT_RGBA = 28,
	COLOR_SPACE_FORMAT_BGRA = 87,
	COLOR_SPACE_FORMAT_NV12 = 103
};

enum RESOURCE_CPU_ACCESS
{
	RESOURCE_CPU_ACCESS_NONE = 0,
	RESOURCE_CPU_ACCESS_WRITE = 0x10000L,
	RESOURCE_CPU_ACCESS_READ = 0x20000L
};

enum RESOURCE_USAGE
{
	RESOURCE_USAGE_GPU_READ_WRITE = 0,
	RESOURCE_USAGE_GPU_READ = 1,
	RESOURCE_USAGE_GPU_READ_CPU_WRITE = 2,
	RESOURCE_USAGE_CPU_COPY = 3
};

enum RESOURCE_MAP_MODE
{
	RESOURCE_MAP_MODE_READ = 1,
	RESOURCE_MAP_MODE_WRITE = 2,
	RESOURCE_MAP_MODE_READ_WRITE = 3,
};

} //namespace