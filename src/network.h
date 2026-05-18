#ifndef NETWORK_H
#define NETWORK_H

#include <stddef.h>

#define SUBNET_BUFFER_SIZE 64

int get_local_subnet(char *subnet_out, size_t size);

#endif