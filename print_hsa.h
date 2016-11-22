#pragma once

#include <hsa/hsa.h>

hsa_status_t print_cache(hsa_cache_t cache, void* data);

void print_extensions(uint8_t extensions[128]);

hsa_status_t print_isa(hsa_isa_t isa, void* data);

hsa_status_t print_agent(hsa_agent_t agent, void *data);

void print_system_info();
