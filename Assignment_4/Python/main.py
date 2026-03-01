"""
Python/main.py
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Entry point for the Employee Schedule Manager (Python).

Usage
─────
  python main.py          →  launches PySide6 Qt GUI
  python main.py --cli    →  CLI demo (no display required)

PyInstaller bundle
──────────────────
  The spec file (SchedulerApp.spec) packages this file as the
  entry-point and includes shared/ui/main_window.ui as a data
  file so QUiLoader can find it at runtime.
"""

import sys
import os

# ── Path bootstrap ────────────────────────────────────────────────────────────
# Ensure that "models", "scheduler", and "gui" packages are importable
# whether we run from:
#   • the source tree   (python main.py from Python/)
#   • a PyInstaller bundle  (sys.frozen = True)
_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)


# ── GUI ───────────────────────────────────────────────────────────────────────
def run_gui() -> None:
    from PySide6.QtWidgets import QApplication
    from gui.main_window_impl import MainWindow

    app = QApplication(sys.argv)
    app.setApplicationName("Employee Schedule Manager")
    app.setOrganizationName("SchedulerProject")
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


# ── CLI / demo ────────────────────────────────────────────────────────────────
def run_cli() -> None:
    from scheduler.scheduler import Scheduler
    from scheduler.demo_data import make_demo_employees

    print("=" * 64)
    print("  Employee Schedule Manager  [Python / CLI demo]")
    print("=" * 64)

    employees = make_demo_employees()
    print(f"\nLoaded {len(employees)} employees:\n")
    for emp in employees:
        print(
            f"  {emp.name:<24} "
            f"prefs: {' > '.join(emp.preferences)}"
        )

    sched = Scheduler(employees)
    sched.build()
    print()
    sched.print_schedule()
    sched.print_employee_summary()


# ── Entry ─────────────────────────────────────────────────────────────────────
if __name__ == "__main__":
    if "--cli" in sys.argv:
        run_cli()
    else:
        try:
            run_gui()
        except ImportError as exc:
            print(
                f"[INFO] PySide6 not available ({exc}).\n"
                "       Falling back to CLI demo mode.\n"
            )
            run_cli()
