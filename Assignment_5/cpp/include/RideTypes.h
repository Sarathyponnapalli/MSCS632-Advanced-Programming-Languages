#pragma once
#include "Ride.h"

// ─────────────────────────────────────────────
//  StandardRide – base rate per mile
//  Demonstrates: Inheritance, Polymorphism
// ─────────────────────────────────────────────
class StandardRide : public Ride {
    static constexpr double BASE_FARE   = 2.50;
    static constexpr double PER_MILE    = 1.20;

public:
    StandardRide(int id,
                 const std::string& pickup,
                 const std::string& dropoff,
                 double dist);

    double      fare()     const override;
    std::string rideType() const override { return "Standard"; }
};

// ─────────────────────────────────────────────
//  PremiumRide – higher rate + luxury surcharge
//  Demonstrates: Inheritance, Polymorphism
// ─────────────────────────────────────────────
class PremiumRide : public Ride {
    static constexpr double BASE_FARE      = 5.00;
    static constexpr double PER_MILE       = 2.80;
    static constexpr double LUXURY_CHARGE  = 4.00;

public:
    PremiumRide(int id,
                const std::string& pickup,
                const std::string& dropoff,
                double dist);

    double      fare()     const override;
    std::string rideType() const override { return "Premium"; }
    void        rideDetails() const override;   // extends base output
};

// ─────────────────────────────────────────────
//  SharedRide – split fare among passengers
//  Demonstrates: Inheritance, Polymorphism
// ─────────────────────────────────────────────
class SharedRide : public Ride {
    int    passengers;
    static constexpr double BASE_FARE = 2.00;
    static constexpr double PER_MILE  = 1.00;

public:
    SharedRide(int id,
               const std::string& pickup,
               const std::string& dropoff,
               double dist,
               int passengers);

    double      fare()     const override;
    std::string rideType() const override { return "Shared"; }
    void        rideDetails() const override;
    int         getPassengers() const { return passengers; }
};
