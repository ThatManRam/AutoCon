#include "network.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

static int count_prefix_bits(uint32_t mask) {
    int bits = 0;

    while (mask) {
        bits += mask & 1;
        mask >>= 1;
    }

    return bits;
}

int get_local_subnet(char *subnet_out, size_t size) {
    struct ifaddrs *ifaddr = NULL;
    struct ifaddrs *ifa = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs failed");
        return -1;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr || !ifa->ifa_netmask) {
            continue;
        }

        if (ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        if (ifa->ifa_flags & IFF_LOOPBACK) {
            continue;
        }

        if (!(ifa->ifa_flags & IFF_UP)) {
            continue;
        }

        struct sockaddr_in *addr =
            (struct sockaddr_in *)ifa->ifa_addr;

        struct sockaddr_in *mask =
            (struct sockaddr_in *)ifa->ifa_netmask;

        uint32_t ip_host_order =
            ntohl(addr->sin_addr.s_addr);

        uint32_t mask_host_order =
            ntohl(mask->sin_addr.s_addr);

        uint32_t network_host_order =
            ip_host_order & mask_host_order;

        struct in_addr network_addr;
        network_addr.s_addr = htonl(network_host_order);

        char network_str[INET_ADDRSTRLEN];

        if (!inet_ntop(
                AF_INET,
                &network_addr,
                network_str,
                sizeof(network_str)
            )) {
            perror("inet_ntop failed");
            freeifaddrs(ifaddr);
            return -1;
        }

        int cidr = count_prefix_bits(mask_host_order);

        snprintf(
            subnet_out,
            size,
            "%s/%d",
            network_str,
            cidr
        );

        printf("[*] Using interface: %s\n", ifa->ifa_name);

        freeifaddrs(ifaddr);
        return 0;
    }

    freeifaddrs(ifaddr);
    return -1;
}