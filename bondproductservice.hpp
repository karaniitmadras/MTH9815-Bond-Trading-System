/**
 * bondproductservice.hpp
 * 
 * This file defines the BondProductService, a singleton service responsible for managing bond product data within the bond trading system. Key features include:
 * - 'BondProductService': A service that extends the base Service class for bond products. It maintains a reference data set of bond securities, with functionality to retrieve and add bond data.
 * - 'GetData': Retrieves bond data for a given product identifier (productId).
 * - 'Add': Adds a new bond to the service's internal data set.
 * - 'GetBonds': Returns all bonds matching a specified ticker.
 * - Singleton Pattern: Ensures a single instance of the BondProductService is created, accessible via 'GetInstance' method.
 *
 * This service acts as the central repository for bond product information, crucial for various trading and risk management operations within the system.
 */

#ifndef BOND_PRODUCT_SERVICE_HPP
#define BOND_PRODUCT_SERVICE_HPP

/**
 * Defines Bond and IRSwap ProductServices
 */

#include <iostream>
#include <map>
#include "products.hpp"
#include "soa.hpp"

 /**
  * Bond Product Service to own reference data over a set of bond securities.
  * Key is the productId string, value is a Bond.
  */
class BondProductService : public Service<string, Bond> {

public:
    static BondProductService* GetInstance();

    // Return the bond data for a particular bond product identifier
    Bond& GetData(string productId) override;

    // Add a bond to the service (convenience method)
    void Add(Bond& bond);

    // Get all Bonds with the specified ticker.
    vector<Bond> GetBonds(string& _ticker);
    void OnMessage(Bond& data) override;

private:
    unordered_map<string, Bond> bondMap; // cache of bond products
    static BondProductService* instance; // reference to singleton instance

    // Private ctor to disallow direct initialization.
    BondProductService();
};

BondProductService::BondProductService() {
    bondMap = unordered_map<string, Bond>();
}

void BondProductService::OnMessage(Bond& data) {

}

Bond& BondProductService::GetData(string productId) {
    return bondMap[productId];
}

void BondProductService::Add(Bond& bond) {
    bondMap.insert(make_pair(bond.GetProductId(), bond));
}

/**
 * Get all bonds with a specified ticker.
 *
 * @param _ticker A string representing the ticker of the bond to search for.
 * @return A vector of matching bond objects.
 */
vector<Bond> BondProductService::GetBonds(string& _ticker) {
    auto bonds = vector<Bond>();
    for (const auto& entry : bondMap) {
        if (entry.second.GetTicker() == _ticker) {
            bonds.push_back(entry.second);
        }
    }
    return bonds;
}

BondProductService* BondProductService::GetInstance() {
    if (!instance) {
        instance = new BondProductService;
    }
    return instance;
}

BondProductService* BondProductService::instance = 0;

#endif //BOND_PRODUCT_SERVICE_HPP