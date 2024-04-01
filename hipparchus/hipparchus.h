#ifndef __HIPPARCHUS_H__
#define __HIPPARCHUS_H__

#include <string>
#include <memory>
#include "include/CkRest.h"
#include "include/CkJsonObject.h"

class Hipparchus;

class Hipparchus
{
    public:
        class HostNode 
        {
            public:
                HostNode(const std::string host_name, std::shared_ptr<HostNode>& next_host);
                HostNode(const std::string host_name);
                HostNode(const HostNode&) {};
                void set_next(const std::shared_ptr<HostNode>& next_host);
                void dump_node(void) const;

                std::shared_ptr<HostNode>& next() {return next_host;}
                std::string get_host_name(void) const {return this->host_name;};
                CkJsonObject& get_json_ref (void) {return this->host_info;};
                std::string get_json_val (const std::string& key);
            private:
                std::string host_name; 
                std::shared_ptr<HostNode> next_host;
                CkJsonObject host_info;
        };
        Hipparchus(std::shared_ptr<HostNode>& new_head, const std::string new_server_host);
        Hipparchus(const std::string new_server_host);
        bool connect_to_server(void);
        bool request_server_info(std::shared_ptr<HostNode>& host);
        void dump_hostnodes(void) const;
        void add_host (const std::shared_ptr<HostNode>& new_host);
    private:
        std::shared_ptr<HostNode> head;
        std::shared_ptr<HostNode> tail;
        unsigned int hostlist_len;
        std::string server_host;
        CkRest rest;
        // Connect to freegeoip.net
        const bool bTls = false;
        const int port = 80;
        const bool bAutoReconnect = true;
};

#endif /* HIPPARCHUS_H */