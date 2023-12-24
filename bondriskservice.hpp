/**
 * bondriskservice.hpp
 * 
 * This file defines the BondRiskService for a bond trading system. It is focused on assessing and managing the risk associated with bond positions. Key components include:
 * - 'BondRiskService': A service extending RiskService for bonds, responsible for calculating and updating the PV01 risk metric based on bond positions.
 * - 'BondPositionRiskServiceListener': A listener for the BondPositionService, which processes updates to bond positions and recalculates risk metrics accordingly.
 *
 * The service calculates PV01 (Price Value of a Basis Point), a common risk metric in fixed income trading, for individual bonds and aggregated sectors, providing essential risk management capabilities within the trading system.
 */

#ifndef BOND_RISK_SERVICE_HPP
#define BOND_RISK_SERVICE_HPP

#include "soa.hpp"
#include "products.hpp"
#include "streamingservice.hpp"
#include "riskservice.hpp"

class BondRiskService : public RiskService<Bond> {
public:
    BondRiskService() {}
    void OnMessage(PV01<Bond>& data) override {
        // Do nothing. Since streaming service does not have a connector.
    }

    /**
     * Update risk based on each products pre-determined pv01 values and their current positions.
     * @param position
     */
    void AddPosition(Position<Bond>& position) override {
        auto product = position.GetProduct();
        // source of PV01 values : https://eiptrading.com/risk-management/
        PV01<Bond> risk(product, position.GetAggregatePosition() * product.GetPV01(), position.GetAggregatePosition());
        if (dataStore.find(position.GetProduct().GetProductId()) == dataStore.end()) {
            dataStore.insert(make_pair(position.GetProduct().GetProductId(), risk));
            for (auto listener : this->GetListeners()) {
                listener->ProcessAdd(risk);
            }
        }
        else {
            dataStore.insert(make_pair(position.GetProduct().GetProductId(), risk));
            for (auto listener : this->GetListeners()) {
                listener->ProcessUpdate(risk);
            }
        }
    }

    /**
     * Aggregate the risk of all products that belong to a sector.
     * @param sector
     * @return
     */
    const PV01<BucketedSector<Bond>>& GetBucketedRisk(const BucketedSector<Bond>& sector) const override {
        double totalPV01 = 0;
        long totalPosition = 0;
        for (const auto& product : sector.GetProducts()) {
            if (dataStore.find(product.GetProductId()) != dataStore.end()) {
                totalPV01 += dataStore.at(product.GetProductId()).GetPV01();
                totalPosition += dataStore.at(product.GetProductId()).GetQuantity();
            }
        }
        auto pv01 = new PV01<BucketedSector<Bond>>(sector, totalPV01, totalPosition);
        return *pv01;
    }
};

class BondPositionRiskServiceListener : public ServiceListener<Position<Bond>> {
public:
    explicit BondPositionRiskServiceListener(BondRiskService* listeningService) : listeningService(listeningService) {}

    void ProcessAdd(Position<Bond>& data) override {
        listeningService->AddPosition(data);
    }
    void ProcessRemove(Position<Bond>& data) override {
        // NO-OP : Positions are never updated in this project.
    }
    void ProcessUpdate(Position<Bond>& data) override {
        // AddPosition updates the risk of existing positions.
        listeningService->AddPosition(data);
    }

private:
    BondRiskService* listeningService;

};
#endif //BOND_RISK_SERVICE_HPP