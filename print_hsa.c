#include "print_hsa.h"

#include "hsa_error.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// Docs:
// http://www.hsafoundation.com/html_spec11/HSA_Library.htm

hsa_status_t print_cache(hsa_cache_t cache, void* data)
{
  hsa_status_t status;

  /*
  uint32_t name_length;
  status = hsa_cache_get_info(cache, HSA_CACHE_INFO_NAME_LENGTH, &name_length);
  hsa_error(status, "cache info name length");

  const char* name;
  status = hsa_cache_get_info(cache, HSA_CACHE_INFO_NAME, &name);
  hsa_error(status, "cache info name");
  assert(name[name_length] == '\0');

  printf("cache:\t%s\n", name);
  */


  uint32_t size;
  status = hsa_cache_get_info(cache, HSA_CACHE_INFO_SIZE, &size);
  hsa_error(status, "cache info size");
  if (size != 0) {
    printf("%d bytes ", size);
  } else {
    printf("NA bytes ");
  }

  uint8_t level;
  status = hsa_cache_get_info(cache, HSA_CACHE_INFO_LEVEL, &level);
  hsa_error(status, "cache info level");
  printf("%dL; ", level);

  return HSA_STATUS_SUCCESS;
}

void print_extensions(uint8_t extensions[128])
{
  const char* extension_name;
  // see:
  // https://github.com/RadeonOpenCompute/ROCR-Runtime/blob/6c4c7c3b8a6a917d40357ab898e7ca248e107d13/src/core/runtime/runtime.cpp#L518
  for (uint32_t bit=0; bit<8*128; ++bit) {
    uint8_t index = bit / 8;
    uint8_t sub_bit = bit % 8;
    if (extensions[index] & (1<<sub_bit)) {
       hsa_status_t status = hsa_extension_get_name(bit, &extension_name);
       hsa_error(status, "get extension name");
       printf("%s ", extension_name);
    }
  }
  printf("\n");
}

hsa_status_t print_isa(hsa_isa_t isa, void* data)
{
  hsa_status_t status;

  uint32_t name_length;
  status = hsa_isa_get_info_alt(isa, HSA_ISA_INFO_NAME_LENGTH, &name_length);
  hsa_error(status, "get isa name length");

  char name[name_length];
  status = hsa_isa_get_info_alt(isa, HSA_ISA_INFO_NAME, &name);
  hsa_error(status, "get isa name");
  assert(name[name_length-1] == '\0');

  printf("%s ", name);
  //FIXME print more info
  //FIXME hsa_isa_iterate_wavefronts

  return HSA_STATUS_SUCCESS;
}

hsa_status_t print_memory(hsa_region_t region, void* data)
{
  hsa_status_t status;

  hsa_region_segment_t segment;
  status = hsa_region_get_info(region, HSA_REGION_INFO_SEGMENT, &segment);
  hsa_error(status, "get region segment");
  switch (segment) {
    case HSA_REGION_SEGMENT_GLOBAL:
      printf("global");
      break;
    case HSA_REGION_SEGMENT_READONLY:
      printf("readonly");
      break;
    case HSA_REGION_SEGMENT_PRIVATE:
      printf("private");
      break;
    case HSA_REGION_SEGMENT_GROUP:
      printf("group");
      break;
    case HSA_REGION_SEGMENT_KERNARG:
      printf("kernarg");
      break;
  }

  size_t size;
  status = hsa_region_get_info(region, HSA_REGION_INFO_SIZE, &size);
  hsa_error(status, "get region size");
  printf(" %d bytes; ", size);

  return HSA_STATUS_SUCCESS;
}

