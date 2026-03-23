#include "Rider.h"
#include <iostream>
#include <iomanip>

Rider::Rider(int id, const std::string& n)
    : riderID(id), name(n) {}

void Rider::requestRide(std::shared_ptr<Ride> ride) {
    requestedRides.push_back(ride);
}

double Rider::totalSpent() const {
    double sum = 0.0;
    for (const auto& r : requestedRides) sum += r->fare();
    return sum;
}

void Rider::viewRides() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n╔══ Rider #" << riderID << " ─ " << name << "\n"
              << "║   Rides   : " << requestedRides.size() << "\n"
              << "║   Spent   : $" << totalSpent() << "\n"
              << "╚══ Ride history:\n";
    for (const auto& r : requestedRides) r->rideDetails();
}
