#include "hudini.h"

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

u32 init_UDP_ctx (struct _hudini_udp_ctx** p_ctx, const i8* hostname, i8 hostname_len, i16 portno)
{
    *p_ctx = (struct _hudini_udp_ctx*)malloc(sizeof(struct _hudini_udp_ctx));

    fprintf(stderr, "setting up new context...");
    (*p_ctx)->serveraddr = (struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
    bzero((i8 *) (*p_ctx)->serveraddr, sizeof(struct sockaddr_in));
    (*p_ctx)->previous_ttl = 0;
    (*p_ctx)->active_ttl = 1;
    (*p_ctx)->hostname = strndup(hostname, hostname_len); /* Free this!! */
    (*p_ctx)->portno = portno;
    (*p_ctx)->sockfd = -1;
    
    fprintf(stderr, "ctx->serveraddr: %p\n", (*p_ctx)->serveraddr);
    fprintf(stderr, "ctx->previous_ttl: %d\n", (*p_ctx)->previous_ttl);
    fprintf(stderr, "ctx->active_ttl: %d\n", (*p_ctx)->active_ttl);
    fprintf(stderr, "ctx->hostname: %.*s\n", hostname_len, (*p_ctx)->hostname);
    fprintf(stderr, "ctx->portno: %d\n", (*p_ctx)->portno);
    fprintf(stderr, "ctx->sockfd: %d\n", (*p_ctx)->sockfd);
    
    return HUDINI_SUCC;
}

u32 free_UDP_ctx (struct _hudini_udp_ctx** ctx)
{
    close ((*ctx)->sockfd);
    free  ((*ctx)->serveraddr);
    free  ((*ctx)->hostname);
    free  (*ctx);
    return HUDINI_SUCC;
}

u32 update_TTL (struct _hudini_udp_ctx* ctx) 
{
    if (ctx->active_ttl == 20) {
        perror ("TTL limit exceeded");
        return (TTL_LIM_EXC);
    }
    ctx->previous_ttl = ctx->active_ttl++;
    fprintf(stderr, "new values are: \n ctx->previous_ttl: %d, ctx->active_ttl: %d\n", 
                                        ctx->previous_ttl, ctx->active_ttl);
    return HUDINI_SUCC;
}

u32 prepare_UDP_socket (struct _hudini_udp_ctx* ctx) 
{
    if ((ctx->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("Failed to create socket");
        return (SOCK_INV);
    }

    ctx->serveraddr->sin_family = AF_INET;
    ctx->serveraddr->sin_port = htons(ctx->portno);
    ctx->serveraddr->sin_addr.s_addr = inet_addr(ctx->hostname);
    return HUDINI_SUCC;
}

u32 send_UDP_packet (struct _hudini_udp_ctx* ctx, const char* payload, u32 payload_length)
{
    if (payload == NULL) {
        perror ("Invalid message buffer");
        return (MSGBUF_INV);
    }

    if ((ctx->last_bytes_sent = sendto(ctx->sockfd, payload, payload_length, 0, (struct sockaddr*)ctx->serveraddr, sizeof (struct sockaddr))) < 0) {
        perror ("Failed to send UDP packet");
        return (UDP_SEND_FAIL);
    }
    fprintf(stderr, "Sent %d bytes ... \n", ctx->last_bytes_sent);
    return HUDINI_SUCC;
}

u32 regen_TTL (struct _hudini_udp_ctx* ctx)
{
    if (ctx->sockfd < 0) {
        perror("Socket is invalid");
        return SOCK_INV;
    }

    fprintf(stderr, "Setting TTL as socket option...\n");
    if (setsockopt(ctx->sockfd, IPPROTO_IP, IP_TTL, &ctx->active_ttl, sizeof (ctx->active_ttl)) < 0) {
        perror ("Failed to set socket option TTL");
        return SOCK_INV;
    }
    
    fprintf(stderr, "Set the socket option TTL with value %d\n", ctx->active_ttl);

    return update_TTL(ctx);;
}