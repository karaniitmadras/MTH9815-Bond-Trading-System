/**
 * guiservice.hpp
 * 
 * This file defines the GUIService for the bond trading system, which is responsible for displaying bond prices on a GUI and managing data output. Key components include:
 * - 'GUIConnector': An OutputFileConnector that formats Price<Bond> data into CSV strings and writes them to 'gui.csv'.
 * - 'GUIService': A service that throttles the frequency of data updates to the GUI to prevent flooding. It only updates once every specified interval (default 300ms).
 * - 'BondPriceServiceListener': Listens to the BondPriceService and forwards updates to the GUIService for display and logging.
 *
 * The primary purpose of this service is to interface with a GUI, ensuring the display of bond prices is both current and manageable in terms of update frequency.
 */

#ifndef GUI_SERVICE_HPP
#define GUI_SERVICE_HPP

#include "products.hpp"
#include "pricingservice.hpp"
#include "outputfileconnector.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * This is the interface to display prices to any GUI.
 * Writes data to gui.csv
 */
class GUIConnector : public OutputFileConnector<Price<Bond>> {
public:
    GUIConnector(const string& filePath);
    string toCSVString(Price<Bond>& data) override;
    string getCSVHeader() override;
};

class GUIService : public Service<string, Price<Bond>> {
public:
    GUIService(const unsigned int throttle);
    void PersistData(Price<Bond>& data);
    void OnMessage(Price<Bond>& data) override;

private:
    // defined in milliseconds
    const unsigned int throttle = 300;
    GUIConnector* connector;
    boost::posix_time::ptime lastTick = boost::posix_time::microsec_clock::universal_time();
};

class BondPriceServiceListener : public ServiceListener<Price<Bond>> {
public:
    BondPriceServiceListener(GUIService* listeningService);
private:
    void ProcessAdd(Price<Bond>& data) override;
    void ProcessRemove(Price<Bond>& data) override;
    void ProcessUpdate(Price<Bond>& data) override;
    GUIService* listeningService;
};

BondPriceServiceListener::BondPriceServiceListener(GUIService* listeningService) : listeningService(listeningService) {}

void BondPriceServiceListener::ProcessAdd(Price<Bond>& data) {
    listeningService->PersistData(data);
}
void BondPriceServiceListener::ProcessRemove(Price<Bond>& data) {

}
void BondPriceServiceListener::ProcessUpdate(Price<Bond>& data) {
    listeningService->PersistData(data);
}

GUIConnector::GUIConnector(const string& filePath) : OutputFileConnector(filePath) {}

string GUIConnector::toCSVString(Price<Bond>& data) {
    std::ostringstream oss;
    // Calculate bid and offer prices from mid and spread.
    oss << boost::posix_time::microsec_clock::universal_time() << "," <<
        data.GetProduct().GetProductId() << "," <<
        (data.GetMid() - data.GetBidOfferSpread() / 2) << "," <<
        (data.GetMid() + data.GetBidOfferSpread() / 2);
    return oss.str();
}
string GUIConnector::getCSVHeader() {
    return "Timestamp,CUSIP,BidPrice,OfferPrice";
}

/**
 * Store data only once every 300ms. Any updates within that interval are discarded.
 *
 * @param data
 */
void GUIService::PersistData(Price<Bond>& data) {
    auto currentTick = boost::posix_time::microsec_clock::universal_time();
    boost::posix_time::time_duration diff = currentTick - lastTick;
    if (diff.total_milliseconds() > 300) {
        connector->Publish(data);
        lastTick = currentTick;
    }
}

void GUIService::OnMessage(Price<Bond>& data) {

}

GUIService::GUIService(const unsigned int throttle) : throttle(throttle) {
    connector = new GUIConnector("gui.csv");
    connector->WriteHeader();
}

#endif //GUI_SERVICE_HPP