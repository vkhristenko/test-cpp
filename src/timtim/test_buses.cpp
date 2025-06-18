#include <cassert>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    string operation_code;
    is >> operation_code;
    if (operation_code == "NEW_BUS") {
        q.type = QueryType::NewBus;
        is >> q.bus;
        int stop_count;
        is >> stop_count;
        q.stops.clear();
        for (int i = 0; i < stop_count; i++) {
            is >> q.stop;
            q.stops.push_back(q.stop);
        }
    } else if (operation_code == "BUSES_FOR_STOP") {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    } else if (operation_code == "STOPS_FOR_BUS") {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    } else if (operation_code == "ALL_BUSES") {
        q.type = QueryType::AllBuses;
    }
    return is;
}

struct BusesForStopResponse {
    vector<string> buses_for_stop;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.buses_for_stop.size() == 0) {
        os << "No buses";
        return os;
    }

    bool is_first = true;
    for (const string& bus : r.buses_for_stop) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << bus;
    }
    os << endl;
    return os;
}

struct StopsForBusResponse {
    vector<string> stops_for_bus;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.stops_for_bus.size() == 0) {
        os << "No stops" << endl;
        return os;
    }

    bool is_first = true;
    for (auto const& s : r.stops_for_bus) {
        if (!is_first) {
            os << " ";
        }
        is_first = false;
        os << s;
    }
    os << endl;
    return os;
}

struct AllBusesResponse {
    map<string, vector<string>> stops_for_bus;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.stops_for_bus.empty()) {
        cout << "No buses" << endl;
        return os;
    }
    for (const auto& bus_item : r.stops_for_bus) {
        cout << "Bus " << bus_item.first << ":";
        for (const string& stop : bus_item.second) {
            os << " " << stop;
        }
        os << endl;
    }
    return os;
}

class BusManager {
public:
    void AddBus(const string& bus, const vector<string>& stops) {
        cout << "stops: ";
        for (auto const& s : stops) {
            cout << s << "  ";
        }
        cout << endl;

        for (const auto& stop : stops) {
            buses_for_stop_[stop].push_back(bus);
            stops_for_bus_[bus].push_back(stop);
        }

        for (auto const& [stop, buses] : buses_for_stop_) {
            cout << stop << endl;
            for (auto const& b : buses) {
                cout << "  " << b << endl;
            }
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse r;

        if (buses_for_stop_.count(stop) == 0) {
            return r;
        }

        for (const auto& bus : buses_for_stop_.at(stop)) {
            r.buses_for_stop.push_back(bus);
        }

        return r;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse r;

        if (stops_for_bus_.count(bus) == 0) {
            return r;
        }

        r.stops_for_bus = stops_for_bus_.at(bus);

        return r;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse r;
        for (const auto& [bus, stops] : stops_for_bus_) {
            r.stops_for_bus.insert({bus, stops});
        }
        return r;
    }

private:
    map<string, vector<string>> buses_for_stop_;
    map<string, vector<string>> stops_for_bus_;
};

// Реализуйте функции и классы, объявленные выше, чтобы эта функция main
// решала задачу "Автобусные остановки"

int main() {
    int query_count;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        Query q;
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}
