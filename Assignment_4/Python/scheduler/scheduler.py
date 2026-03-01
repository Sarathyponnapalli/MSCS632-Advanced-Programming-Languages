"""
Python/scheduler/scheduler.py
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Two-pass scheduling algorithm with preference-aware gap filling.

Pass 1 — Preferred Assignment
    For every (day, employee) pair (employees shuffled per day for
    fairness), assign the employee's highest-ranked shift that
    doesn't violate the 5-day cap or the one-shift-per-day rule.

Pass 2 — Gap Filling
    Scan every (day, shift) slot. If headcount < MIN_EMPLOYEES_PER_SHIFT,
    pull available employees and sort them by preference_rank for that
    shift — so the person who actually wants that slot goes first.
"""

import random
from models.employee import Employee, DAYS, SHIFTS, MIN_EMPLOYEES_PER_SHIFT


class Scheduler:
    """Builds the weekly shift schedule for a pool of employees."""

    def __init__(self, employees: list[Employee]) -> None:
        self.employees: list[Employee] = employees
        # schedule[day][shift] → [name, ...]
        self.schedule: dict[str, dict[str, list[str]]] = {
            day: {shift: [] for shift in SHIFTS} for day in DAYS
        }
        self.warnings: list[str] = []

    # ── Public entry ─────────────────────────────────────────────────────────
    def build(self) -> None:
        """Reset, then run both scheduling passes."""
        self._reset()
        self._pass1_preferred()
        self._pass2_fill_gaps()

    # ── Pass 1 ───────────────────────────────────────────────────────────────
    def _pass1_preferred(self) -> None:
        for day in DAYS:
            pool = self.employees[:]
            random.shuffle(pool)                    # fairness: no fixed order

            for emp in pool:
                if not emp.is_available():
                    continue
                if emp.is_working_on(day):
                    continue

                # Walk ranked preferences; first valid shift wins.
                # "Conflict" = employee already assigned or capped (handled
                # by the guards above). If all preferences blocked, the
                # employee stays free and may be pulled in Pass 2.
                for shift in emp.preferences:
                    self._assign(emp, day, shift)
                    break

    # ── Pass 2 ───────────────────────────────────────────────────────────────
    def _pass2_fill_gaps(self) -> None:
        for day in DAYS:
            for shift in SHIFTS:
                while len(self.schedule[day][shift]) < MIN_EMPLOYEES_PER_SHIFT:
                    available = [
                        e for e in self.employees
                        if not e.is_working_on(day) and e.is_available()
                    ]
                    if not available:
                        self.warnings.append(
                            f"Cannot fill '{shift}' on {day}: "
                            "not enough available employees."
                        )
                        break

                    # Bonus: sort by preference rank so the best-match person
                    # is always chosen first even during forced assignments.
                    available.sort(key=lambda e: e.preference_rank(shift))
                    self._assign(available[0], day, shift)

    # ── Helper ───────────────────────────────────────────────────────────────
    def _assign(self, emp: Employee, day: str, shift: str) -> None:
        emp.assign(day, shift)
        self.schedule[day][shift].append(emp.name)

    def _reset(self) -> None:
        for emp in self.employees:
            emp.reset()
        self.schedule  = {day: {shift: [] for shift in SHIFTS} for day in DAYS}
        self.warnings  = []

    # ── Reporting helpers (used by GUI and CLI) ───────────────────────────────
    def get_employee_summary(self) -> list[dict]:
        """
        Returns one dict per employee (sorted by name):
            { name, days_worked, preferences, days: [(day, shift), …] }
        """
        out = []
        for emp in sorted(self.employees, key=lambda e: e.name):
            out.append({
                "name":        emp.name,
                "days_worked": emp.days_worked,
                "preferences": emp.preferences,
                "days": [
                    (d, emp.schedule[d])
                    for d in DAYS
                    if emp.is_working_on(d)
                ],
            })
        return out

    # ── CLI text output ───────────────────────────────────────────────────────
    def print_schedule(self) -> None:
        col = 26
        header = f"{'Day':<13}" + "".join(
            f"{s.capitalize():<{col}}" for s in SHIFTS
        )
        border = "=" * len(header)
        sep    = "-" * len(header)

        print(border)
        print("         WEEKLY EMPLOYEE SCHEDULE")
        print(border)
        print(header)
        print(sep)
        for day in DAYS:
            row = f"{day:<13}"
            for shift in SHIFTS:
                names = self.schedule[day][shift]
                cell  = ", ".join(names) if names else "(empty)"
                row  += f"{cell:<{col}}"
            print(row)
        print(border)

        if self.warnings:
            print("\n[WARNINGS]")
            for w in self.warnings:
                print(f"  ⚠  {w}")

    def print_employee_summary(self) -> None:
        print("\n         EMPLOYEE SUMMARY")
        print("-" * 56)
        STARS = {0: "★★★", 1: "★★☆", 2: "★☆☆"}
        for row in self.get_employee_summary():
            print(f"  {row['name']:<24} Days worked: {row['days_worked']}")
            for day, shift in row["days"]:
                rank  = row["preferences"].index(shift)
                print(f"    {day:<13} → {shift:<11} {STARS[rank]}")
        print("-" * 56)
