#include <memory>
#include <cstdlib>
#include <restbed>
#include <iostream>
#include <thread>
#include <map>
#include <mutex>
#include <vector>
#include <chrono>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

#define GATEWAY_IP   "35.180.63.114"
#define GATEWAY_PORT "20000"

class ResourceManager {
public:
    ResourceManager(std::string url, uint16_t port);
    virtual ~ResourceManager(void);

    int16_t announce_service(void) const;
    int16_t start_resource_service(void) const;

    virtual std::string get_data() = 0;

protected:
    std::string _url;
    uint16_t _port;
    std::string _uri_gtw;
private:
    void start_service_thread(void);
    std::thread _resource_thread;
};