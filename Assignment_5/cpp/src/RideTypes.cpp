#include "RideTypes.h"
#include <iostream>
#include <iomanip>

// ── StandardRide ──────────────────────────────
StandardRide::StandardRide(int id,
                           const std::string& pickup,
                           const std::string& dropoff,
                           double dist)
    : Ride(id, pickup, dropoff, dist) {}

double StandardRide::fare() const {
    return BASE_FARE + (PER_MILE * distance);
}

// ── PremiumRide ───────────────────────────────
PremiumRide::PremiumRide(int id,
                         const std::string& pickup,
                         const std::string& dropoff,
                         double dist)
    : Ride(id, pickup, dropoff, dist) {}

double PremiumRide::fare() const {
    return BASE_FARE + (PER_MILE * distance) + LUXURY_CHARGE;
}

void PremiumRide::rideDetails() const {
    Ride::rideDetails();   // call base display
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "           └─ incl. luxury surcharge $"
              << LUXURY_CHARGE << "\n";
}

// ── SharedRide ────────────────────────────────
SharedRide::SharedRide(int id,
                       const std::string& pickup,
                       const std::string& dropoff,
                       double dist,
                       int pax)
    : Ride(id, pickup, dropoff, dist), passengers(pax) {}

double SharedRide::fare() const {
    double total = BASE_FARE + (PER_MILE * distance);
    return total / passengers;   // split equally
}

void SharedRide::rideDetails() const {
    Ride::rideDetails();
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "           └─ shared among " << passengers
              << " passengers ($" << fare() << " each)\n";
}
