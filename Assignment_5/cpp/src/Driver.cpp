#include "Driver.h"
#include <iostream>
#include <iomanip>
#include <numeric>

Driver::Driver(int id, const std::string& n, double r)
    : driverID(id), name(n), rating(r) {}

void Driver::addRide(std::shared_ptr<Ride> ride) {
    assignedRides.push_back(ride);
}

double Driver::totalEarnings() const {
    double sum = 0.0;
    for (const auto& r : assignedRides) sum += r->fare();
    return sum;
}

void Driver::getDriverInfo() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n╔══ Driver #" << driverID << " ─ " << name << "\n"
              << "║   Rating : " << rating << " ★\n"
              << "║   Rides  : " << assignedRides.size() << "\n"
              << "║   Earned : $" << totalEarnings() << "\n"
              << "╚══ Ride history:\n";
    for (const auto& r : assignedRides) r->rideDetails();
}
