#ifndef __HUDINI_H__
#define __HUDINI_H__

#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define TTL_MAX CHAR_MAX
#define MAX_HOPS    30
#define PORT_TO_WORLD 8000

#define HUDINI_SUCC     0
#define TTL_LIM_EXC     1
#define CTX_INV         2
#define SOCK_INV        3
#define MSGBUF_INV      4
#define UDP_SEND_FAIL   5
#define IP_CVT_FAIL     6
#define ICMP_RECV_FAIL  7
#define INV_ADDR        8   

typedef char    i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef u8 TTL_t;

struct _hudini_udp_ctx 
{
    struct sockaddr_in* serveraddr;
    TTL_t active_ttl;   /* The TTL that is not yet applied to packet */
    TTL_t previous_ttl; /* The TTL that is already applied to packet */
    i8*   hostname; /* Maybe remove these as serveraddr will keep same values?? */
    u16   portno;   /* Maybe remove these as serveraddr will keep same values?? */
    u32   sockfd;
    u32   last_bytes_sent;
};

struct _hudini_icmp_ctx
{
    u32 sockfd;
    struct in_addr route_nodes[MAX_HOPS];
    u32 route_node_count;
    struct sockaddr_in* listen_addr;
};

u32 init_UDP_ctx        (struct _hudini_udp_ctx** p_ctx, const i8* hostname, i8 hostname_len, i16 portno);
u32 free_UDP_ctx        (struct _hudini_udp_ctx** ctx);
u32 update_TTL          (struct _hudini_udp_ctx*  ctx);
u32 prepare_UDP_socket  (struct _hudini_udp_ctx*  ctx);
u32 send_UDP_packet     (struct _hudini_udp_ctx*  ctx, const char* payload, u32 payload_length);
u32 regen_TTL           (struct _hudini_udp_ctx*  ctx);

u32 init_ICMP_ctx       (struct _hudini_icmp_ctx** p_ctx);
u32 free_ICMP_ctx       (struct _hudini_icmp_ctx** p_ctx);
u32 prepare_ICMP_socket (struct _hudini_icmp_ctx* ctx);
u32 get_TTL_exceed_packets (struct _hudini_icmp_ctx* ctx);
void start_listen_ICMP_server   (struct _hudini_icmp_ctx* ctx);

#endif /* __HUDINI_H__ */