"""
Python/scheduler/demo_data.py
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Pre-built Indian employee roster used by both the GUI demo
button and the CLI demo mode.
"""

from models.employee import Employee

_ROSTER: list[tuple[str, list[str]]] = [
    ("Aarav Sharma",   ["morning",   "afternoon", "evening"]),
    ("Priya Patel",    ["afternoon", "morning",   "evening"]),
    ("Rohan Mehta",    ["evening",   "morning",   "afternoon"]),
    ("Divya Nair",     ["morning",   "evening",   "afternoon"]),
    ("Kiran Reddy",    ["afternoon", "evening",   "morning"]),
    ("Sneha Iyer",     ["evening",   "afternoon", "morning"]),
    ("Arjun Gupta",    ["morning",   "afternoon", "evening"]),
    ("Meera Joshi",    ["afternoon", "morning",   "evening"]),
    ("Vikram Singh",   ["evening",   "morning",   "afternoon"]),
    ("Ananya Pillai",  ["morning",   "evening",   "afternoon"]),
]


def make_demo_employees() -> list[Employee]:
    """Return a fresh list of Employee objects from the Indian demo roster."""
    return [Employee(name, prefs) for name, prefs in _ROSTER]
