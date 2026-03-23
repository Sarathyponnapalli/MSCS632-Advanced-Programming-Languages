#pragma once
#include "Driver.h"
#include "Rider.h"
#include "Ride.h"
#include <vector>
#include <memory>
#include <map>

// ─────────────────────────────────────────────
//  RideSystem – top-level orchestrator
// ─────────────────────────────────────────────
class RideSystem {
    std::map<int, std::shared_ptr<Driver>> drivers;
    std::map<int, std::shared_ptr<Rider>>  riders;
    // Polymorphic ride pool – base-class pointers to any Ride subtype
    std::vector<std::shared_ptr<Ride>>     allRides;
    int nextRideID = 1000;

public:
    void addDriver(std::shared_ptr<Driver> d);
    void addRider (std::shared_ptr<Rider>  r);

    // Returns the newly created ride
    std::shared_ptr<Ride> bookStandardRide(int riderID, int driverID,
                                           const std::string& pickup,
                                           const std::string& dropoff,
                                           double dist);

    std::shared_ptr<Ride> bookPremiumRide (int riderID, int driverID,
                                           const std::string& pickup,
                                           const std::string& dropoff,
                                           double dist);

    std::shared_ptr<Ride> bookSharedRide  (int riderID, int driverID,
                                           const std::string& pickup,
                                           const std::string& dropoff,
                                           double dist,
                                           int passengers);

    // Polymorphic dispatch over allRides
    void printAllRides()   const;
    void printSystemSummary() const;

    std::shared_ptr<Driver> getDriver(int id);
    std::shared_ptr<Rider>  getRider (int id);

    const std::vector<std::shared_ptr<Ride>>& getAllRides() const { return allRides; }
};
