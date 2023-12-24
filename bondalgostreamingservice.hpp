/**
 * bondalgostreamingservice.hpp
 
 * This file defines the BondAlgoStreamingService for a bond trading system, focusing on algorithmically generating and managing price streams. Key components include:
 * - 'AlgoStream': A template class that encapsulates a PriceStream object, representing an algorithmically generated stream of prices for a financial product.
 * - 'BondAlgoStreamingService': A service that generates and publishes new price streams for bonds, using an alternating volume strategy.
 * - 'BondPricesServiceListener': A listener for the BondPricingService, responsible for updating the BondAlgoStreamingService with new price data.
 *
 * The service aims to provide dynamic and algorithm-driven price streams for bonds, enhancing the trading system's responsiveness and market adaptability.
 */

#ifndef BOND_ALGO_STREAMING_SERVICE_HPP
#define BOND_ALGO_STREAMING_SERVICE_HPP

#include "pricingservice.hpp"
#include "products.hpp"
#include "soa.hpp"
#include "streamingservice.hpp"

template<typename T>
class AlgoStream {
public:
    explicit AlgoStream(const PriceStream<T>& priceStream) : priceStream(priceStream) {}

    const PriceStream<T>& getPriceStream() const {
        return priceStream;
    }

private:
    const PriceStream<T>& priceStream;
};

class BondAlgoStreamingService : public Service<string, AlgoStream<Bond>> {
public:
    BondAlgoStreamingService() {}

    /**
     * Publish a new price stream alternating between volumes of 1000000 & 2000000.
     * @param newPrice
     */
    void PublishPrice(Price<Bond>& newPrice) {
        auto bond = BondProductService::GetInstance()->GetData(newPrice.GetProduct().GetProductId());

        PriceStreamOrder bidOrder
        (newPrice.GetMid() - newPrice.GetBidOfferSpread() / 2,
            states[currentState],
            2 * states[currentState],
            PricingSide::BID);
        PriceStreamOrder offerOrder
        (newPrice.GetMid() + newPrice.GetBidOfferSpread() / 2,
            states[currentState],
            2 * states[currentState],
            PricingSide::OFFER);
        PriceStream<Bond> priceStream(bond, bidOrder, offerOrder);
        AlgoStream<Bond> algoStream(priceStream);

        cycleState();
        dataStore.insert(make_pair(bond.GetProductId(), algoStream));
        if (dataStore.find(bond.GetProductId()) == dataStore.end()) {
            for (auto listener : this->GetListeners()) {
                listener->ProcessAdd(algoStream);
            }
        }
        else {
            for (auto listener : this->GetListeners()) {
                listener->ProcessUpdate(algoStream);
            }
        }
    }

    void OnMessage(AlgoStream<Bond>& data) override {

    }

private:
    std::array<int, 2> states = { {1000000, 2000000} };
    unsigned int currentState = 0;
    void cycleState() {
        currentState = (currentState + 1) % states.size();
    }
};

class BondPricesServiceListener : public ServiceListener<Price<Bond>> {
public:
    explicit BondPricesServiceListener(BondAlgoStreamingService* listeningService) : listeningService(listeningService) {}

    void ProcessAdd(Price<Bond>& data) override {
        listeningService->PublishPrice(data);
    }

    void ProcessRemove(Price<Bond>& data) override {

    }

    void ProcessUpdate(Price<Bond>& data) override {
        listeningService->PublishPrice(data);
    }

private:
    BondAlgoStreamingService* listeningService;
};

#endif //BOND_ALGO_STREAMING_SERVICE_HPP