(* stats.ml  -  RideShare Statistics Calculator (Functional)
   Course   : MSCS-632 Advanced Programming Languages
   Author   : Parthasarathi Ponnapalli
   Paradigm : Functional (OCaml)

   Analyses completed rides from the RideShare system.
   Calculates mean fare, median distance, mode ride type,
   and a frequency table of ride types — using immutable
   lists and higher-order functions throughout.
   No mutable state is used anywhere.                      *)

(* ── Ride type ──────────────────────────────────────────────────────────── *)
type ride_type = Standard | Premium | Shared

let type_name = function
  | Standard -> "Standard"
  | Premium  -> "Premium"
  | Shared   -> "Shared"

(* ── RideShare dataset ──────────────────────────────────────────────────── *)
(*
 * Each ride is a tuple: (fare, distance, ride_type).
 * OCaml's algebraic data types make ride_type a proper first-class value
 * rather than a raw integer constant — more expressive than C.
 *
 * Fares match the Assignment 5 formulas:
 *   Standard : $2.50 + $1.20/mi
 *   Premium  : $5.00 + $2.80/mi + $4.00 luxury
 *   Shared   : ($2.00 + $1.00/mi) / passengers
 *)
let rides = [
  (  7.54,  4.2, Standard );
  ( 16.84,  2.8, Premium  );
  (  3.38, 11.5, Shared   );
  (  9.70,  6.0, Standard );
  ( 35.04,  9.3, Premium  );
  ( 12.70,  8.5, Standard );
  ( 42.60, 12.0, Premium  );
  (  5.02,  2.1, Standard );
  (  8.94,  5.4, Standard );
  ( 19.64,  3.8, Premium  );
]

(* ── Projection helpers ─────────────────────────────────────────────────── *)
let fares     = List.map (fun (f, _, _) -> f) rides
let distances = List.map (fun (_, d, _) -> d) rides
let types     = List.map (fun (_, _, t) -> t) rides

(* ── Mean fare ──────────────────────────────────────────────────────────── *)
(*
 * Business meaning: average amount charged per trip.
 * fold_left accumulates the sum without any loop variable.
 *)
let mean_fare fares =
  let sum = List.fold_left ( +. ) 0.0 fares in
  sum /. float_of_int (List.length fares)

(* ── Median distance ────────────────────────────────────────────────────── *)
(*
 * Business meaning: typical trip length — resistant to skew from
 * outlier long-distance rides, unlike the mean.
 * List.sort returns a new sorted list; the original is unchanged.
 *)
let median_distance distances =
  let sorted = List.sort compare distances in
  let n      = List.length sorted in
  if n mod 2 = 1 then
    List.nth sorted (n / 2)
  else
    (List.nth sorted (n / 2 - 1) +. List.nth sorted (n / 2)) /. 2.0

(* ── Frequency table of ride types ─────────────────────────────────────── *)
(*
 * Additional functionality:
 * Business meaning: demand breakdown by ride category.
 * Built with fold_left over the types list — counting Standard,
 * Premium, and Shared occurrences without any mutable counters.
 *)
let frequency_table types =
  List.fold_left (fun (s, p, sh) t ->
    match t with
    | Standard -> (s + 1, p,     sh    )
    | Premium  -> (s,     p + 1, sh    )
    | Shared   -> (s,     p,     sh + 1)
  ) (0, 0, 0) types

(* ── Mode ride type ─────────────────────────────────────────────────────── *)
(*
 * Business meaning: the most commonly booked ride type.
 * Derived from the frequency table tuple — no second pass needed.
 *)
let mode_ride_type (std_cnt, prm_cnt, shd_cnt) =
  let max_cnt = max std_cnt (max prm_cnt shd_cnt) in
  if      std_cnt = max_cnt then Standard
  else if prm_cnt = max_cnt then Premium
  else                           Shared

(* ── Print helpers ──────────────────────────────────────────────────────── *)
let print_rides () =
  Printf.printf "  %-4s  %-10s  %-10s  %-10s\n"
    "#" "Type" "Distance" "Fare";
  Printf.printf "  %-4s  %-10s  %-10s  %-10s\n"
    "----" "----------" "----------" "----------";
  List.iteri (fun i (f, d, t) ->
    Printf.printf "  %-4d  %-10s  %7.1f mi  $%8.2f\n"
      (i + 1) (type_name t) d f
  ) rides

let print_frequency_table (std_cnt, prm_cnt, shd_cnt) =
  let n = float_of_int (std_cnt + prm_cnt + shd_cnt) in
  print_endline "\nRide Type Frequency Table:";
  Printf.printf "  %-10s  %5s  %7s\n" "Type" "Count" "Share";
  Printf.printf "  %-10s  %5s  %7s\n" "----------" "-----" "-------";
  Printf.printf "  %-10s  %5d  %6.1f%%\n"
    "Standard" std_cnt (100.0 *. float_of_int std_cnt /. n);
  Printf.printf "  %-10s  %5d  %6.1f%%\n"
    "Premium"  prm_cnt (100.0 *. float_of_int prm_cnt /. n);
  Printf.printf "  %-10s  %5d  %6.1f%%\n"
    "Shared"   shd_cnt (100.0 *. float_of_int shd_cnt /. n)

(* ── Main ───────────────────────────────────────────────────────────────── *)
let () =
  print_endline "==============================================";
  print_endline "  RideShare Statistics  --  OCaml (Functional)";
  print_endline "  MSCS-632  Advanced Programming Languages";
  print_endline "==============================================\n";

  print_endline "Completed Rides:";
  print_rides ();

  let freq = frequency_table types in

  print_endline "\n=== RideShare Statistics ===";
  Printf.printf "  Mean Fare       : $%.2f  (avg charge per trip)\n"
    (mean_fare fares);
  Printf.printf "  Median Distance : %.1f mi  (typical trip length)\n"
    (median_distance distances);
  Printf.printf "  Mode Ride Type  : %s  (most booked type)\n"
    (type_name (mode_ride_type freq));
  print_frequency_table freq;

  print_endline "\n==============================================";
  print_endline "  Done.";
  print_endline "=============================================="
