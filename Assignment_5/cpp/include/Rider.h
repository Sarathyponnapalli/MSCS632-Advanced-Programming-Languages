#pragma once
#include "Ride.h"
#include <string>
#include <vector>
#include <memory>

// ─────────────────────────────────────────────
//  Rider – tracks requested rides
// ─────────────────────────────────────────────
class Rider {
    int         riderID;
    std::string name;
    // Private list – encapsulated behind requestRide / viewRides
    std::vector<std::shared_ptr<Ride>> requestedRides;

public:
    Rider(int id, const std::string& name);

    void requestRide(std::shared_ptr<Ride> ride);
    void viewRides()          const;
    double totalSpent()       const;

    // Accessors
    int         getRiderID()  const { return riderID; }
    std::string getName()     const { return name; }
    int         rideCount()   const { return static_cast<int>(requestedRides.size()); }

    const std::vector<std::shared_ptr<Ride>>& getRides() const { return requestedRides; }
};
