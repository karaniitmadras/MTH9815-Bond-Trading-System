/**
 * bondmarketdataservice.hpp
 * 
 * This file defines the BondMarketDataService and related components for a bond trading system. It includes:
 * - 'BondMarketDataConnector': An InputFileConnector responsible for parsing bond market data from a file and updating the service.
 * - 'BondMarketDataService': A service that provides market data specifically for bonds. It includes methods to get the best bid/offer
 *   and aggregate depth of the order book.
 * - Functionality: The connector parses bond data from a CSV file, creating OrderBook objects. The service manages this data,
 *   offering access to the best bid/offer and aggregated depth information. It integrates with the overall bond trading system,
 *   providing crucial market data for trading decisions.
 */

#ifndef BOND_MARKET_DATA_SERVICE_HPP
#define BOND_MARKET_DATA_SERVICE_HPP

#include <sstream>
#include "products.hpp"
#include "marketdataservice.hpp"
#include "InputFileConnector.hpp"
#include "formatting.hpp"

class BondMarketDataConnector : public InputFileConnector<string, OrderBook<Bond>> {
public:
    BondMarketDataConnector(const string& filePath, Service<string, OrderBook<Bond>>* connectedService);
private:
    void parse(string line) override;
};

class BondMarketDataService : public MarketDataService<Bond> {
public:
    BondMarketDataService() {}
    const BidOffer& GetBestBidOffer(const string& productId) override;
    const OrderBook<Bond>& AggregateDepth(const string& productId) override;
    void Subscribe(BondMarketDataConnector* connector);
    void OnMessage(OrderBook<Bond>& data) override;
};

void BondMarketDataConnector::parse(string line) {
    auto split = splitString(line, ',');
    string id = split[0];
    auto bond = BondProductService::GetInstance()->GetData(id);
    vector<Order> bidStack;
    vector<Order> offerStack;
    for (int i = 1; i <= 5; ++i) {
        Order bid(convertFractionalPriceToDouble(split[2 * i - 1]), stol(split[2 * i]), PricingSide::BID);
        Order offer(convertFractionalPriceToDouble(split[9 + 2 * i]), stol(split[10 + 2 * i]), PricingSide::OFFER);
        bidStack.push_back(bid);
        offerStack.push_back(offer);
    }
    auto book = OrderBook<Bond>(bond, bidStack, offerStack);
    connectedService->OnMessage(book);
}

BondMarketDataConnector::BondMarketDataConnector(const string& filePath,
    Service<string, OrderBook<Bond>>* connectedService)
    : InputFileConnector(filePath, connectedService) {}

/**
 * Store the OrderBook and notify listeners to process the new state of the OrderBook.
 * @param data
 */
void BondMarketDataService::OnMessage(OrderBook<Bond>& data) {
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

void BondMarketDataService::Subscribe(BondMarketDataConnector* connector) {
    connector->read();
}

/**
 * Get the best bid and offer(with their price and quantity) in the current OrderBook.
 * @param productId
 * @return
 */
const BidOffer& BondMarketDataService::GetBestBidOffer(const string& productId) {
    if (!(dataStore.find(productId) == dataStore.end())) {
        OrderBook<Bond> orderBook = dataStore.at(productId);
        BidOffer* bidOffer = new BidOffer
        (Order(orderBook.GetBidStack()[0].GetPrice(), orderBook.GetBidStack()[0].GetQuantity(), PricingSide::BID),
            Order(orderBook.GetOfferStack()[0].GetPrice(),
                orderBook.GetOfferStack()[0].GetQuantity(),
                PricingSide::OFFER));
        return *bidOffer;
    }
}

/**
 * Aggregates the depth of a product in given OrderBook.
 * For volume, we calculate the sum of all individual orders.
 * For price, we calculate the Volume Weighted Average Price (VWAP).
 *
 * @param productId
 * @return
 */
const OrderBook<Bond>& BondMarketDataService::AggregateDepth(const string& productId) {
    if (!(dataStore.find(productId) == dataStore.end())) {
        OrderBook<Bond> orderBook = dataStore.at(productId);
        double totalBidCost = 0.0;
        long totalBidVolume = 0;
        double totalOfferCost = 0.0;
        long totalOfferVolume = 0;
        for (int i = 0; i < 5; ++i) {
            totalBidVolume += orderBook.GetBidStack()[i].GetQuantity();
            totalBidCost += orderBook.GetBidStack()[i].GetQuantity() * orderBook.GetBidStack()[i].GetPrice();
            totalOfferVolume += orderBook.GetOfferStack()[i].GetQuantity();
            totalOfferCost += orderBook.GetOfferStack()[i].GetQuantity() * orderBook.GetOfferStack()[i].GetPrice();
        }
        double averageBidPrice = totalBidVolume / totalBidCost;
        double averageOfferPrice = totalOfferVolume / totalOfferCost;
        vector<Order> aggregatedBidStack({ Order(averageBidPrice, totalBidVolume, PricingSide::BID) });
        vector<Order> aggregatedOfferStack({ Order(averageOfferPrice, totalOfferVolume, PricingSide::OFFER) });
        OrderBook<Bond>
            * aggregateOrderBook = new OrderBook<Bond>(orderBook.GetProduct(), aggregatedBidStack, aggregatedOfferStack);
        return *aggregateOrderBook;
    }
}

#endif //BOND_MARKET_DATA_SERVICE_HPP