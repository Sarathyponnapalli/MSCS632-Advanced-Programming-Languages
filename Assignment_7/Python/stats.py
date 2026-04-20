# stats.py  -  RideShare Statistics Calculator (Object-Oriented)
# Course   : MSCS-632 Advanced Programming Languages
# Author   : Parthasarathi Ponnapalli
# Paradigm : Object-Oriented (Python)
#
# Analyses completed rides from the RideShare system.
# Calculates mean fare, median distance, mode ride type,
# and a frequency table of ride types — all encapsulated
# inside a RideShareStats class, demonstrating OOP principles.

from dataclasses import dataclass


# ── Ride data class ───────────────────────────────────────────────────────────
@dataclass
class Ride:
    """
    Represents one completed trip in the RideShare system.
    Matches the C++ Ride class from Assignment 5.
    """
    ride_id: int
    ride_type: str      # 'Standard', 'Premium', or 'Shared'
    distance: float    # miles
    fare: float    # dollars


# ── Statistics calculator class ───────────────────────────────────────────────
class RideShareStats:
    """
    Calculates descriptive statistics over a collection of completed rides.

    Encapsulates:
        _rides  - private list of Ride objects (not directly accessible)

    Methods cover the four business questions a ride-sharing company
    would regularly ask about its trip data.
    """

    def __init__(self, rides: list[Ride]):
        if not rides:
            raise ValueError("Rides list must not be empty.")
        self._rides = rides[:]          # private defensive copy

    # ── Mean fare ─────────────────────────────────────────────────────────────
    def mean_fare(self) -> float:
        """
        Average fare charged per trip.
        Business use: benchmark pricing strategy, detect under/overpricing.
        """
        return sum(r.fare for r in self._rides) / len(self._rides)

    # ── Median distance ───────────────────────────────────────────────────────
    def median_distance(self) -> float:
        """
        Median trip distance in miles.
        Business use: understand the typical trip length without being
        skewed by occasional long-distance outliers.
        """
        sorted_d = sorted(r.distance for r in self._rides)
        n = len(sorted_d)
        mid = n // 2
        if n % 2 == 1:
            return sorted_d[mid]
        return (sorted_d[mid - 1] + sorted_d[mid]) / 2.0

    # ── Frequency table ───────────────────────────────────────────────────────
    def frequency_table(self) -> dict[str, int]:
        """
        Additional functionality:
        Count how many rides of each type were completed.
        Business use: understand demand mix — if Standard dominates,
        the company may not need to market Premium rides as aggressively.
        Returns a dict sorted by count descending.
        """
        counts: dict[str, int] = {}
        for ride in self._rides:
            counts[ride.ride_type] = counts.get(ride.ride_type, 0) + 1
        return dict(sorted(counts.items(), key=lambda x: -x[1]))

    # ── Mode ride type ────────────────────────────────────────────────────────
    def mode_ride_type(self) -> str:
        """
        Most commonly booked ride type.
        Business use: drives fleet allocation and driver incentive programs.
        Derived from frequency_table — no duplicate counting logic.
        """
        freq = self.frequency_table()
        return max(freq, key=freq.get)

    # ── Summary ───────────────────────────────────────────────────────────────
    def print_summary(self) -> None:
        """Print all four statistics with business context labels."""
        freq = self.frequency_table()
        total = len(self._rides)

        print("\n=== RideShare Statistics ===")
        print(f"  Mean Fare       : ${self.mean_fare():.2f}"
              f"  (avg charge per trip)")
        print(f"  Median Distance : {self.median_distance():.1f} mi"
              f"  (typical trip length)")
        print(f"  Mode Ride Type  : {self.mode_ride_type()}"
              f"  (most booked type)")

        print("\nRide Type Frequency Table:")
        print(f"  {'Type':<10}  {'Count':>5}  {'Share':>7}")
        print(f"  {'-'*10}  {'-'*5}  {'-'*7}")
        for ride_type, count in freq.items():
            pct = 100.0 * count / total
            print(f"  {ride_type:<10}  {count:>5}  {pct:>6.1f}%")

    def print_rides(self) -> None:
        """Print the full ride list."""
        print(f"  {'#':<4}  {'Type':<10}  {'Distance':<10}  {'Fare':<10}")
        print(
            f"  {'----':<4}  {'----------':<10}  {'----------':<10}  {'----------':<10}")
        for r in self._rides:
            print(f"  {r.ride_id:<4}  {r.ride_type:<10}  "
                  f"{r.distance:>7.1f} mi  ${r.fare:>8.2f}")


# ── Main ──────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    print("==============================================")
    print("  RideShare Statistics  --  Python (OOP)")
    print("  MSCS-632  Advanced Programming Languages")
    print("==============================================\n")

    # Sample rides matching the RideShare system from Assignment 5.
    # Fares calculated using the same formulas:
    #   Standard : $2.50 + $1.20/mi
    #   Premium  : $5.00 + $2.80/mi + $4.00 luxury
    #   Shared   : ($2.00 + $1.00/mi) / passengers
    rides = [
        Ride(1,  "Standard",  4.2,  7.54),
        Ride(2,  "Premium",   2.8, 16.84),
        Ride(3,  "Shared",   11.5,  3.38),
        Ride(4,  "Standard",  6.0,  9.70),
        Ride(5,  "Premium",   9.3, 35.04),
        Ride(6,  "Standard",  8.5, 12.70),
        Ride(7,  "Premium",  12.0, 42.60),
        Ride(8,  "Standard",  2.1,  5.02),
        Ride(9,  "Standard",  5.4,  8.94),
        Ride(10, "Premium",   3.8, 19.64),
    ]

    stats = RideShareStats(rides)

    print("Completed Rides:")
    stats.print_rides()
    stats.print_summary()

    print("\n==============================================")
    print("  Done.")
    print("==============================================")
