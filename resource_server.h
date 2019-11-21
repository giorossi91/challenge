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

typedef std::string (*get_data_func_T)(void);

#define CPU_PORT 10002
#define RAM_PORT 10001
#define LAT_PORT 10000
#define GTW_PORT 10003

class ResourceManager {
public:
    ResourceManager(std::string uri_gtw, std::string url, uint16_t port, get_data_func_T func);
    virtual ~ResourceManager(void);

    void announce_service(void) const;
    void start_resource_service(void);

    std::string _url;
    uint16_t _port;
    std::string _uri_gtw;

private:
    static void start_service_thread(ResourceManager *obj);
    static void get_resource_handler( const std::shared_ptr< restbed::Session > session );
    std::thread _resource_thread;
};