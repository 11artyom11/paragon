extern "C" {
#include "hudini.h"
}
#include "paragon.h"
#include "hipparchus.h"
#include "rembrandt.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <vector>
#include <cstring>

static i8 *hostnames[MAX_HOPS];
static std::vector<host> hosts;
static u32 hostname_count;

u32 init_hudini(u32 argc, i8* argv[], struct _hudini_udp_ctx** uctx, struct _hudini_icmp_ctx** ictx);
u32 init_rembrandt(int argc, char **argv);
u32 to_hostname (struct in_addr* addr, char* hostname);

int main(int argc, char* argv[])
{
    struct _hudini_udp_ctx* uctx = NULL;
    struct _hudini_icmp_ctx* ictx = NULL;
    i8 hostname[INET_ADDRSTRLEN] = {0};

    init_hudini(argc, argv, &uctx, &ictx);

    fprintf(stderr, "Done reverse lookup... the node list retrieved\n");
    hostname_count = ictx->route_node_count;
    for (u32 i = 0; i < ictx->route_node_count; ++i) {        
        if (to_hostname(&ictx->route_nodes[i], hostname) != HUDINI_SUCC) {
            perror ("Failed to convert address");
            continue;
        }
        hostnames[i] = strdup(hostname);
        fprintf(stderr, "#[%d] -- %s\n", i, hostname);
    }

/*  */
    Hipparchus hip("ip-api.com");
    if (hip.connect_to_server() == false) {
        std::cout << "error while connecting to the server...";
        return 0;
    }
    std::cout << "Successfully connected to the server\n";

    for (size_t i = 0; i < hostname_count; i++) {
    //     std::string s = hostnames[i];
        auto node = std::make_shared<Hipparchus::HostNode>(hostnames[i]);
        hip.add_host(node);
        if (hip.request_server_info(node) == false) {
            std::cout << "Failed to request ip host information...\n";
            continue;
        }
        std::cout << node->get_host_name() << ": [country] = " << node->get_json_val("country") << "\n" \
                        "\t\t: [latitude] = " << node->get_json_val("lat") << "\n" \ 
                        "\t\t: [longitude] = " << node->get_json_val("lon") << "\n";
        std::string lat = node->get_json_val("lat");
        std::string lon = node->get_json_val("lon");
        double latd = atof(lat.c_str());
        double lond = atof(lon.c_str());
        host h;
        h.host_coordinates = {latd,lond};
        h.city = node->get_json_val("country");
        hosts.push_back(h);
    }

    hip.dump_hostnodes();
/*  */
    init_rembrandt(argc, argv);

    u32 status = free_UDP_ctx(&uctx);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }

    status = free_ICMP_ctx(&ictx);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }
    return PARAGON_SUCC;
}

u32 init_hudini(u32 argc, i8* argv[], struct _hudini_udp_ctx** uctx, struct _hudini_icmp_ctx** ictx)
{

    u32 status = 0;
    i8 *hostname = argv[1];
    i16 portno  = PORT_TO_WORLD;
    i8 buffer[] = "Hello world\n";

    if (argc < 2) {
        perror ("Too few arguments passed to Paragon");
        exit (TOOFEWARG);
    }
    status = init_UDP_ctx(uctx, hostname, strlen(hostname), portno);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }

    status = init_ICMP_ctx(ictx);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }
    
    status = prepare_UDP_socket(*uctx);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }

    status = prepare_ICMP_socket(*ictx);
    if (status != HUDINI_SUCC) {
        perror ("Hudini failed");
        return status;
    }

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, (void* (*)(void *))start_listen_ICMP_server, (void*) (*ictx));

    while (1) {
        status = regen_TTL(*uctx);
        if (status == TTL_LIM_EXC) {
            fprintf(stderr, "TTL limit is reached. Breaking loop.\n");
            break;    
        }

        if (status != HUDINI_SUCC) {
            perror ("Hudini failed");
            return status;
        }

        status = send_UDP_packet(*uctx, buffer, strlen (buffer));
        if (status != HUDINI_SUCC) {
            perror ("Hudini failed");
            return status;
        }
        usleep(5000);
    }    
    
    pthread_join(thread_id, NULL);
    return PARAGON_SUCC;
}

u32 to_hostname (struct in_addr* addr, char* hostname)
{
    if (hostname == NULL) {
        return MSGBUF_INV;
    }

    if (inet_ntop(AF_INET, addr, hostname, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop");
        return INV_ADDR;
    }
    return HUDINI_SUCC;
}

u32 init_rembrandt(int argc, char **argv)
{

    init_rembrandt_internal(argc, argv, hostnames, hostname_count, hosts);

    return PARAGON_SUCC;
}
