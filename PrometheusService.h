#ifndef PROMETHEUSSERVICE_H_
#define PROMETHEUSSERVICE_H_

#include <cstdint>
#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <map>
#include <chrono>
#include <memory>

#include <prometheus/counter.h>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

enum class EMetricType {
    STRING,
    NUMERIC
};

struct PromData {
    prometheus::Gauge gauge;
    EMetricType metricType;
};

class PrometheusService {
public:


	PrometheusService(uint16_t port, std::string metricName, EMetricType metricType);
	virtual ~PrometheusService();

    void addFamily(std::string host);
    void addMetric(std::string host, std::string metricName, EMetricType metricType);
    void removeMetric(std::string metricName);
    void removeFamily(std::string host);

	void startService(void);
	void stopService(void);
	void publishData(std::string metricName, std::string data);

private:
	static void exposer_task(PrometheusService *obj);

    std::map<std::string, PromData> _gauge_map; //{ <nome_servizio>, gauge data }
    std::map<std::string, prometheus::Family<prometheus::Gauge>> _host_family_map; //{ <nome_host>, family gauge }
	std::atomic<bool> _is_running;
    std::mutex _prom_mutex;

	std::thread _task;
	uint16_t _port;

    prometheus::Exposer _exposer;
    std::shared_ptr<prometheus::Registry> _registry;
	
};


#endif /* PROMETHEUSSERVICE_H_ */
