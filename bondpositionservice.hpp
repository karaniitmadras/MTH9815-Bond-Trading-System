/**
 * bondpositionservice.hpp
 * 
 * This file defines the BondPositionService and related listener for the bond trading system. Key functionalities include:
 * - 'BondPositionService': A service that extends PositionService for bonds. It manages positions in bonds by adding
 *   or updating them based on newly executed trades.
 * - 'AddTrade': Adds a new bond position or updates an existing one, triggered by a new trade execution.
 * - 'BondTradesServiceListener': Listens to the Trade<Bond> service and processes new trades by updating the bond positions
 *   through the BondPositionService.
 *
 * The BondPositionService is crucial for tracking the current positions in bonds, which is essential for portfolio management
 * and risk assessment in the bond trading system.
 */

#ifndef BOND_POSITION_SERVICE_HPP
#define BOND_POSITION_SERVICE_HPP

#include "positionservice.hpp"
#include "products.hpp"
#include "soa.hpp"

class BondPositionService : public PositionService<Bond> {
public:
    BondPositionService() {}

    /**
     * Add a new position or update a given position if it already exists.
     * @param trade a new trade that has been recently executed
     */
    void AddTrade(const Trade<Bond>& trade) override {
        if (dataStore.find(trade.GetProduct().GetProductId()) == dataStore.end()) {
            Position<Bond> newPosition = Position<Bond>(trade.GetProduct());
            newPosition.UpdatePosition(trade); // TODO: Move this to ctor?
            dataStore.insert(make_pair(trade.GetProduct().GetProductId(), newPosition));
            for (auto listener : this->GetListeners()) {
                listener->ProcessAdd(newPosition);
            }
        }
        else {
            auto position = dataStore.at(trade.GetProduct().GetProductId());
            position.UpdatePosition(trade);
            for (auto listener : this->GetListeners()) {
                listener->ProcessUpdate(position);
            }
        }
    }

    void OnMessage(Position<Bond>& data) override {

    }
};

class BondTradesServiceListener : public ServiceListener<Trade<Bond>> {
public:
    explicit BondTradesServiceListener(BondPositionService* listeningService) : listeningService(listeningService) {}

    void ProcessAdd(Trade<Bond>& data) override {
        listeningService->AddTrade(data);
    }

    void ProcessRemove(Trade<Bond>& data) override {
        // NO-OP : Trades are never removed in this project.
    }
    void ProcessUpdate(Trade<Bond>& data) override {
        // NO-OP : Trades are never updated in this project.
    }

private:
    BondPositionService* listeningService;
};

#endif //BOND_POSITION_SERVICE_HPP