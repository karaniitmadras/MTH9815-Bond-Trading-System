/**
 * formatting.hpp
 *
 * This utility header file defines functions for string manipulation and conversion, specifically tailored for a bond trading system. Key functions include:
 * - 'splitString': Splits a given string into substrings based on a specified delimiter. Useful for parsing CSV or similarly formatted data.
 * - 'convertFractionalPriceToDouble': Converts bond prices from a fractional representation (common in bond markets) to a double value.
 *   This function is particularly important for processing bond prices which are often quoted in fractions.
 *
 * These utility functions are crucial for data parsing and conversion in various parts of the bond trading system, ensuring accurate and efficient data handling.
 */

#ifndef FORMATTING_HPP
#define FORMATTING_HPP

#include<vector>
#include<string>
#include<sstream>

using namespace std;

/**
 * Split a given string into parts separated by a single delimiter character.
 *
 * @param input the string that has to be split.
 * @param delimiter the character by which the string is split.
 * @return a vector of strings resulting from splitting input at each occurence of delimiter char.
 */
vector<string> splitString(string input, char delimiter) {
    stringstream inputstream(input);
    string element;
    vector<string> result;
    while (getline(inputstream, element, delimiter)) {
        result.push_back(element);
    }
    return result;
}

/**
 * Converts a given fractional representation of price into a numerical value.
 *
 * @param price price in fractional form i.e. 100-xyz
 * @return double value of the given price
 */
double convertFractionalPriceToDouble(string price) {
    auto split = splitString(price, '-');
    if (split.size() != 2) {
        //invalid input
        return 0.0;
    }
    double integerPart = stod(split[0]);
    double firstFractionalPart = stod(split[1].substr(0, 2)) / 32.0;
    double secondFractionalPart = ((split[1][2] == '+') ? 4 : (split[1][2] - '0')) / 256.0;
    return integerPart + firstFractionalPart + secondFractionalPart;
}
#endif //FORMATTING_HPP