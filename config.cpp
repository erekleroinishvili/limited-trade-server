#ifndef CONFIG /* Include this file only once to avoid duplicate code */
#define CONFIG
#include <iostream>

using namespace std;

/** Output Place-Order commands as they are received */
const bool SHOW_ORDERS = false;

/* Separator (delimiter) for grouping thousands, e.g.: 1,000,000 */
const char THOUSAND_SEPARATOR = ',';

enum TradeType {
    BUY = 'B',
    SELL = 'S',
};

#endif
