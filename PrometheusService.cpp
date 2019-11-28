#include "PrometheusService.h"

using namespace prometheus;

pthread_mutex_t PROM_DATA_MTX = PTHREAD_MUTEX_INITIALIZER;

PrometheusService::PrometheusService(uint16_t port) {
	_port = port;
	_is_running = true;

	std::string prom_server = "0.0.0.0:" + std::to_string(_port);
	_exposer = Exposer(prom_server);

    _registry = make_shared<Registry>();
    _exposer.RegisterCollectable(_registry);
}

PrometheusService::~PrometheusService() {
	if(_is_running) {
		_task.join();
	}
}

void PrometheusService::addFamily(std::string host) {
    _prom_mutex.lock();
    _host_family_map[host] = BuildGauge().Name(host).Register(*_registry);
	_prom_mutex.unlock();
}

void PrometheusService::removeFamily(std::string host) {
    //TODO se avanza tempo
}

void PrometheusService::addMetric(string host, string metricName, EMetricType metricType) {
  	_prom_mutex.lock();
    std::map<std::string, std::string> labels;
   
    auto& gauge = _host_family_map[host].Add(labels);

    PromData data;
    data.metricType = metricType;
	
    _gauge_map[metricName] = data;
	_prom_mutex.unlock();
}

void PrometheusService::removeMetric(string metricName) {
 	_prom_mutex.lock();
    PromData data = _gauge_map[metricName];
    //data.family_gauge.Remove();
	_prom_mutex.unlock();	
}

void PrometheusService::startService(void) {
	_task = std::thread(exposer_task, this);
	_is_running = true;
}


void PrometheusService::stopService(void) {
	_task.join();
	_is_running = false;
}


void PrometheusService::publishData(std::string metricName, std::string data) {
	_prom_mutex.lock();
    std::map<std::string, std::string> labels;

    PromData& promdata = _gauge_map[metricName];
    if(promdata.metricType == EMetricType::STRING) {
    	labels.insert({metricName, data});
        auto& gauge = gauge_family.Add(labels);
        gauge.Set(0.0);
    } else {
        auto& gauge = promdata.gauge_family.Add(labels);
        gauge.Set(stod(data)):
    }
	_prom_mutex.unlock();
}

void PrometheusService::exposer_task(PrometheusService *obj) {
	if(obj == NULL) {
		return;
	}

	while(obj->_is_running) {
        sleep(1);
    }
}
