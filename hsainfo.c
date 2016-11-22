#include "print_hsa.h"
#include "hsa_error.h"

#include <hsa/hsa.h>

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
  if (argc != 1) {
    printf("prints info about the HSA (Heterogeneous System Architecture).\n");
    printf("usage: %s\n", argv[0]);

    return EXIT_FAILURE;
  }

  hsa_status_t status;

  status = hsa_init();
  hsa_error(status, "has_init");

  printf("HSA system:\n");
  print_system_info();
  printf("\n");

  printf("Agents:\n\n");
  status = hsa_iterate_agents(print_agent, NULL);
  hsa_error(status, "iterate agents");

  status = hsa_shut_down();
  hsa_error(status, "hsa_shut_down");

  return EXIT_SUCCESS;
}
