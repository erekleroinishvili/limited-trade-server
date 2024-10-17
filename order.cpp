#include <iostream>
#include <string>
#include <regex>
#include <optional>
#include <assert.h>
#include <algorithm>
#include "all-includes.h"

using namespace std;

/**
 * Virtual Clock.
 * Used by Order to register order creation and refresh time.
 * Singleton.
 */
class {
    private:
        unsigned int tick = 0;
    public:
        unsigned int next() {
            return ++this->tick;
        }
} Clock;

class Order {
private:
    unsigned int hiddenVolume = 0;
    unsigned int maxPeakSize = 0;
    void refresh() {
        if (this->volume == 0 && this->hiddenVolume > 0) {
            this->volume = min({this->hiddenVolume, this->maxPeakSize});
            this->hiddenVolume -= this->volume;
            this->refreshed = Clock.next();
        }
    }
    bool isIceberg() const {
        return this->maxPeakSize > 0;
    }
public:
    TradeType type;
    unsigned int id;
    unsigned short price; // in pence
    unsigned int volume; // Trade volume with which to participate in trades
    unsigned int refreshed = Clock.next();

    Order() {}

    Order(TradeType type, unsigned int id, unsigned short price, unsigned int volume)
        : Order(type, id, price, volume, 0) {}

    Order(TradeType type, unsigned int id, unsigned short price, unsigned int volume, unsigned int peak)
        : type(type), id(id), price(price), volume(volume), maxPeakSize(peak)
    {
        assert(type == BUY || type == SELL);
    }

    /**
     * @brief Order is fully filled with its full capacity - visible and hidden
     */
    bool fulfilled() const {
        return !this->volume;
    }

    bool canTradeAt(unsigned short price) const {
        return false ||
            this->type == BUY && this->price >= price ||
            this->type == SELL && this->price <= price;
    }

    void trade(unsigned int tradeVolume) {
        assert(tradeVolume <= this->volume);
        this->volume -= tradeVolume;
        if (this->isIceberg() && this->volume == 0) {
            this->refresh();
        }
    }

    void becomePassive() {
        if (this->isIceberg() && this->volume > this->maxPeakSize) {
            this->hiddenVolume = this->volume - this->maxPeakSize;
            this->volume = this->maxPeakSize;
        }
    }

    /**
     * @brief Compare orders for priority
     * `*this` order has price/time priority over `order`
     * @param order the other order being compared with `*this`
     * @return true if `*this` has priority
     * @return false otherwise
     */
    bool operator <(const Order & order) const {
        assert(this->type == order.type);
        if (this->price != order.price) return (this->price < order.price) ^ (this->type == BUY);
        if (this->refreshed != order.refreshed) return this->refreshed < order.refreshed;
        return false; // Should only happen when `*this` and `order` are the same object
    }
    bool operator >(const Order & order) const {
        return order < * this;
    }

    friend std::istream & operator >>(std::istream & inputStream, Order & order);
    friend std::ostream & operator <<(std::ostream & outputStream, const Order & order);

};


