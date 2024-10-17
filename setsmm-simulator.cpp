#include <iostream>
#include "all-includes.h"

using namespace std;

int main() {
    Order order;
    OrderBook book;
    while (cin >> order) { // Read next order
        if (SHOW_ORDERS) cout << order; // May output order, useful during testing

        // Execute order and get back resulting trades
        vector<Trade> trades = book.insertAggressive(order);
        for (Trade trade : trades) { // Loop over all resulting trades
            cout << trade; // Output resulting trade
        }
        cout << book; // Output Order Book
    }
    return 0; // Exit with success
}
