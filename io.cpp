/** Input and Output functionality:
    • Input order placement commands
    • Output order
    • Output trade, resulting from order execution
    • Output Order Book
*/
#include <iostream>
#include <string>
#include <vector>
#include "all-includes.h"

using namespace std;

/**
 * @brief Output OrderBook
 * Allows printing of the `OrderBook` to `cout`
 */
std::ostream & operator<<(std::ostream & out, const OrderBook & book) {
    // Output Header
    out << "+-----------------------------------------------------------------+" << endl;
    out << "| BUY                            | SELL                           |" << endl;
    out << "| Id       | Volume      | Price | Price | Volume      | Id       |" << endl;
    out << "+----------+-------------+-------+-------+-------------+----------+" << endl;

    // Output Entries
    const vector<Order> & buy = book.getBuyLedger();
    const vector<Order> & sell = book.getSellLedger();
    const size_t size = max({buy.size(), sell.size()});
    for (size_t i = 0; i < size; ++i) {
        out << '|' << setw(10) << (i < buy.size() ? to_string(buy[i].id) : "");
        out << '|' << setw(13) << (i < buy.size() ? thousandsSeparated(buy[i].volume) : "");
        out << '|' << setw(7) << (i < buy.size() ? thousandsSeparated(buy[i].price) : "");
        out << '|' << setw(7) << (i < sell.size() ? thousandsSeparated(sell[i].price) : "");
        out << '|' << setw(13) << (i < sell.size() ? thousandsSeparated(sell[i].volume) : "");
        out << '|' << setw(10) << (i < sell.size() ? to_string(sell[i].id) : "");
        out << '|' << endl;
    }

    // Output Footer
    out << "+-----------------------------------------------------------------+" << endl;

    // Return stream
    return out;
}


/**
 * @brief Parses a string containing a single command for placing an order.
 * Performs only limited validity checking for input format.
 * If input is not recognized: Does not throw, logs errors to `stderr` and returns `nullopt`.
 *
 * @return optional<Order> - `nullopt` if the command is empty, is a comment or is malformed
 */
optional<Order> parsePlaceOrderCommand(string command) {
    regex orderPattern("\\s*([SB]),\\s*(\\d+),\\s*(\\d+),\\s*(\\d+),?\\s*(\\d+)?.*");
    regex commentPattern("\\s*#.*"); // Matches a comment
    regex emptyPattern("\\s*"); // Matches an emtpy line
    smatch orderMatch; // Contains order bits as RegEx capture groups, if `command` matches order pattern
    if (regex_match(command, orderMatch, orderPattern)) {
        // `command` matches order pattern
        TradeType type = orderMatch[1].str()[0] == BUY ? BUY : SELL;
        unsigned int id = stoi(orderMatch[2]);
        unsigned short price = stoi(orderMatch[3]);
        unsigned int volume = stoi(orderMatch[4]);
        unsigned int peakMax = orderMatch[5] == "" ? 0 : stoi(orderMatch[5]);
        if (peakMax) { // Iceberg Order detected
            return Order{type, id, price, volume, peakMax};
        } else { // Limit Order detected
            return Order{type, id, price, volume, 0};
        }
    } else if (regex_match(command, commentPattern)) {
        return nullopt; // `command` is a comment
    } else if (regex_match(command, emptyPattern)) {
        return nullopt; // `command` is empty
    } else { // `command` is malformed
        cerr << "Unrecognized command: " << command << endl;
        return nullopt;
    }
}

/**
 * @brief Input Order
 * Allows the use of `>>` operator with Input Streams
 */
std::istream & operator >>(std::istream & inputStream, Order & order) {
    string line; // To read each line
    optional<Order> maybeOrder = nullopt;
    while (getline(inputStream, line) && !(maybeOrder = parsePlaceOrderCommand(line))) {/* Ignore input */}
    if ( maybeOrder ) order = maybeOrder.value();
    return inputStream;
}

/**
 * @brief Output order
 * Allows the use of `<<` operator with Output Streams
 */
std::ostream & operator <<(std::ostream & outputStream, const Order & order) {
    outputStream << (order.type == BUY ? "Buy" : "Sell") << ' ';
    outputStream << thousandsSeparated(order.volume + order.hiddenVolume) << " units ";
    if ( order.maxPeakSize ) {
        outputStream << "with peaks of " << thousandsSeparated(order.maxPeakSize) << ' ';
    }
    outputStream << "@ " << thousandsSeparated(order.price) << 'p' << endl;

    return outputStream;
}
