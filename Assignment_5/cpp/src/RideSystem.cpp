#include "RideSystem.h"
#include "RideTypes.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

void RideSystem::addDriver(std::shared_ptr<Driver> d) {
    drivers[d->getDriverID()] = d;
}

void RideSystem::addRider(std::shared_ptr<Rider> r) {
    riders[r->getRiderID()] = r;
}

// ── helpers ──────────────────────────────────────────────────────────────────
static void wire(std::shared_ptr<Ride>   ride,
                 std::shared_ptr<Driver> driver,
                 std::shared_ptr<Rider>  rider)
{
    driver->addRide(ride);
    rider->requestRide(ride);
}

// ── booking methods ───────────────────────────────────────────────────────────
std::shared_ptr<Ride> RideSystem::bookStandardRide(
        int riderID, int driverID,
        const std::string& pickup, const std::string& dropoff, double dist)
{
    auto ride   = std::make_shared<StandardRide>(nextRideID++, pickup, dropoff, dist);
    auto driver = drivers.at(driverID);
    auto rider  = riders .at(riderID);
    allRides.push_back(ride);
    wire(ride, driver, rider);
    return ride;
}

std::shared_ptr<Ride> RideSystem::bookPremiumRide(
        int riderID, int driverID,
        const std::string& pickup, const std::string& dropoff, double dist)
{
    auto ride   = std::make_shared<PremiumRide>(nextRideID++, pickup, dropoff, dist);
    auto driver = drivers.at(driverID);
    auto rider  = riders .at(riderID);
    allRides.push_back(ride);
    wire(ride, driver, rider);
    return ride;
}

std::shared_ptr<Ride> RideSystem::bookSharedRide(
        int riderID, int driverID,
        const std::string& pickup, const std::string& dropoff,
        double dist, int passengers)
{
    auto ride   = std::make_shared<SharedRide>(nextRideID++, pickup, dropoff, dist, passengers);
    auto driver = drivers.at(driverID);
    auto rider  = riders .at(riderID);
    allRides.push_back(ride);
    wire(ride, driver, rider);
    return ride;
}

// ── polymorphic display ───────────────────────────────────────────────────────
void RideSystem::printAllRides() const {
    std::cout << "\n══════════════════════════════════════════════\n"
              << "  ALL RIDES  (polymorphic dispatch)\n"
              << "══════════════════════════════════════════════\n";
    for (const auto& ride : allRides) {
        ride->rideDetails();   // virtual call – correct subtype runs
    }
}

void RideSystem::printSystemSummary() const {
    std::cout << std::fixed << std::setprecision(2);
    double total = 0.0;
    for (const auto& r : allRides) total += r->fare();
    std::cout << "\n══════════════════════════════════════════════\n"
              << "  SYSTEM SUMMARY\n"
              << "══════════════════════════════════════════════\n"
              << "  Total rides   : " << allRides.size()  << "\n"
              << "  Total drivers : " << drivers.size()   << "\n"
              << "  Total riders  : " << riders.size()    << "\n"
              << "  Total revenue : $" << total           << "\n"
              << "══════════════════════════════════════════════\n";
}

std::shared_ptr<Driver> RideSystem::getDriver(int id) { return drivers.at(id); }
std::shared_ptr<Rider>  RideSystem::getRider (int id) { return riders .at(id); }
