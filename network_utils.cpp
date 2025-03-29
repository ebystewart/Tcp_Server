#include <stdint.h>
#include <stdlib.h>
#include "memory.h"
#include "sys/socket.h"
#include "arpa/inet.h"
#include "network_utils.h"

char *network_convert_ip_n_to_p(uint32_t ip, char *output_buffer)
{
    char *out = NULL;
    char str_ip[16];
    out = !output_buffer ? str_ip : output_buffer;
    memset(out, 0, 16);
    /* For little endian data, byte order will be reverseed; ignored otherwise */
    ip = htonl(ip);
    /* convert ip interger to ip string */
    inet_ntop(AF_INET, &ip, out, 16);
    str_ip[15] = '\0';
    if(output_buffer != NULL)
        output_buffer = out;
    else 
        return out;
    return NULL;
}

uint32_t network_convert_ip_p_to_n(const char *ip_addr)
{
    uint32_t ip_ret;
    /* convert ip string to equivalent integer */
    inet_pton(AF_INET, ip_addr, &ip_ret);
    /* convert to network byte order in little endian machine */
    ip_ret = htonl(ip_ret);
    return ip_ret;
}