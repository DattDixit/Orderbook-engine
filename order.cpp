#include <chrono>
#include <cstdint>
#include <map>
#include <vector>
#include <memory>
#include<iostream>

using namespace std;

enum class OrderType { GTC, FOK, FAK, MARKET };
enum class OrderSide { BUY, SELL };

struct Order {
    uint64_t orderId;
    OrderSide side;
    double price;           // Use fixed-point in production for precision
    uint32_t quantity;
    OrderType type;
    chrono::nanoseconds timestamp;

    Order(uint64_t id, OrderSide s, double p, uint32_t qty, OrderType t)
        : orderId(id), side(s), price(p), quantity(qty), type(t),
          timestamp(chrono::high_resolution_clock::now().time_since_epoch()) {}
};



class Orderbook {
private:
    multimap<double,shared_ptr<Order>,greater<double>> bids; // Highest price first
    multimap<double, shared_ptr<Order>> asks;                      // Lowest price first
    uint64_t nextOrderId = 1;

public:
    uint64_t addOrder(OrderSide side, double price, uint32_t quantity, OrderType type);
    bool cancelOrder(uint64_t orderId);
    bool modifyOrder(uint64_t orderId, double newPrice, uint32_t newQuantity);
    void matchOrders();
};

uint64_t Orderbook::addOrder(OrderSide side, double price, uint32_t quantity, OrderType type) {
    auto order = make_shared<Order>(nextOrderId++, side, price, quantity, type);
    
    if (type == OrderType::MARKET) {
        // Market orders execute immediately
        matchOrders();
        return order->orderId;
    }

    if (side == OrderSide::BUY) {
        bids.emplace(price, order);
    } else {
        asks.emplace(price, order);
    }

    matchOrders(); // Attempt to match after adding
    return order->orderId;
}

void Orderbook::matchOrders() {
    while (!bids.empty() && !asks.empty()) {
        auto bestBid = bids.begin();
        auto bestAsk = asks.begin();

        if (bestBid->first < bestAsk->first) {
            break; // No match possible
        }

        // Trade execution
        uint32_t tradeQty = std::min(bestBid->second->quantity, bestAsk->second->quantity);
            cout << "Trade: " << tradeQty << " units at " << bestAsk->first 
                  << " (Bid ID: " << bestBid->second->orderId 
                  << ", Ask ID: " << bestAsk->second->orderId << ")\n";

        // Update quantities
        bestBid->second->quantity -= tradeQty;
        bestAsk->second->quantity -= tradeQty;

        // Remove fully filled orders
        if (bestBid->second->quantity == 0) bids.erase(bids.begin());
        if (bestAsk->second->quantity == 0) asks.erase(asks.begin());
    }

    // Handle FOK/FAK logic post-matching
    // (For simplicity, assume GTC here; extend for other types)
}

bool Orderbook::cancelOrder(uint64_t orderId) {
    for (auto it = bids.begin(); it != bids.end(); ++it) {
        if (it->second->orderId == orderId) {
            bids.erase(it);
            return true;
        }
    }
    for (auto it = asks.begin(); it != asks.end(); ++it) {
        if (it->second->orderId == orderId) {
            asks.erase(it);
            return true;
        }
    }
    return false;
}

bool Orderbook::modifyOrder(uint64_t orderId, double newPrice, uint32_t newQuantity) {
    for (auto it = bids.begin(); it != bids.end(); ++it) {
        if (it->second->orderId == orderId) {
            auto order = it->second;
            bids.erase(it);
            order->price = newPrice;
            order->quantity = newQuantity;
            bids.emplace(newPrice, order);
            matchOrders();
            return true;
        }
    }
    // Similar logic for asks (omitted for brevity)
    return false;
}


int main() {
    Orderbook ob;
    ob.addOrder(OrderSide::BUY, 100.0, 10, OrderType::GTC);
    ob.addOrder(OrderSide::SELL, 99.5, 5, OrderType::GTC);
    ob.addOrder(OrderSide::BUY, 99.5, 5, OrderType::GTC); // Should match with sell
    return 0;
}
