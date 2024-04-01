#include "hudini.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define BUFFER_SIZE 1024
#define ICMP_HEADER_SIZE 8

u32 init_ICMP_ctx (struct _hudini_icmp_ctx** p_ctx)
{
    (*p_ctx) = (struct _hudini_icmp_ctx*) malloc(sizeof(struct _hudini_icmp_ctx));
    memset((*p_ctx)->route_nodes, 0, MAX_HOPS * sizeof (u32));
    (*p_ctx)->route_node_count = 0;
    (*p_ctx)->sockfd = -1;
    (*p_ctx)->listen_addr = (struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
    return HUDINI_SUCC;
}

u32 free_ICMP_ctx (struct _hudini_icmp_ctx** p_ctx)
{
    close((*p_ctx)->sockfd);
    // free ((*p_ctx)->listen_addr); /* DONNO throws segfault */
    free ((*p_ctx));
    return HUDINI_SUCC;
}

u32 prepare_ICMP_socket (struct _hudini_icmp_ctx* ctx)
{
    fprintf(stderr, "Preparing ICMP server...\n");
    if ((ctx->sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        perror("Failed to create socket");
        return (SOCK_INV);
    }
    struct timeval timeout;
    timeout.tv_sec = 1;  // 5 seconds timeout
    timeout.tv_usec = 0;

    if (setsockopt(ctx->sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    return HUDINI_SUCC;
}

u32 get_TTL_exceed_packets(struct _hudini_icmp_ctx* ctx) 
{
    u32 client_len = sizeof(ctx->listen_addr);
    char buffer[BUFFER_SIZE];
    i32 bytes_received;
    
    if ((bytes_received = recvfrom(ctx->sockfd, buffer, sizeof(buffer), 0,
                            (struct sockaddr *)&ctx->listen_addr, &client_len)) < 0) {
        perror ("ICMP packet receive failed");
        return ICMP_RECV_FAIL;
    }

    struct iphdr *ip_header = (struct iphdr *)buffer;
    u32 ip_header_length = ip_header->ihl * 4;
    struct icmphdr *icmp_header = (struct icmphdr *)(buffer + ip_header_length);

    if (icmp_header->type == ICMP_DEST_UNREACH) {
        perror ("Destination is unreachable");
        return ICMP_DEST_UNREACH;
    }

    if (icmp_header->type == ICMP_TIME_EXCEEDED) {
        ctx->route_nodes[ctx->route_node_count++].s_addr = ip_header->saddr;
        return HUDINI_SUCC;
    }
    
    return ICMP_RECV_FAIL;
}

void start_listen_ICMP_server (struct _hudini_icmp_ctx* ctx)
{
    fprintf(stderr, "Listening ICMP server...\n");
    i8 hostname[INET_ADDRSTRLEN];
    struct in_addr addr;
    while (1)
    {
        u32 status = get_TTL_exceed_packets(ctx);

        if (status == ICMP_RECV_FAIL) {
            return;
        }

        if (status == ICMP_DEST_UNREACH) {
            return;
        }

        if (status != HUDINI_SUCC) {
            continue;
        }


        addr = ctx->route_nodes[ctx->route_node_count-1];
        fprintf(stderr, "adr %u\n", addr.s_addr);
        if (inet_ntop(AF_INET, &addr, hostname, INET_ADDRSTRLEN) == NULL) {
            perror("inet_ntop");
            return;
        }
        fprintf(stderr, "\n\n%d# [%s]\n\n", ctx->route_node_count, hostname);
    }
    return;
}