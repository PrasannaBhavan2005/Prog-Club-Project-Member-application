#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <memory>
#include <string>
#include <cstdint>

// Base class for Publishers
class Publisher {
public:
    virtual void update_data(std::uint64_t instrumentId, double lastTradedPrice, double additionalData) = 0;
    virtual void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) = 0;
    virtual std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) = 0;
    virtual ~Publisher() = default;
};

// EquityPublisher implementation
class EquityPublisher : public Publisher {
private:
    struct EquityData {
        double lastTradedPrice;
        std::uint64_t lastDayVolume;
    };

    std::unordered_map<std::uint64_t, EquityData> equityData_;
    std::unordered_map<std::uint64_t, std::unordered_set<std::uint64_t>> subscribers_;

public:
    void update_data(std::uint64_t instrumentId, double lastTradedPrice, double lastDayVolume) override {
        equityData_[instrumentId] = {lastTradedPrice, static_cast<std::uint64_t>(lastDayVolume)};
    }

    void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) override {
        subscribers_[instrumentId].insert(subscriberId);
    }

    std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) override {
        if (subscribers_[instrumentId].find(subscriberId) == subscribers_[instrumentId].end() || equityData_.find(instrumentId) == equityData_.end()) {
            return "invalid_request";
        }
        const auto& data = equityData_[instrumentId];
        return std::to_string(instrumentId) + ", " + std::to_string(data.lastTradedPrice) + ", " + std::to_string(data.lastDayVolume);
    }
};

// BondPublisher implementation
class BondPublisher : public Publisher {
private:
    struct BondData {
        double lastTradedPrice;
        double bondYield;
    };

    std::unordered_map<std::uint64_t, BondData> bondData_;
    std::unordered_map<std::uint64_t, std::unordered_set<std::uint64_t>> subscribers_;

public:
    void update_data(std::uint64_t instrumentId, double lastTradedPrice, double bondYield) override {
        bondData_[instrumentId] = {lastTradedPrice, bondYield};
    }

    void subscribe(std::uint64_t subscriberId, std::uint64_t instrumentId) override {
        subscribers_[instrumentId].insert(subscriberId);
    }

    std::string get_data(std::uint64_t subscriberId, std::uint64_t instrumentId) override {
        if (subscribers_[instrumentId].find(subscriberId) == subscribers_[instrumentId].end() || bondData_.find(instrumentId) == bondData_.end()) {
            return "invalid_request";
        }
        const auto& data = bondData_[instrumentId];
        return std::to_string(instrumentId) + ", " + std::to_string(data.lastTradedPrice) + ", " + std::to_string(data.bondYield);
    }
};

// Subscriber base class
class Subscriber {
protected:
    std::uint64_t id_;

public:
    explicit Subscriber(std::uint64_t id) : id_(id) {}
    virtual void subscribe(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) = 0;
    virtual std::string get_data(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) = 0;
    virtual ~Subscriber() = default;
};

// FreeSubscriber implementation
class FreeSubscriber : public Subscriber {
private:
    int requestCount_ = 0;
    const int maxRequests_ = 100;

public:
    explicit FreeSubscriber(std::uint64_t id) : Subscriber(id) {}

    void subscribe(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override {
        publisher->subscribe(id_, instrumentId);
    }

    std::string get_data(std::shared_ptr<Publisher> publisher, std::uint64_t instrumentId) override {
        if (requestCount_ >= maxRequests_) {
            return std::to_string(id_) + ", " + std::to_string(instrumentId) + ", invalid_request";
        }
        ++requestCount_;
        return publisher->get_data(id_, instrumentId);
    }
};

// PaidSubscriber implementation
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
    auto bondPublisher = std::make_shared<BondPublisher>();

    auto freeSubscriber = std::make_shared<FreeSubscriber>(1);
    auto paidSubscriber = std::make_shared<PaidSubscriber>(2);

    // Example: Update and subscribe
    equityPublisher->update_data(100, 123.45, 10000);
    bondPublisher->update_data(1100, 98.76, 3.5);

    freeSubscriber->subscribe(equityPublisher, 100);
    paidSubscriber->subscribe(bondPublisher, 1100);

    // Example: Get data
    std::cout << freeSubscriber->get_data(equityPublisher, 100) << std::endl;
    std::cout << paidSubscriber->get_data(bondPublisher, 1100) << std::endl;

    // Exceeding free subscriber limit
    for (int i = 0; i < 101; ++i) {
        std::cout << freeSubscriber->get_data(equityPublisher, 100) << std::endl;
    }

    return 0;
}

