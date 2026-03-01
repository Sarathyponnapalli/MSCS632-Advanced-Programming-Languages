"""
Python/models/employee.py
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Employee data-model with ranked shift preferences.
"""

# ── Shared constants ──────────────────────────────────────────────────────────
DAYS:   list[str] = [
    "Monday","Tuesday","Wednesday","Thursday","Friday","Saturday","Sunday"
]
SHIFTS: list[str] = ["morning", "afternoon", "evening"]

MAX_DAYS_PER_WEEK:       int = 5
MIN_EMPLOYEES_PER_SHIFT: int = 2


class Employee:
    """
    Represents a company employee.

    Attributes
    ----------
    name        : str
    preferences : list[str]
        Ranked from most-preferred (index 0) to least-preferred (index 2).
    days_worked : int
        Running count of shift assignments this week.
    schedule    : dict[str, str | None]
        Maps every day-name to the assigned shift, or None if unassigned.
    """

    # ── Construction ──────────────────────────────────────────────────────────
    def __init__(self, name: str, preferences: list[str]) -> None:
        name = name.strip()
        if not name:
            raise ValueError("Employee name cannot be empty.")
        if len(preferences) != len(SHIFTS):
            raise ValueError(f"Exactly {len(SHIFTS)} preferences required.")
        for p in preferences:
            if p not in SHIFTS:
                raise ValueError(f"'{p}' is not a valid shift. Use: {SHIFTS}")
        if len(set(preferences)) != len(SHIFTS):
            raise ValueError("Each shift must appear exactly once.")

        self.name:        str                   = name
        self.preferences: list[str]             = list(preferences)
        self.days_worked: int                   = 0
        self.schedule:    dict[str, str | None] = {d: None for d in DAYS}

    # ── Queries ───────────────────────────────────────────────────────────────
    def is_available(self) -> bool:
        """True when the employee hasn't hit the 5-day cap."""
        return self.days_worked < MAX_DAYS_PER_WEEK

    def is_working_on(self, day: str) -> bool:
        return self.schedule[day] is not None

    def preference_rank(self, shift: str) -> int:
        """0 = most preferred, 2 = least preferred, 99 if unknown."""
        try:
            return self.preferences.index(shift)
        except ValueError:
            return 99

    # ── Mutation ──────────────────────────────────────────────────────────────
    def assign(self, day: str, shift: str) -> None:
        if self.is_working_on(day):
            raise RuntimeError(
                f"{self.name} is already assigned on {day} "
                f"(current shift: {self.schedule[day]})."
            )
        self.schedule[day] = shift
        self.days_worked += 1

    def reset(self) -> None:
        """Clear all assignments so the scheduler can re-run."""
        self.days_worked = 0
        self.schedule    = {d: None for d in DAYS}

    # ── Dunder ────────────────────────────────────────────────────────────────
    def __repr__(self) -> str:
        return (
            f"Employee(name={self.name!r}, "
            f"preferences={self.preferences}, "
            f"days_worked={self.days_worked})"
        )
