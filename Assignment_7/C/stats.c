/*
 * stats.c  -  RideShare Statistics Calculator (Procedural)
 * Course   : MSCS-632 Advanced Programming Languages
 * Author   : Parthasarathi Ponnapalli
 * Paradigm : Procedural (C)
 *
 * Analyses a set of completed rides from the RideShare system.
 * Calculates mean fare, median distance, mode ride type,
 * and a frequency table of ride types — all statistics a
 * real ride-sharing company would run on their trip data.
 *
 * Data structures: parallel arrays for rides (no structs kept
 * opaque to demonstrate procedural memory management).
 * All dynamic arrays are heap-allocated and manually freed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ── Ride type constants ─────────────────────────────────────────────────── */
#define STANDARD 0
#define PREMIUM 1
#define SHARED 2

static const char *TYPE_NAMES[] = {"Standard", "Premium", "Shared"};

/* ── RideShare dataset ───────────────────────────────────────────────────── */
/*
 * Each ride is stored as parallel arrays:
 *   fares[]      - fare charged in dollars
 *   distances[]  - trip distance in miles
 *   types[]      - ride type (STANDARD / PREMIUM / SHARED)
 *   n            - number of rides
 *
 * This mirrors the C++ RideShare backend from Assignment 5.
 */

/* ── Comparator for qsort ────────────────────────────────────────────────── */
int cmp_double(const void *a, const void *b)
{
    double da = *(double *)a;
    double db = *(double *)b;
    return (da > db) - (da < db);
}

/* ── Mean fare ───────────────────────────────────────────────────────────── */
/*
 * Business meaning: the average amount riders are charged per trip.
 * Helps the company assess pricing strategy.
 */
double mean_fare(double *fares, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += fares[i];
    return sum / n;
}

/* ── Median distance ─────────────────────────────────────────────────────── */
/*
 * Business meaning: the typical trip length — less skewed than mean
 * because a few very long rides would distort the average.
 * Useful for driver planning and surge pricing zones.
 */
double median_distance(double *distances, int n)
{
    double *sorted = (double *)malloc(n * sizeof(double));
    memcpy(sorted, distances, n * sizeof(double));
    qsort(sorted, n, sizeof(double), cmp_double);
    double med = (n % 2 == 0)
                     ? (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0
                     : sorted[n / 2];
    free(sorted);
    return med;
}

/* ── Frequency table of ride types ──────────────────────────────────────── */
/*
 * Additional functionality:
 * Business meaning: shows how many rides of each type were completed.
 * Helps the company understand demand mix — if Shared rides dominate,
 * they might invest in more multi-passenger vehicles.
 *
 * Implementation: three counters (one per type), no heap allocation needed
 * since the number of categories is fixed and known at compile time.
 */
void frequency_table(int *types, int n,
                     int *out_std, int *out_prm, int *out_shd)
{
    *out_std = *out_prm = *out_shd = 0;
    for (int i = 0; i < n; i++)
    {
        if (types[i] == STANDARD)
            (*out_std)++;
        else if (types[i] == PREMIUM)
            (*out_prm)++;
        else if (types[i] == SHARED)
            (*out_shd)++;
    }
}

/* ── Mode ride type ──────────────────────────────────────────────────────── */
/*
 * Business meaning: the most commonly booked ride type.
 * Derived directly from the frequency table — no separate pass needed.
 */
const char *mode_ride_type(int std_cnt, int prm_cnt, int shd_cnt)
{
    int max = std_cnt;
    const char *mode = TYPE_NAMES[STANDARD];
    if (prm_cnt > max)
    {
        max = prm_cnt;
        mode = TYPE_NAMES[PREMIUM];
    }
    if (shd_cnt > max)
    {
        mode = TYPE_NAMES[SHARED];
    }
    return mode;
}

/* ── Print helpers ───────────────────────────────────────────────────────── */
void print_rides(double *fares, double *distances, int *types, int n)
{
    printf("  %-4s  %-10s  %-10s  %-10s\n",
           "#", "Type", "Distance", "Fare");
    printf("  %-4s  %-10s  %-10s  %-10s\n",
           "----", "----------", "----------", "----------");
    for (int i = 0; i < n; i++)
        printf("  %-4d  %-10s  %7.1f mi  $%8.2f\n",
               i + 1, TYPE_NAMES[types[i]], distances[i], fares[i]);
}

void print_frequency_table(int std_cnt, int prm_cnt, int shd_cnt, int n)
{
    printf("\nRide Type Frequency Table:\n");
    printf("  %-10s  %5s  %7s\n", "Type", "Count", "Share");
    printf("  %-10s  %5s  %7s\n", "----------", "-----", "-------");
    printf("  %-10s  %5d  %6.1f%%\n",
           "Standard", std_cnt, 100.0 * std_cnt / n);
    printf("  %-10s  %5d  %6.1f%%\n",
           "Premium", prm_cnt, 100.0 * prm_cnt / n);
    printf("  %-10s  %5d  %6.1f%%\n",
           "Shared", shd_cnt, 100.0 * shd_cnt / n);
}

/* ── Main ────────────────────────────────────────────────────────────────── */
int main(void)
{
    printf("==============================================\n");
    printf("  RideShare Statistics  --  C (Procedural)\n");
    printf("  MSCS-632  Advanced Programming Languages\n");
    printf("==============================================\n\n");

    /*
     * Sample rides matching the RideShare system from Assignment 5.
     * Fares calculated using the same formulas:
     *   Standard : $2.50 + $1.20/mi
     *   Premium  : $5.00 + $2.80/mi + $4.00 luxury
     *   Shared   : ($2.00 + $1.00/mi) / passengers
     */
    double fares[] = {7.54, 16.84, 3.38, 9.70, 35.04,
                      12.70, 42.60, 5.02, 8.94, 19.64};
    double distances[] = {4.2, 2.8, 11.5, 6.0, 9.3,
                          8.5, 12.0, 2.1, 5.4, 3.8};
    int types[] = {STANDARD, PREMIUM, SHARED, STANDARD, PREMIUM,
                   STANDARD, PREMIUM, STANDARD, STANDARD, PREMIUM};
    int n = 10;

    printf("Completed Rides:\n");
    print_rides(fares, distances, types, n);

    /* ── Statistics ─────────────────────────────────────────────────────── */
    int std_cnt, prm_cnt, shd_cnt;
    frequency_table(types, n, &std_cnt, &prm_cnt, &shd_cnt);

    printf("\n=== RideShare Statistics ===\n");
    printf("  Mean Fare       : $%.2f  (avg charge per trip)\n",
           mean_fare(fares, n));
    printf("  Median Distance : %.1f mi  (typical trip length)\n",
           median_distance(distances, n));
    printf("  Mode Ride Type  : %s  (most booked type)\n",
           mode_ride_type(std_cnt, prm_cnt, shd_cnt));

    print_frequency_table(std_cnt, prm_cnt, shd_cnt, n);

    printf("\n==============================================\n");
    printf("  Done.\n");
    printf("==============================================\n");

    return 0;
}
