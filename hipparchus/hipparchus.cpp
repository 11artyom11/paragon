
#include <iostream>

#include "hipparchus.h"
Hipparchus::Hipparchus(std::shared_ptr<HostNode>& new_head, const std::string new_server_host) :
                 head{new_head},
                    tail{new_head},
                        server_host{new_server_host}
{
    std::cout << "Constructed Hipparchus...";
}

Hipparchus::Hipparchus(const std::string new_server_host) :
                        server_host{new_server_host}
{
    std::cout << "Constructed Hipparchus...";
}


bool Hipparchus::connect_to_server(void)
{
    bool success = rest.Connect("ip-api.com",port,bTls,bAutoReconnect);
    if (success == false) {
        std::cout << rest.lastErrorText() << "\r\n";
        return false;
    }
    return true;
}

bool Hipparchus::request_server_info(std::shared_ptr<HostNode>& host)
{
    std::string request_str = "/json/";
    request_str += host->get_host_name();
    const char* responseJson = rest.fullRequestNoBody("GET", request_str.c_str());
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return false;
    }
    host->get_json_ref().Load(responseJson);
    return true;
}

void Hipparchus::dump_hostnodes(void) const
{
    std::shared_ptr<HostNode> tmp_host = this->head;
    while (tmp_host.get() != nullptr) 
    {
        tmp_host->dump_node();
        tmp_host = tmp_host->next();
    } 
}

void Hipparchus::add_host(const std::shared_ptr<HostNode>& new_host)
{
    if(head.get() == nullptr) {
        this->head = new_host;
        this->tail = new_host;
        return;
    }
    this->tail->set_next(new_host);++this->hostlist_len;
    this->tail = new_host;
}

Hipparchus::HostNode::HostNode(const std::string new_host_name, std::shared_ptr<HostNode>& new_next_host) :
                                    host_name{new_host_name},
                                    next_host{new_next_host}
{
    // std::cout << "constructed HostNode\n";
    // dump_node();
}

Hipparchus::HostNode::HostNode(const std::string new_host_name) :
                                host_name{new_host_name}
{
    // std::cout << "constructed HostNode\n";
    // dump_node();
}

void Hipparchus::HostNode::set_next(const std::shared_ptr<HostNode>& next_host)
{
    this->next_host = next_host;
}

void Hipparchus::HostNode::dump_node(void) const
{
    std::cout << "=============================\n";
    std::cout << "host_name = " << host_name << std::endl;
    std::cout << "=============================\n";
}

std::string Hipparchus::HostNode::get_json_val (const std::string& key)
{
    CkString value;
    if (host_info.StringOf(key.c_str(), value) == false) {
        std::cerr << "Failed to retrieve key for specified value... \n";
        return "Nil";
    }
    return value.getString();
}


void ChilkatSample(void)
{
    // This example requires the Chilkat API to have been previously unlocked.
    // See Global Unlock Sample for sample code.

    // The IP address used in this example is 104.40.211.35
    const char *ipAddress = "157.240.9.35";
    (void)ipAddress;
    // First we'll try the service at freegeoip.net.
    // They have a limit of 10,000 queries per hour, and also 
    // provide free source code to run your own server.

    CkRest rest;

    // Connect to freegeoip.net
    bool bTls = false;
    int port = 80;
    bool bAutoReconnect = true;
    bool success ;

    // Query the IP address to return JSON.
    const char *responseJson;
    // Just in case we are still connected..
    int maxWaitMs = 10;
    
    CkJsonObject json;
   
    // -----------------------------------------------------
    // Now to use ip-api.com, which is mostly the same..

    success = rest.Connect("ip-api.com",port,bTls,bAutoReconnect);
    if (success == false) {
        std::cout << rest.lastErrorText() << "\r\n";
        return;
    }

    // Query the IP address to return JSON.
    responseJson = rest.fullRequestNoBody("GET","/json/104.40.211.35");
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return;
    }

    // Just in case we are still connected..
    rest.Disconnect(maxWaitMs);

    json.Load(responseJson);
    json.put_EmitCompact(false);

    std::cout << "<><<>>><><>" <<json.emit() << "\r\n";

    // The JSON we get back looks like this:
    // This is very strange, because the two services don't agree.
    // {
    //   "as": "AS8075 Microsoft Corporation",
    //   "city": "Amsterdam",
    //   "country": "Netherlands",
    //   "countryCode": "NL",
    //   "isp": "Microsoft Corporation",
    //   "lat": 52.35,
    //   "lon": 4.9167,
    //   "org": "Microsoft Azure",
    //   "query": "104.40.211.35",
    //   "region": "NH",
    //   "regionName": "North Holland",
    //   "status": "success",
    //   "timezone": "Europe/Amsterdam",
    //   "zip": "1091"
    // }

    // Examine a few bits of information:
    std::cout << "country name = " << json.stringOf("country") << "\r\n";
    std::cout << "country code = " << json.stringOf("countryCode") << "\r\n";
}
