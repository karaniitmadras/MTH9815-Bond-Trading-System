/**
 * bondriskhistoricaldataservice.hpp
 * 
 * This file defines the BondRiskHistoricalDataService for a bond trading system. It is responsible for managing and persisting risk data related to bond positions. Key components include:
 * - 'BondRiskServiceListener': A listener that processes updates from the BondRiskService, forwarding PV01 data (Price Value of a Basis Point) to the historical data service.
 * - 'BondRiskConnector': An OutputFileConnector for formatting PV01<Bond> data into CSV strings and writing them to 'risk.csv'.
 * - 'BondRiskHistoricalDataService': A service that extends HistoricalDataService for PV01<Bond> objects, focusing on the storage and historical tracking of bond risk data.
 *
 * The primary aim of this service is to maintain a record of bond risk metrics, essential for risk management and analysis within the trading system.
 */

#ifndef BOND_RISK_HISTORICAL_DATA_SERVICE_HPP
#define BOND_RISK_HISTORICAL_DATA_SERVICE_HPP

#include "historicaldataservice.hpp"
#include "products.hpp"
#include "OutputFileConnector.hpp"
#include "riskservice.hpp"

/**
 * Listens to updates from BondRiskService.
 */
class BondRiskServiceListener : public ServiceListener<PV01<Bond>> {
public:
    explicit BondRiskServiceListener(HistoricalDataService<PV01<Bond>>* listeningService);
    void ProcessRemove(PV01<Bond>& data) override;
    void ProcessUpdate(PV01<Bond>& data) override;
private:
    HistoricalDataService<PV01<Bond>>* listeningService;
    void ProcessAdd(PV01<Bond>& data) override;
};

/**
 * Writes output to risk.csv
 */
class BondRiskConnector : public OutputFileConnector<PV01<Bond>> {
public:
    explicit BondRiskConnector(const string& filePath);
private:
    string toCSVString(PV01<Bond>& data) override;
    string getCSVHeader() override;
};

class BondRiskHistoricalDataService : public HistoricalDataService<PV01<Bond>> {
public:
    BondRiskHistoricalDataService();
    void PersistData(string persistKey, const PV01<Bond>& data) override;
private:
    void OnMessage(PV01<Bond>& data) override;
    BondRiskConnector* connector;
};

void BondRiskHistoricalDataService::PersistData(string persistKey, const PV01<Bond>& data) {
    connector->Publish(const_cast<PV01<Bond> &>(data));
}
BondRiskHistoricalDataService::BondRiskHistoricalDataService() {
    connector = new BondRiskConnector("risk.csv");
    connector->WriteHeader();
}

BondRiskConnector::BondRiskConnector(const string& filePath) : OutputFileConnector(filePath) {
}

string BondRiskConnector::toCSVString(PV01<Bond>& data) {
    std::ostringstream oss;
    oss << boost::posix_time::microsec_clock::universal_time() << "," <<
        data.GetProduct().GetProductId() << "," <<
        data.GetQuantity() << "," <<
        data.GetPV01();
    return oss.str();;
}
string BondRiskConnector::getCSVHeader() {
    return "Timestamp,CUSIP,Quantity,PV01";
}

BondRiskServiceListener::BondRiskServiceListener(HistoricalDataService<PV01<Bond>>* listeningService)
    : listeningService(
        listeningService) {}

void BondRiskServiceListener::ProcessAdd(PV01<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondRiskServiceListener::ProcessRemove(PV01<Bond>& data) {

}
void BondRiskServiceListener::ProcessUpdate(PV01<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondRiskHistoricalDataService::OnMessage(PV01<Bond>& data) {

}
#endif //BOND_RISK_HISTORICAL_DATA_SERVICE_HPP
