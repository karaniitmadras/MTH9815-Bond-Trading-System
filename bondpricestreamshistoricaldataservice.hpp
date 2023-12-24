/**
 * bondstreamshistoricaldataservice.hpp
 * 
 * This header file defines the structure and functionality for persisting historical data of bond price streams in a bond trading system. It includes:
 * - 'BondPriceStreamsServiceListener': A listener that processes additions, updates, and removals of PriceStream objects and forwards them to the historical data service.
 * - 'BondPriceStreamsConnector': An OutputFileConnector responsible for formatting PriceStream data into CSV strings and writing them to a file.
 * - 'BondPriceStreamsHistoricalDataService': A service that extends HistoricalDataService for PriceStream objects, managing the persistence of bond price stream data.
 *
 * The service's primary function is to capture and record the historical data of bond price streams, facilitating analysis and record-keeping in the bond trading system.
 */

#ifndef BOND_STREAMS_HISTORICAL_DATA_SERVICE_HPP
#define BOND_STREAMS_HISTORICAL_DATA_SERVICE_HPP

#include "historicaldataservice.hpp"
#include "products.hpp"
#include "OutputFileConnector.hpp"
#include "streamingservice.hpp"

/**
 * Listens to price stream updats from StreamingService
 * and writes them to output file streaming.csv
 */
class BondPriceStreamsServiceListener : public ServiceListener<PriceStream<Bond>> {
public:
    explicit BondPriceStreamsServiceListener(HistoricalDataService<PriceStream<Bond>>* listeningService);
    void ProcessRemove(PriceStream<Bond>& data) override;
    void ProcessUpdate(PriceStream<Bond>& data) override;
    void ProcessAdd(PriceStream<Bond>& data) override;
private:
    HistoricalDataService<PriceStream<Bond>>* listeningService;
};

class BondPriceStreamsConnector : public OutputFileConnector<PriceStream<Bond>> {
public:
    explicit BondPriceStreamsConnector(const string& filePath);
private:
    string toCSVString(PriceStream<Bond>& data) override;
    string getCSVHeader() override;
};

class BondPriceStreamsHistoricalDataService : public HistoricalDataService<PriceStream<Bond>> {
public:
    BondPriceStreamsHistoricalDataService();
    void PersistData(string persistKey, const PriceStream<Bond>& data) override;
private:
    void OnMessage(PriceStream<Bond>& data) override;
    BondPriceStreamsConnector* connector;
};

void BondPriceStreamsHistoricalDataService::PersistData(string persistKey, const PriceStream<Bond>& data) {
    connector->Publish(const_cast<PriceStream<Bond> &>(data));
}
BondPriceStreamsHistoricalDataService::BondPriceStreamsHistoricalDataService() {
    connector = new BondPriceStreamsConnector("streaming.csv");
    connector->WriteHeader();
}

BondPriceStreamsConnector::BondPriceStreamsConnector(const string& filePath) : OutputFileConnector(filePath) {
}

string BondPriceStreamsConnector::toCSVString(PriceStream<Bond>& data) {
    std::ostringstream oss;
    oss << boost::posix_time::microsec_clock::universal_time() << "," <<
        data.GetProduct().GetProductId() << "," <<
        data.GetBidOrder().GetPrice() << "," <<
        data.GetBidOrder().GetVisibleQuantity() << "," <<
        data.GetBidOrder().GetHiddenQuantity() << "," <<
        data.GetOfferOrder().GetPrice() << "," <<
        data.GetOfferOrder().GetVisibleQuantity() << "," <<
        data.GetOfferOrder().GetHiddenQuantity();
    return oss.str();
}
string BondPriceStreamsConnector::getCSVHeader() {
    return "Timestamp,CUSIP,BidPrice,BidVisibleQuantity,BidHiddenQuantity,OfferPrice,OfferVisibleQuantity,OfferHiddenQuantity";
}

BondPriceStreamsServiceListener::BondPriceStreamsServiceListener(HistoricalDataService<PriceStream<Bond>>* listeningService)
    : listeningService(
        listeningService) {}

void BondPriceStreamsServiceListener::ProcessAdd(PriceStream<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondPriceStreamsServiceListener::ProcessRemove(PriceStream<Bond>& data) {

}

void BondPriceStreamsServiceListener::ProcessUpdate(PriceStream<Bond>& data) {
    listeningService->PersistData(data.GetProduct().GetProductId(), data);
}

void BondPriceStreamsHistoricalDataService::OnMessage(PriceStream<Bond>& data) {

}
#endif //BOND_STREAMS_HISTORICAL_DATA_SERVICE_HPP