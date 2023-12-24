/**
 * inputfileconnector.hpp
 * 
 * This file defines the InputFileConnector template class for the bond trading system. It is designed to read and process data from input files for various services. Key features include:
 * - Template Parameters: K (Key type) and V (Value type) for the connected service.
 * - 'parse': A pure virtual function to be overridden by implementing classes for custom parsing logic.
 * - 'read': Opens and reads from the specified file, calling 'parse' for each line in the file.
 * - 'Publish': Overridden as a no-op, as this connector is intended only for data input, not output.
 *
 * The class is a crucial part of the system's data pipeline, enabling the integration of external data files into the trading system's various services.
 */

#ifndef INPUT_FILE_CONNECTOR_HPP
#define INPUT_FILE_CONNECTOR_HPP

#include <string>
#include <fstream>
#include <iostream>
#include "soa.hpp"

/**
 * This class is used to read data from files into services. Implementing classes should override the parse method.
 * Once the read method is called, it calls parse for each line of the input.
 *
 */
template<typename K, typename V>
class InputFileConnector : public Connector<V> {
private:
    string filePath;

protected:
    Service<K, V>* connectedService;

public:
    virtual void parse(string line) = 0;

    void Publish(V& data) override {
        //do nothing since this is a subscribe only connector.
    }

    void read() {
        ifstream inFile;
        string line;
        inFile.open(filePath);
        if (!inFile) {
            std::cerr << "Unable to open file " << filePath;
            exit(1);   // call system to stop
        }
        inFile >> line; // skip headers

        while (inFile >> line) {
            parse(line);
        }
        inFile.close();
    }

    InputFileConnector(const string& filePath, Service<K, V>* connectedService)
        : filePath(filePath), connectedService(connectedService) {
    }
};

#endif //INPUT_FILE_CONNECTOR_HPP