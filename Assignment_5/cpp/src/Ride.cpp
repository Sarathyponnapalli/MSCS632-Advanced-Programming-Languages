#include "Ride.h"
#include <iostream>
#include <iomanip>

Ride::Ride(int id,
           const std::string& pickup,
           const std::string& dropoff,
           double dist)
    : rideID(id), pickupLocation(pickup), dropoffLocation(dropoff), distance(dist) {}

void Ride::rideDetails() const {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  [" << rideType() << " #" << rideID << "]"
              << "  " << pickupLocation << " → " << dropoffLocation
              << "  |  " << distance << " mi"
              << "  |  $" << fare() << "\n";
}
