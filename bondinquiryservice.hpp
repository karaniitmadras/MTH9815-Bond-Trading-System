/**
 * bondinquiryservice.hpp
 * 
 * This header file defines the structure and functionality for handling inquiries in a bond trading system. Key components include:
 * - 'BondInquirySubscriber': An InputFileConnector to read inquiries from a file and update the InquiryService with new data.
 * - 'BondInquiryPublisher': An OutputFileConnector to output Inquiry data to a CSV file, including formatting methods for CSV strings.
 * - 'BondInquiryService': Manages bond inquiries, handling incoming messages, sending quotes, and publishing completed inquiries.
 * - 'BondInquiryServiceListener': A listener for the BondInquiryService, processing added, removed, or updated inquiries.
 *
 * The service facilitates the management of bond inquiries, enabling interaction between market participants and the bond market.
 */

#ifndef BOND_INQUIRY_SERVICE_HPP
#define BOND_INQUIRY_SERVICE_HPP

#include "products.hpp"
#include "inquiryservice.hpp"
#include "InputFileConnector.hpp"
#include "OutputFileConnector.hpp"

class BondInquirySubscriber : public InputFileConnector<string, Inquiry<Bond>> {
public:
    BondInquirySubscriber(const string& filePath, Service<string, Inquiry<Bond>>* connectedService) : InputFileConnector(
        filePath,
        connectedService) {}

private:
    void parse(string line) override {
        auto split = splitString(line, ',');
        string productId = split[0], inquiryId = split[1];
        Side side = (split[2].compare("0") == 0) ? BUY : SELL;
        long quantity = stol(split[3]);
        auto bond = BondProductService::GetInstance()->GetData(productId);
        auto inquiry = Inquiry<Bond>(inquiryId, bond, side, quantity, 0.0, InquiryState::RECEIVED);
        connectedService->OnMessage(inquiry);
    }
};

class BondInquiryPublisher : public OutputFileConnector<Inquiry<Bond>> {
public:
    explicit BondInquiryPublisher(const string& filePath) : OutputFileConnector(filePath) {}

    string toCSVString(Inquiry<Bond>& data) override {
        std::ostringstream oss;
        oss << boost::posix_time::microsec_clock::universal_time() << "," <<
            data.GetInquiryId() << "," <<
            data.GetProduct().GetProductId() << "," <<
            data.GetSide() << "," <<
            data.GetQuantity() << "," <<
            data.GetPrice() << "," <<
            data.GetState();
        return oss.str();
    }

    string getCSVHeader() override {
        return "Timestamp,InquiryId,ProductId,Side,Quantity,Price,State";
    }
};

class BondInquiryService : public InquiryService<Bond> {
public:
    BondInquiryService() {
        publishConnector = new BondInquiryPublisher("allinquires.csv");
        publishConnector->WriteHeader();
    }

    void OnMessage(Inquiry<Bond>& data) override {
        // Store the inquiry
        dataStore.insert(make_pair(data.GetInquiryId(), data));

        if (data.GetState() == InquiryState::RECEIVED) {
            // If this is a new inquiry, send a quote.
            SendQuote(data.GetInquiryId(), 100.0);
        }
        else if (data.GetState() == InquiryState::QUOTED) {
            // If this is already quoted, then mark as completed and update listeners.
            data.SetState(InquiryState::DONE);
            publishConnector->Publish(data);
            for (auto listener : this->GetListeners()) {
                listener->ProcessUpdate(data);
            }
        }
    }

    /**
     * Updates a quote with the current running price and sends it to listeners.
     * @param inquiryId
     * @param price
     */
    void SendQuote(const string& inquiryId, double price) override {
        auto data = dataStore.at(inquiryId);
        data.SetPrice(price);
        for (auto listener : this->GetListeners()) {
            listener->ProcessAdd(data);
        }
    }

    void Subscribe(BondInquirySubscriber* subscribeConnector) {
        subscribeConnector->read();
    }

private:
    BondInquiryPublisher* publishConnector;
};

class BondInquiryServiceListener : public ServiceListener<Inquiry<Bond>> {
public:
    explicit BondInquiryServiceListener(BondInquiryService* listeningService) : listeningService(listeningService) {}

    void ProcessAdd(Inquiry<Bond>& data) override {
        // Short-circuit and send this quote back to the inquiry service.
        // Acting on behalf of market.
        data.SetState(InquiryState::QUOTED);
        listeningService->OnMessage(data);
    }

    void ProcessRemove(Inquiry<Bond>& data) override {

    }

    void ProcessUpdate(Inquiry<Bond>& data) override {

    }

private:
    BondInquiryService* listeningService;
};
#endif //BOND_INQUIRY_SERVICE_HPP