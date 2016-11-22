#include "hsa_error.h"

#include <stdio.h>
#include <stdlib.h>

void hsa_error(hsa_status_t status, const char* message)
{
  if (status != HSA_STATUS_SUCCESS) {
    fprintf(stderr, "error: %s:", message, status);
    const char* status_string;
    hsa_status_t ret = hsa_status_string(status, &status_string);
    if (ret != HSA_STATUS_SUCCESS) {
      fprintf(stderr, "hsa_status: %d (additional error in hsa_status_string %d\n",
          status, ret);
    } else {
      fprintf(stderr, "%s\n", status_string);
    }
    exit(EXIT_FAILURE);
  }
}
