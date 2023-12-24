/**
 * bondexecutionhistoricaldataservice.hpp
 *
 * This file defines the BondExecutionHistoricalDataService and its related components for persisting execution order data in a bond trading system. Key elements include:
 * - 'BondExecutionOrderServiceListener': A listener that processes ExecutionOrder<Bond> data and forwards it to the historical data service.
 * - 'BondExecutionOrderConnector': An OutputFileConnector that formats ExecutionOrder<Bond> data into CSV strings and writes them to 'executions.csv'.
 * - 'BondExecutionHistoricalDataService': A service that extends HistoricalDataService for ExecutionOrder<Bond>. It manages the persistence of execution order data to a file.
 *
 * This service is integral for recording historical data of bond execution orders, which is vital for analysis, compliance, and auditing purposes within the trading system.
 */

#ifndef BOND_EXECUTION_HISTORICAL_DATA_SERVICE_HPP
#define BOND_EXECUTION_HISTORICAL_DATA_SERVICE_HPP

#include "historicaldataservice.hpp"
#include "products.hpp"
#include "OutputFileConnector.hpp"
#include "streamingservice.hpp"
#include "executionservice.hpp"

/**
 * Writes all ExecutionOrder to executions.csv
 */
class BondExecutionOrderServiceListener : public ServiceListener<ExecutionOrder<Bond>> {
public:
    explicit BondExecutionOrderServiceListener(HistoricalDataService<ExecutionOrder<Bond>>* listeningService);
    void ProcessRemove(ExecutionOrder<Bond>& data) override;
    void ProcessUpdate(ExecutionOrder<Bond>& data) override;
private:
    HistoricalDataService<ExecutionOrder<Bond>>* listeningService;
    void ProcessAdd(ExecutionOrder<Bond>& data) override;
};

class BondExecutionOrderConnector : public OutputFileConnector<ExecutionOrder<Bond>> {
public:
    explicit BondExecutionOrderConnector(const string& filePath);
private:
    string toCSVString(ExecutionOrder<Bond>& data) override;
    string getCSVHeader() override;
};

class BondExecutionHistoricalDataService : public HistoricalDataService<ExecutionOrder<Bond>> {
public:
    BondExecutionHistoricalDataService();
    void PersistData(string persistKey, const ExecutionOrder<Bond>& data) override;
private:
    void OnMessage(ExecutionOrder<Bond>& data) override;
    BondExecutionOrderConnector* connector;
};

void BondExecutionHistoricalDataService::PersistData(string persistKey, const ExecutionOrder<Bond>& data) {
    connector->Publish(const_cast<ExecutionOrder<Bond> &>(data));
}
BondExecutionHistoricalDataService::BondExecutionHistoricalDataService() {
    connector = new BondExecutionOrderConnector("execution.csv");
    connector->WriteHeader();
}

BondExecutionOrderConnector::BondExecutionOrderConnector(const string& filePath) : OutputFileConnector(filePath) {
}

string BondExecutionOrderConnector::toCSVString(ExecutionOrder<Bond>& data) {
    std::ostringstream oss;
    oss << boost::posix_time::microsec_clock::universal_time() << "," <<
        data.GetProduct().GetProductId() << "," <<
        data.GetSide() << "," <<
        data.GetOrderId() << "," <<
        data.GetOrderType() << "," <<
        data.GetPrice() << "," <<
        data.GetVisibleQuantity() << "," <<
        data.GetHiddenQuantity() << "," <<
        data.GetParentOrderId() << "," <<
        data.IsChildOrder();
    return oss.str();;
}
string BondExecutionOrderConnector::getCSVHeader() {
    return "Timestamp,CUSIP,PricingSide,OrderId,OrderType,Price,VisibleQuantity,HiddenQuantity,ParentOrderId,IsChildOrder";
}

BondExecutionOrderServiceListener::BondExecutionOrderServiceListener(HistoricalDataService<ExecutionOrder<Bond>>* listeningService)
    : listeningService(
        listeningService) {}

void BondExecutionOrderServiceListener::ProcessAdd(ExecutionOrder<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondExecutionOrderServiceListener::ProcessRemove(ExecutionOrder<Bond>& data) {

}
void BondExecutionOrderServiceListener::ProcessUpdate(ExecutionOrder<Bond>& data) {

}

void BondExecutionHistoricalDataService::OnMessage(ExecutionOrder<Bond>& data) {

}

#endif //