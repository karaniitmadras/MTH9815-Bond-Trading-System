/**
 * bondpositionhistoricaldataservice.hpp
 * 
 * This file defines the BondPositionHistoricalDataService for the bond trading system, focusing on capturing and storing historical data of bond positions. It includes:
 * - 'BondPositionServiceListener': A listener that processes updates from the BondPositionService, forwarding them to the historical data service.
 * - 'BondPositionConnector': An OutputFileConnector for formatting Position<Bond> data into CSV strings and writing them to 'positions.csv'.
 * - 'BondPositionHistoricalDataService': A service that extends HistoricalDataService for Position<Bond> objects, handling the persistence of bond position data.
 *
 * The main goal of this service is to maintain a historical record of bond positions, crucial for portfolio tracking, compliance, and analysis within the trading system.
 */

#ifndef BOND_POSITION_HISTORICAL_DATA_SERVICE_HPP
#define BOND_POSITION_HISTORICAL_DATA_SERVICE_HPP

#include "historicaldataservice.hpp"
#include "products.hpp"
#include "OutputFileConnector.hpp"
#include "positionservice.hpp"

/**
 * Listens to updates in positions from BondPositionService.
 */
class BondPositionServiceListener : public ServiceListener<Position<Bond>> {
public:
    explicit BondPositionServiceListener(HistoricalDataService<Position<Bond>>* listeningService);
    void ProcessRemove(Position<Bond>& data) override;
    void ProcessUpdate(Position<Bond>& data) override;
private:
    HistoricalDataService<Position<Bond>>* listeningService;
    void ProcessAdd(Position<Bond>& data) override;
};

/**
 * Writes output to positions.csv
 */
class BondPositionConnector : public OutputFileConnector<Position<Bond>> {
public:
    explicit BondPositionConnector(const string& filePath);
private:
    string toCSVString(Position<Bond>& data) override;
    string getCSVHeader() override;
};

class BondPositionHistoricalDataService : public HistoricalDataService<Position<Bond>> {
public:
    BondPositionHistoricalDataService();
    void PersistData(string persistKey, const Position<Bond>& data) override;
private:
    void OnMessage(Position<Bond>& data) override;
    BondPositionConnector* connector;
};

void BondPositionHistoricalDataService::PersistData(string persistKey, const Position<Bond>& data) {
    connector->Publish(const_cast<Position<Bond> &>(data));
}
BondPositionHistoricalDataService::BondPositionHistoricalDataService() {
    connector = new BondPositionConnector("positions.csv");
    connector->WriteHeader();
}

BondPositionConnector::BondPositionConnector(const string& filePath) : OutputFileConnector(filePath) {
}

string BondPositionConnector::toCSVString(Position<Bond>& data) {
    std::ostringstream oss;
    oss << boost::posix_time::microsec_clock::universal_time() << "," <<
        data.GetProduct().GetProductId() << "," <<
        data.GetAggregatePosition();
    return oss.str();;
}
string BondPositionConnector::getCSVHeader() {
    return "Timestamp,CUSIP,Position";
}

BondPositionServiceListener::BondPositionServiceListener(HistoricalDataService<Position<Bond>>* listeningService)
    : listeningService(
        listeningService) {}

void BondPositionServiceListener::ProcessAdd(Position<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondPositionServiceListener::ProcessRemove(Position<Bond>& data) {

}
void BondPositionServiceListener::ProcessUpdate(Position<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondPositionHistoricalDataService::OnMessage(Position<Bond>& data) {

}
#endif //BOND_POSITION_HISTORICAL_DATA_SERVICE_HPP