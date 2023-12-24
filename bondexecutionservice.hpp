/**
 * bondexecutionhistoricaldataservice.hpp
 * 
 * This header file defines components for managing and persisting historical data of bond execution orders in a bond trading system. It includes:
 * - 'BondExecutionOrderServiceListener': A listener class for the ExecutionOrder service, responsible for processing and forwarding execution order data to the historical data service.
 * - 'BondExecutionOrderConnector': Responsible for outputting ExecutionOrder data to a CSV file, including functions for formatting data as CSV strings.
 * - 'BondExecutionHistoricalDataService': A service that extends the HistoricalDataService for ExecutionOrder, handling the persistence of bond execution order data to a file.
 *
 * The main functionality revolves around capturing bond execution orders, converting them to a CSV format, and writing them to 'executions.csv'. This allows for a historical record of all bond transactions executed in the system.
 */
#ifndef BOND_EXECUTION_SERVICE_HPP
#define BOND_EXECUTION_SERVICE_HPP

#include "bondalgoexecutionservice.hpp"
#include "soa.hpp"
#include "products.hpp"
#include "executionservice.hpp"

class BondExecutionService : public ExecutionService<Bond> {
public:
    BondExecutionService() {}
    void OnMessage(ExecutionOrder<Bond>& data) override {
        // Do nothing. Since streaming service does not have a connector.
    }

    // Execute an order and notify listeners.
    void ExecuteOrder(const ExecutionOrder<Bond>& order, Market market) override {
        for (auto listener : this->GetListeners()) {
            listener->ProcessAdd(const_cast<ExecutionOrder<Bond> &>(order));
        }
    }
};

class BondAlgoExecutionServiceListener : public ServiceListener<AlgoExecution<Bond>> {
public:
    explicit BondAlgoExecutionServiceListener(BondExecutionService* listeningService)
        : listeningService(listeningService) {}

    // Execute a given order on the CME market by default.
    void ProcessAdd(AlgoExecution<Bond>& data) override {
        listeningService->ExecuteOrder(data.getExecutionOrder(), Market::CME);
    }
    void ProcessRemove(AlgoExecution<Bond>& data) override {

    }
    void ProcessUpdate(AlgoExecution<Bond>& data) override {

    }

private:
    BondExecutionService* listeningService;

};

#endif//BOND_EXECUTION_SERVICE_HPP