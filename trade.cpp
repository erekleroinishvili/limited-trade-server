#include <iostream>
#include <vector>
#include <assert.h>

using namespace std;

class Trade {
private:
    unsigned int buyOrderId;
    unsigned int sellOrderId;
    unsigned short price;
    unsigned int volume;

public:
    Trade(unsigned int buyOrderId, unsigned int sellOrderId, unsigned short price, unsigned int volume)
        : buyOrderId(buyOrderId), sellOrderId(sellOrderId), price(price), volume(volume) {}

    bool match(const Trade & trade) {
        return this->buyOrderId == trade.buyOrderId && this->sellOrderId == trade.sellOrderId;
    }
    void addVolumeFrom(const Trade & trade) {
        assert(match(trade) && this->price == trade.price);
        this->volume += trade.volume;
    }

    /**
     * @brief Allows the use of `<<` operator with Output Streams
     */
    friend std::ostream &operator <<(std::ostream & outputStream, const Trade & trade) {
        return outputStream << trade.buyOrderId << ',' << trade.sellOrderId << ',' << trade.price << ',' << trade.volume << endl;
    }
};

class TradeList {
private:
    vector<Trade> trades;

public:
    void addTrade(const Trade & trade) {
        bool existingTrade = false;
        for (Trade & t : this->trades) {
            if (t.match(trade)) {
                existingTrade = true;
                t.addVolumeFrom(trade);
            }
        }
        if (!existingTrade) {
            trades.emplace_back(trade);
        }
    }
    vector<Trade> getTrades() {
        return this->trades;
    }
};
