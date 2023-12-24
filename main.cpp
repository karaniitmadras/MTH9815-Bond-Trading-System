/**
 * main.cpp
 * This file is the main entry point for a bond trading system simulation, orchestrating various components and workflows. It includes:
 * - setupProducts: Initializes a range of bond products and adds them to the BondProductService.
 * - runTradesAndExecutionFlow: Sets up trade booking, position management, risk assessment services, and their historical data services.
 *   It integrates external trade and market data through file connectors.
 * - runInquiryFlow: Establishes the bond inquiry service and its listener, linking to an external inquiries data source.
 * - runStreamingFlow: Implements services for bond pricing, GUI updates, algorithmic streaming, streaming services,
 *   and a historical data service for price streams. It also connects to an external bond prices file for data input.
 *
 * Each workflow demonstrates a specific aspect of bond trading operations, including market data processing, trade execution,
 * risk management, client inquiries handling, and updating the user interface.
 */

#include "BondPricingService.hpp"
#include "GUIService.hpp"
#include "BondAlgoStreamingService.hpp"
#include "BondStreamingService.hpp"
#include "BondPriceStreamsHistoricalDataService.hpp"
#include "BondInquiryService.hpp"
#include "BondTradeBookingService.hpp"
#include "BondPositionService.hpp"
#include "BondPositionHistoricalDataService.hpp"
#include "BondRiskHistoricalDataService.hpp"
#include "BondRiskService.hpp"
#include "BondMarketDataService.hpp"
#include "BondAlgoExecutionService.hpp"
#include "BondExecutionService.hpp"
#include "BondExecutionHistoricalDataService.hpp"

void setupProducts();
void runStreamingFlow();
void runInquiryFlow();
void runTradesAndExecutionFlow();

int main() {
    setupProducts();
    runStreamingFlow();
    runInquiryFlow();
    runTradesAndExecutionFlow();
}

void setupProducts() {
    auto productService = BondProductService::GetInstance();

    Bond T2("9128283H1", CUSIP, "T", 1.750, date(2019, Nov, 30), 0.019851);
    Bond T3("9128283L2", CUSIP, "T", 1.875, date(2020, Dec, 15), 0.029309);
    Bond T5("912828M80", CUSIP, "T", 2.0, date(2022, Nov, 30), 0.048643);
    Bond T7("9128283J7", CUSIP, "T", 2.125, date(2024, Nov, 30), 0.065843);
    Bond T10("9128283F5", CUSIP, "T", 2.25, date(2027, Dec, 15), 0.087939);
    Bond T30("912810RZ3", CUSIP, "T", 2.75, date(2047, Dec, 15), 0.184698);

    productService->Add(T2);
    productService->Add(T3);
    productService->Add(T5);
    productService->Add(T7);
    productService->Add(T10);
    productService->Add(T30);
}

void runTradesAndExecutionFlow() {
    auto tradeBookingService = new BondTradeBookingService();
    auto positionService = new BondPositionService();
    auto riskService = new BondRiskService();
    auto positionHistoricalDataService = new BondPositionHistoricalDataService();
    auto riskHistoricalDataService = new BondRiskHistoricalDataService();

    auto tradeListener = new BondTradesServiceListener(positionService);
    auto positionListener = new BondPositionServiceListener(positionHistoricalDataService);
    auto positionListenerFromRisk = new BondPositionRiskServiceListener(riskService);
    auto riskListener = new BondRiskServiceListener(riskHistoricalDataService);

    tradeBookingService->AddListener(tradeListener);
    positionService->AddListener(positionListener);
    positionService->AddListener(positionListenerFromRisk);
    riskService->AddListener(riskListener);

    auto marketDataService = new BondMarketDataService();
    auto algoExecutionService = new BondAlgoExecutionService();
    auto executionService = new BondExecutionService();
    auto executionHistoricalDataService = new BondExecutionHistoricalDataService();

    auto marketDataListener = new BondMarketDataServiceListener(algoExecutionService);
    auto algoExecutionListener = new BondAlgoExecutionServiceListener(executionService);
    auto executionListener = new BondExecutionOrderServiceListener(executionHistoricalDataService);
    auto executionListenerFromTrade = new BondExecutionServiceListener(tradeBookingService);

    marketDataService->AddListener(marketDataListener);
    algoExecutionService->AddListener(algoExecutionListener);
    executionService->AddListener(executionListener);
    executionService->AddListener(executionListenerFromTrade);

    std::cout << "Processing trades.csv" << std::endl;
    tradeBookingService->Subscribe(new BondTradesConnector("trades.csv", tradeBookingService));

    std::cout << "Processing marketdata.csv" << std::endl;
    marketDataService->Subscribe(new BondMarketDataConnector("marketdata.csv", marketDataService));
}

void runInquiryFlow() {
    auto inquiryService = new BondInquiryService();
    auto inquiryServiceListener = new BondInquiryServiceListener(inquiryService);
    inquiryService->AddListener(inquiryServiceListener);

    std::cout << "Processing inquiries.csv" << std::endl;
    inquiryService->Subscribe(new BondInquirySubscriber("inquiries.csv", inquiryService));
}

void runStreamingFlow() {
    auto pricingService = new BondPricingService();
    auto guiService = new GUIService(300);
    auto algoStreamingService = new BondAlgoStreamingService();
    auto streamingService = new BondStreamingService();
    auto historicalDataService = new BondPriceStreamsHistoricalDataService();

    auto guiServiceListener = new BondPriceServiceListener(guiService);
    auto algoStreamingServiceListener = new BondPricesServiceListener(algoStreamingService);
    auto streamingServiceListener = new BondAlgoStreamServiceListener(streamingService);
    auto historicalDataServiceListener = new BondPriceStreamsServiceListener(historicalDataService);

    pricingService->AddListener(guiServiceListener);
    pricingService->AddListener(algoStreamingServiceListener);
    algoStreamingService->AddListener(streamingServiceListener);
    streamingService->AddListener(historicalDataServiceListener);

    std::cout << "Processing prices.csv" << std::endl;
    pricingService->Subscribe(
        new BondPricesConnector("prices.csv", pricingService));
}
