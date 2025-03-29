#ifndef NETWORK_UTILS_H
#define NETWORK_UTILS_H

#include <stdint.h> 

char *network_convert_ip_n_to_p(uint32_t ip, char *output_buffer);

uint32_t network_convert_ip_p_to_n(const char *ip_addr);

#endif