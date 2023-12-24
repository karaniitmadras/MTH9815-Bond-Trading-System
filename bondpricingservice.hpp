/**
 * bondpricingservice.hpp
 * 
 * This file defines the BondPricingService for a bond trading system, which is responsible for processing and updating bond prices. Key components include:
 * - 'BondPricesConnector': An InputFileConnector that reads and parses bond price data from 'prices.csv', and updates the pricing service with new data.
 * - 'BondPricingService': A service that extends PricingService for bonds, managing the processing and storage of bond price data.
 *
 * The service aims to maintain an up-to-date record of bond prices, essential for accurate and effective trading and valuation within the bond trading system.
 */

#ifndef BOND_PRICING_SERVICE_HPP
#define BOND_PRICING_SERVICE_HPP

#include <sstream>
#include "products.hpp"
#include "pricingservice.hpp"
#include "InputFileConnector.hpp"
#include "formatting.hpp"
#include "bondproductservice.hpp"

/**
 * Reads data from prices.csv
 */
class BondPricesConnector : public InputFileConnector<string, Price<Bond>> {
public:
    BondPricesConnector(const string& filePath, Service<string, Price<Bond>>* connectedService);
private:
    void parse(string line) override;
};

/**
 * Processes prices.csv
 */
class BondPricingService : public PricingService<Bond> {
public:
    BondPricingService() {}
    void Subscribe(BondPricesConnector* connector);
    void OnMessage(Price<Bond>& data) override;
};

void BondPricesConnector::parse(string line) {
    auto split = splitString(line, ',');
    string id = split[0];
    double mid = convertFractionalPriceToDouble(split[1]), bidOfferSpread = convertFractionalPriceToDouble(split[2]);

    auto bond = BondProductService::GetInstance()->GetData(id);
    auto price = Price<Bond>(bond, mid, bidOfferSpread);
    connectedService->OnMessage(price);
}

BondPricesConnector::BondPricesConnector(const string& filePath, Service<string, Price<Bond>>* connectedService)
    : InputFileConnector(filePath, connectedService) {}

/**
 * Store the new price and update all listeners.
 *
 * @param data
 */
void BondPricingService::OnMessage(Price<Bond>& data) {
    if (dataStore.find(data.GetProduct().GetProductId()) == dataStore.end()) {
        dataStore.insert(make_pair(data.GetProduct().GetProductId(), data));
        for (auto listener : this->GetListeners()) {
            listener->ProcessAdd(data);
        }
    }
    else {
        dataStore.insert(make_pair(data.GetProduct().GetProductId(), data));
        for (auto listener : this->GetListeners()) {
            listener->ProcessUpdate(data);
        }
    }
}

void BondPricingService::Subscribe(BondPricesConnector* connector) {
    connector->read();
}
#endif //BOND_PRICING_SERVICE_HPP