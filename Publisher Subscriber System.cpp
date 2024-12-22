#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

// Base class for the publishers
class Publisher {  // Abstract base class that defines the interface for publisher types 
public:
    virtual void update_data(std::uint64_t instrumentId, double lastTradedPrice, double additionalData) = 0; // virtual method to update data.
    virtual void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) = 0;  // virtual method to allow a subscriber to subscribe to the required thing.
    virtual std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) = 0; // virtual method to fetch the required instrument data for a specific subscriber.
    virtual ~Publisher() = default; // destructor is called so as to ensure proper cleanup for derived classes
};

// Equity publisher implementation
class EquityPublisher : public Publisher {
private:
    struct EquityData {  // structure to hold lasttradedprice and lastdayvolume for an equity instrument
        double lastTradedPrice;
        std::uint64_t lastDayVolume;
    };

    std::unordered_map<std::uint64_t, EquityData> equityData_; // map storing equity data for each instrument 
    std::unordered_map<std::uint64_t, std::unordered_set<std::uint64_t>> subscribers_; // map of instruments to sets of subscribers who have subscribed.

public:
    void update_data(std::uint64_t instrumentId, double lastTradedPrice, double lastDayVolume) override {  // updates equity data with the latest data 
        equityData_[instrumentId] = {lastTradedPrice, static_cast<std::uint64_t>(lastDayVolume)};
    }

    void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) override {  // adds a subscriber to the list of those following an instrument.
        subscribers_[instrumentId].insert(subscriberId);
    }

    std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) override { // it checks if the subscriber is allowed to access data. If yes, it returns data as a string.
        if (subscribers_[instrumentId].find(subscriberId) == subscribers_[instrumentId].end() || equityData_.find(instrumentId) == equityData_.end()) {
            return "invalid_request"; // else invalid request 
        }
        const auto& data = equityData_[instrumentId];
        return std::to_string(instrumentId) + ", " + std::to_string(data.lastTradedPrice) + ", " + std::to_string(data.lastDayVolume);
    }
};

// Bond publisher implementation. similar to equity publisher, but it works with bonds.
class BondPublisher : public Publisher {
private:
    struct BondData { // it stores most recent traded price and yield of the bond 
        double lastTradedPrice;
        double bondYield;
    };

    std::unordered_map<std::uint64_t, BondData> bondData_;
    std::unordered_map<std::uint64_t, std::unordered_set<std::uint64_t>> subscribers_;

public:
    void update_data(std::uint64_t instrumentId, double lastTradedPrice, double bondYield) override { // it updates bond specific data(lasttradedprice and bondyield)
        bondData_[instrumentId] = {lastTradedPrice, bondYield};
    }

    void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) override {
        subscribers_[instrumentId].insert(subscriberId);
    }

    std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) override { // returns bond data in proper format 
        if (subscribers_[instrumentId].find(subscriberId) == subscribers_[instrumentId].end() || bondData_.find(instrumentId) == bondData_.end()) {
            return "invalid_request";
        }
        const auto& data = bondData_[instrumentId];
        return std::to_string(instrumentId) + ", " + std::to_string(data.lastTradedPrice) + ", " + std::to_string(data.bondYield);
    }
};

// subscriber base class
class Subscriber {  // an abstract baswe class for different subscriber types 
protected:
    std::uint64_t id_;  // stores the unique id of subscriber 

public: // constructor called 
    explicit Subscriber(std::uint64_t id) : id_(id) {} // initialsies the subscriber with a unique id 
    virtual void subscribe(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) = 0;  // virtual function for subscribingf to a publisher 
    virtual std::string get_data(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) = 0; // virtual function for retrieving data 
    virtual ~Subscriber() = default; // destructor called for safe cleanup
};

// Freesubscriber implementation
class FreeSubscriber : public Subscriber { // tracks the number of requrests made
private:
    int requestCount_ = 0;
    const int maxRequests_ = 100; // max number of rwequests allowed is 100

public: // constructor called 
    explicit FreeSubscriber(std::uint64_t id) : Subscriber(id) {} // initializes the subscriber with an ID and starts the request count at 0.

    void subscribe(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override {
        publisher->subscribe(id_, instrumentId);
    }

    std::string get_data(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override { // checks if request count exceeds max requests allowed
        if (requestCount_ >= maxRequests_) {
            return std::to_string(id_) + ", " + std::to_string(instrumentId) + ", invalid_request"; // if exceeded then invalid request 
        }
        ++requestCount_; // otherwise retrieve data 
        return publisher->get_data(id_, instrumentId);
    }
};

// Paid subscriber implementation // no limit on number of rrequests and always retrieve data if valid 
class PaidSubscriber : public Subscriber {
public:
    explicit PaidSubscriber(std::uint64_t id) : Subscriber(id) {}

    void subscribe(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override {
        publisher->subscribe(id_, instrumentId);
    }

    std::string get_data(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override {
        return publisher->get_data(id_, instrumentId);
    }
};

// Main function to demonstrate the system
int main() {
    auto equityPublisher = std::make_shared<EquityPublisher>();
    auto bondPublisher = std::make_shared<BondPublisher>(); // createas shared poinrter for equity and bond publisher 

    auto freeSubscriber = std::make_shared<FreeSubscriber>(1); // creates a free subscriber(id=1) and paid subscriber (id=2)
    auto paidSubscriber = std::make_shared<PaidSubscriber>(2);

    // example: update and subscribe
    equityPublisher->update_data(100, 123.45, 10000);
    bondPublisher->update_data(1100, 98.76, 3.5); // a sample example 

    freeSubscriber->subscribe(equityPublisher, 100);
    paidSubscriber->subscribe(bondPublisher, 1100);

    // example: Get data
    std::cout << freeSubscriber->get_data(equityPublisher, 100) << std::endl;
    std::cout << paidSubscriber->get_data(bondPublisher, 1100) << std::endl;

    // exceeding free subscriber limit
    for (int i = 0; i < 101; ++i) {
        std::cout << freeSubscriber->get_data(equityPublisher, 100) << std::endl; // prints data for both subscribers
    }

    return 0;
}

