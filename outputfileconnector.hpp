/**
 * outputfileconnector.hpp
 * 
 * This file defines the OutputFileConnector template class for the bond trading system. It is designed to write data to output files for various services. Key features include:
 * - Template Parameter: V (Type of the data that is being written).
 * - 'Publish': Writes the data to the output file by converting it to a string format (CSV).
 * - 'WriteHeader': Writes a header to the output file. It's used for initial file setup or when truncating existing file contents.
 * - 'toCSVString': A pure virtual function that must be implemented by derived classes to convert data objects into CSV string format.
 * - 'getCSVHeader': A pure virtual function to provide a CSV header string.
 *
 * The class serves as a fundamental part of the system's data output process, enabling the exportation of processed data to external files.
 */

#ifndef OUTPUT_FILE_CONNECTOR_HPP
#define OUTPUT_FILE_CONNECTOR_HPP

#include <string>
#include <fstream>
#include <iostream>
#include "soa.hpp"
/**
 * This connector writes data to output files.
 * Implementing classes have to implement a method to convert the object into string form,
 * as well as an optional header method.
 * If a file already exists, its contents are truncated.
 * If a file does not exist, a new one is created.
 *
 */
template<typename V>
class OutputFileConnector : public Connector<V> {
private:
    string filePath;

    void appendLineToFile(string line, bool newFile) {
        ofstream outFile;
        outFile.open(filePath, newFile ? ios_base::trunc : ios_base::app);
        if (!outFile) {
            cerr << "Unable to open file " << filePath;
            exit(1);   // call system to stop
        }
        outFile << line << endl;
        outFile.close();
    }

public:
    void Publish(V& data) override {
        appendLineToFile(toCSVString(data), false);
    }

    void WriteHeader() {
        appendLineToFile(getCSVHeader(), true);
    }

    virtual string toCSVString(V& data) = 0;
    virtual string getCSVHeader() = 0;

    explicit OutputFileConnector(const string& filePath) : filePath(filePath) {}
};

#endif //OUTPUT_FILE_CONNECTOR_HPP