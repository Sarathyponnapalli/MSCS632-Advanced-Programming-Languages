#pragma once
#include "Ride.h"
#include <string>
#include <vector>
#include <memory>

// ─────────────────────────────────────────────
//  Driver – manages its own ride list (encapsulation)
// ─────────────────────────────────────────────
class Driver {
    int         driverID;
    std::string name;
    double      rating;
    // Private: accessible only through addRide / accessors
    std::vector<std::shared_ptr<Ride>> assignedRides;

public:
    Driver(int id, const std::string& name, double rating);

    void   addRide(std::shared_ptr<Ride> ride);
    void   getDriverInfo() const;
    double totalEarnings() const;

    // Accessors
    int         getDriverID() const { return driverID; }
    std::string getName()     const { return name; }
    double      getRating()   const { return rating; }
    int         rideCount()   const { return static_cast<int>(assignedRides.size()); }

    const std::vector<std::shared_ptr<Ride>>& getRides() const { return assignedRides; }
};