hsa_status_t print_agent(hsa_agent_t agent, void *data)
{
  hsa_status_t status;

  char name[64] = { 0 };
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_NAME, name);
  hsa_error(status, "get name");
  printf("Agent:\t%s\n", name);

  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_VENDOR_NAME, name);
  hsa_error(status, "get name");
  printf("vendor:\t%s\n", name);

  hsa_device_type_t device_type;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_DEVICE, &device_type);
  hsa_error(status, "get device type");

  switch (device_type) {
    case HSA_DEVICE_TYPE_CPU:
      printf("type:\tCPU\n");
      break;
    case HSA_DEVICE_TYPE_GPU:
      printf("type:\tGPU\n");
      break;
    case HSA_DEVICE_TYPE_DSP:
      printf("type:\tDSP\n");
      break;
    default:
      printf("type:\tinvalid\n");
      break;
  }

  hsa_profile_t profile;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_PROFILE, &profile);
  hsa_error(status, "get profile");
  printf("profile:\t%s\n", profile == HSA_PROFILE_BASE ? "base" : "full");

  uint16_t major;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_VERSION_MAJOR, &major);
  hsa_error(status, "get major");

  uint16_t minor;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_VERSION_MINOR, &minor);
  hsa_error(status, "get minor");

  printf("version:\t%d.%d\n", major, minor);


  hsa_agent_feature_t feature;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_FEATURE, &feature);
  hsa_error(status, "get feature");
  printf("feature:\t");
  if (feature & HSA_AGENT_FEATURE_KERNEL_DISPATCH) {
    printf("kernel dispatch ");
  }
  if (feature & HSA_AGENT_FEATURE_AGENT_DISPATCH) {
    printf("agent dispatch ");
  }
  printf("\n");

  if (device_type == HSA_DEVICE_TYPE_GPU) {
    printf("isas:\t");
    status = hsa_agent_iterate_isas(agent, &print_isa, NULL);
    hsa_error(status, "iterate isas");
    printf("\n");
  }

  hsa_queue_type_t queue_type;
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_QUEUE_TYPE, &queue_type);
  hsa_error(status, "get queue type");
  switch (queue_type) {
    case HSA_QUEUE_TYPE_MULTI:
      printf("queue type:\tmulti\n");
      break;
    case HSA_QUEUE_TYPE_SINGLE:
      printf("queue type:\tsingle\n");
      break;
    default:
      printf("queue type:\tinvalid\n");
      break;
  }

  uint8_t extensions[128];
  status = hsa_agent_get_info(agent, HSA_AGENT_INFO_EXTENSIONS, &extensions);
  hsa_error(status, "get agent extensions");

  printf("extensions:\t");
  print_extensions(extensions);

  printf("caches:\t");
  status = hsa_agent_iterate_caches(agent, print_cache, NULL);
  hsa_error(status, "iterate agent caches");
  printf("\n");

  printf("memory:\t");
  status = hsa_agent_iterate_regions(agent, print_memory, NULL);
  hsa_error(status, "iterate agent memory");
  printf("\n");

  printf("\n");

  return HSA_STATUS_SUCCESS;
}


void print_system_info()
{
  uint16_t major;
  hsa_status_t status = hsa_system_get_info(HSA_SYSTEM_INFO_VERSION_MAJOR, &major);
  hsa_error(status, "get system major");

  uint16_t minor;
  status = hsa_system_get_info(HSA_SYSTEM_INFO_VERSION_MINOR, &minor);
  hsa_error(status, "get system minor");

  printf("version:\t%d.%d\n", major, minor);


  uint8_t extensions[128];
  status = hsa_system_get_info(HSA_SYSTEM_INFO_EXTENSIONS, &extensions);
  hsa_error(status, "get system extensions");

  printf("extensions:\t");
  print_extensions(extensions);


  hsa_endianness_t endianness;
  status = hsa_system_get_info(HSA_SYSTEM_INFO_ENDIANNESS, &endianness);
  hsa_error(status, "get system endianness");
  switch (endianness) {
    case 0:
      printf("endianness:\tlittle\n");
      break;
    case 1:
      printf("endianness:\tbig\n");
      break;
    default:
      printf("endianness: invalid (%d)\n", endianness);
      break;
  }

  hsa_machine_model_t machine_model;
  status = hsa_system_get_info(HSA_SYSTEM_INFO_MACHINE_MODEL, &machine_model);
  hsa_error(status, "get system machine model");
  switch (machine_model) {
    case 0:
      printf("machine model:\tsmall\n");
      break;
    case 1:
      printf("machine model:\tbig\n");
      break;
    default:
      printf("machine model: invalid (%d)\n", machine_model);
      break;
  }

  printf("\n"); 
}
