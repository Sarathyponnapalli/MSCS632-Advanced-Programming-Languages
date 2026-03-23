#pragma once
#include <string>

// ─────────────────────────────────────────────
//  Abstract Base Class: Ride
//  Demonstrates: Encapsulation, Abstraction
// ─────────────────────────────────────────────
class Ride {
protected:
    int         rideID;
    std::string pickupLocation;
    std::string dropoffLocation;
    double      distance;   // miles

public:
    Ride(int id,
         const std::string& pickup,
         const std::string& dropoff,
         double dist);

    virtual ~Ride() = default;

    // Pure virtual – forces every subclass to define its own pricing
    virtual double      fare()        const = 0;
    virtual std::string rideType()    const = 0;
    virtual void        rideDetails() const;

    // Accessors (encapsulation)
    int         getRideID()          const { return rideID; }
    std::string getPickup()          const { return pickupLocation; }
    std::string getDropoff()         const { return dropoffLocation; }
    double      getDistance()        const { return distance; }
};
