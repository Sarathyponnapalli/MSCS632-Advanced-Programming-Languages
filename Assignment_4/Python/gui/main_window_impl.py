"""
python/gui/main_window_impl.py
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
PySide6 GUI — loads shared/ui/main_window.ui via QUiLoader.

Every widget is accessed as  self.ui.<objectName>  where <objectName>
is exactly the name set in Qt Designer / the .ui XML.  No widgets are
constructed in Python; no aliases are created.

Widget → .ui objectName mapping
────────────────────────────────────────────────────────────
self.ui.tabWidget            QTabWidget   — 3-tab container
self.ui.leEmployeeName       QLineEdit    — employee name input
self.ui.lwPreferences        QListWidget  — drag-to-rank shifts
self.ui.btnAddEmployee       QPushButton
self.ui.btnLoadDemo          QPushButton
self.ui.btnClearAll          QPushButton
self.ui.btnRemoveEmployee    QPushButton
self.ui.btnBuildSchedule     QPushButton
self.ui.tblEmployees         QTableWidget — registered employees
self.ui.tblSchedule          QTableWidget — weekly grid
self.ui.tblSummary           QTableWidget — per-employee summary
self.ui.teWarnings           QTextEdit    — staffing warnings
self.ui.lblStatus            QLabel       — status strip
self.ui.actionLoadDemo       QAction
self.ui.actionExit           QAction
self.ui.actionAbout          QAction
"""

import os
import sys

from PySide6.QtWidgets import (
    QMainWindow, QTableWidgetItem, QHeaderView, QMessageBox,
)
from PySide6.QtCore import Qt
from PySide6.QtGui import QBrush, QColor, QFont
from PySide6.QtUiTools import QUiLoader

from models.employee import Employee, DAYS, SHIFTS
from scheduler.scheduler import Scheduler
from scheduler.demo_data import make_demo_employees

# ── Colour palette (mirrors C++ SHIFT_BG / SHIFT_FG) ─────────────────────────
SHIFT_BG: dict[str, QColor] = {
    "morning":   QColor("#FFF3CD"),
    "afternoon": QColor("#D1ECF1"),
    "evening":   QColor("#E2D9F3"),
}
SHIFT_FG: dict[str, QColor] = {
    "morning":   QColor("#6D4C00"),   # dark amber  readable on #FFF3CD
    "afternoon": QColor("#0A4550"),   # dark teal   readable on #D1ECF1
    "evening":   QColor("#3D1A52"),   # dark violet readable on #E2D9F3
}
SHIFT_EMOJI: dict[str, str] = {
    "morning": "🌅", "afternoon": "☀️", "evening": "🌙",
}
STARS: dict[int, str] = {0: "★★★", 1: "★★☆", 2: "★☆☆"}
EMPTY_BG = QColor("#F8D7DA")
EMPTY_FG = QColor("#721C24")
DARK_TEXT = QColor("#1A252F")
DIM_TEXT = QColor("#888888")


def _ui_path() -> str:
    """
    Resolve shared/ui/main_window.ui regardless of how the app is launched.

    Source tree (python python/main.py from project root):
        __file__ = <root>/python/gui/main_window_impl.py
        go up 3 dirs  →  project root  →  shared/ui/main_window.ui

    PyInstaller bundle (--add-data "shared/ui:shared/ui"):
        sys._MEIPASS/<bundle>/shared/ui/main_window.ui
    """
    if getattr(sys, "frozen", False):
        base = sys._MEIPASS                                     # type: ignore
    else:
        base = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                            "..", "..", "..")
    return os.path.normpath(os.path.join(base, "shared", "ui", "main_window.ui"))


def _cell(text:  str,
          bg:    QColor | None = None,
          fg:    QColor = DARK_TEXT,
          align: Qt.AlignmentFlag = Qt.AlignVCenter | Qt.AlignLeft,
          bold:  bool = False) -> QTableWidgetItem:
    """Create a QTableWidgetItem with an explicit foreground colour.

    setForeground() must be called on every cell — without it Qt's palette
    can silently inherit white text, making cells unreadable on Windows.
    """
    item = QTableWidgetItem(text)
    item.setForeground(QBrush(fg))
    item.setTextAlignment(align)
    if bg is not None:
        item.setBackground(QBrush(bg))
    if bold:
        f = QFont()
        f.setBold(True)
        item.setFont(f)
    return item


class MainWindow(QMainWindow):

    def __init__(self) -> None:
        super().__init__()
        self._employees: list[Employee] = []
        self._scheduler: Scheduler | None = None

        # ── Load shared .ui ───────────────────────────────────────────────────
        path = _ui_path()
        if not os.path.exists(path):
            raise FileNotFoundError(
                f"UI file not found: {path}\n"
                "Run from the project root, or rebuild the bundle with\n"
                "--add-data 'shared/ui:shared/ui'"
            )
        self.ui = QUiLoader().load(path, self)
        if self.ui is None:
            raise RuntimeError(f"QUiLoader failed to load: {path}")

        # Plug the loaded widget tree into this QMainWindow shell
        self.setCentralWidget(self.ui.centralwidget)
        self.setMenuBar(self.ui.menubar)
        self.setStatusBar(self.ui.statusbar)
        self.setWindowTitle("Employee Schedule Manager")
        self.resize(1150, 800)

        self._apply_stylesheet()
        self._configure_tables()
        self._connect_signals()
        self._set_status("Ready — add employees and press ⚙ Build Schedule.")

    # ── Stylesheet ────────────────────────────────────────────────────────────
    def _apply_stylesheet(self) -> None:
        self.setStyleSheet("""
            QMainWindow { background: #F8F9FA; }

            QLabel#lblBanner {
                background: #1A252F; color: #FFFFFF;
                font-size: 20px; font-weight: bold;
                padding: 10px; letter-spacing: 1px;
            }

            QTabWidget::pane {
                border: 1px solid #CED4DA; border-radius: 4px; background: #FFFFFF;
            }
            QTabBar::tab {
                background: #E9ECEF; color: #333333;
                padding: 8px 22px; font-size: 13px;
                border-top-left-radius: 4px; border-top-right-radius: 4px;
            }
            QTabBar::tab:selected {
                background: #FFFFFF; color: #1A252F;
                font-weight: bold; border-bottom: 3px solid #2980B9;
            }

            QPushButton {
                background: #2980B9; color: white;
                border: none; border-radius: 5px; padding: 6px 14px; font-size: 13px;
            }
            QPushButton:hover   { background: #2471A3; }
            QPushButton:pressed { background: #1A5276; }
            QPushButton#btnRemoveEmployee       { background: #C0392B; }
            QPushButton#btnRemoveEmployee:hover { background: #A93226; }
            QPushButton#btnLoadDemo             { background: #1E8449; }
            QPushButton#btnLoadDemo:hover       { background: #196F3D; }
            QPushButton#btnClearAll             { background: #7F8C8D; }
            QPushButton#btnClearAll:hover       { background: #717D7E; }
            QPushButton#btnBuildSchedule        { background: #7D3C98; font-size: 14px; font-weight: bold; }
            QPushButton#btnBuildSchedule:hover  { background: #6C3483; }

            QGroupBox {
                font-size: 13px; font-weight: bold; color: #1A252F;
                border: 1px solid #CED4DA; border-radius: 6px;
                margin-top: 12px; padding-top: 14px;
            }
            QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; }

            QTableWidget {
                border: 1px solid #DEE2E6; border-radius: 4px;
                gridline-color: #DEE2E6; font-size: 12px; color: #1A252F;
            }
            QTableWidget::item          { color: #1A252F; padding: 2px 4px; }
            QTableWidget::item:selected { background: #AED6F1; color: #1A252F; }
            QHeaderView::section {
                background: #1A252F; color: #FFFFFF;
                padding: 6px; font-size: 12px; font-weight: bold; border: none;
            }

            QListWidget {
                border: 2px dashed #ADB5BD; border-radius: 4px;
                font-size: 13px; color: #1A252F; background: #FFFFFF;
            }
            QListWidget::item          { padding: 7px 10px; border-bottom: 1px solid #DEE2E6; color: #1A252F; }
            QListWidget::item:selected { background: #D6EAF8; color: #1A252F; }

            QLineEdit {
                border: 1px solid #CED4DA; border-radius: 4px;
                padding: 6px; font-size: 13px; color: #1A252F; background: #FFFFFF;
            }
            QLineEdit:focus { border: 2px solid #2980B9; }

            QLabel#lblStatus {
                background: #ECF0F1; color: #555555;
                padding: 4px 12px; font-size: 12px; border-top: 1px solid #CED4DA;
            }

            QTextEdit {
                border: 1px solid #CED4DA; border-radius: 4px;
                font-size: 12px; color: #1A252F; background: #FFFFFF;
            }
        """)

    # ── Table column/row behaviour (layout comes from .ui, not here) ──────────
    def _configure_tables(self) -> None:
        # tblEmployees: name stretches, 3 choice columns auto-size
        h = self.ui.tblEmployees.horizontalHeader()
        h.setSectionResizeMode(0, QHeaderView.Stretch)
        for c in (1, 2, 3):
            h.setSectionResizeMode(c, QHeaderView.ResizeToContents)

        # tblSchedule: row/col headers are data-driven (not fixed in .ui)
        self.ui.tblSchedule.setRowCount(len(DAYS))
        self.ui.tblSchedule.setColumnCount(len(SHIFTS))
        self.ui.tblSchedule.setVerticalHeaderLabels(DAYS)
        self.ui.tblSchedule.setHorizontalHeaderLabels(
            [f"{SHIFT_EMOJI[s]}  {s.capitalize()}" for s in SHIFTS]
        )
        sh = self.ui.tblSchedule.horizontalHeader()
        for c in range(len(SHIFTS)):
            sh.setSectionResizeMode(c, QHeaderView.Stretch)
        self.ui.tblSchedule.verticalHeader().setDefaultSectionSize(70)

        # tblSummary: name/days/prefs + 7 day columns
        sh2 = self.ui.tblSummary.horizontalHeader()
        sh2.setSectionResizeMode(0, QHeaderView.ResizeToContents)
        sh2.setSectionResizeMode(1, QHeaderView.ResizeToContents)
        sh2.setSectionResizeMode(2, QHeaderView.Stretch)
        for c in range(3, 10):
            sh2.setSectionResizeMode(c, QHeaderView.ResizeToContents)

    # ── Connect .ui widget signals to Python slots ────────────────────────────
    def _connect_signals(self) -> None:
        self.ui.btnAddEmployee   .clicked.connect(self._on_add_employee)
        self.ui.btnLoadDemo      .clicked.connect(self._on_load_demo)
        self.ui.btnClearAll      .clicked.connect(self._on_clear_all)
        self.ui.btnRemoveEmployee.clicked.connect(self._on_remove_employee)
        self.ui.btnBuildSchedule .clicked.connect(self._on_build_schedule)
        self.ui.actionLoadDemo   .triggered.connect(self._on_load_demo)
        self.ui.actionExit       .triggered.connect(self.close)
        self.ui.actionAbout      .triggered.connect(self._on_about)

    # ── Slots ─────────────────────────────────────────────────────────────────

    def _on_add_employee(self) -> None:
        name = self.ui.leEmployeeName.text().strip()
        if not name:
            self._set_status("⚠  Please enter an employee name.", error=True)
            return

        # lwPreferences items look like "🌅  morning" — last token is shift name
        prefs = [
            self.ui.lwPreferences.item(i).text().split()[-1]
            for i in range(self.ui.lwPreferences.count())
        ]

        try:
            emp = Employee(name, prefs)
        except ValueError as exc:
            self._set_status(f"⚠  {exc}", error=True)
            return

        if any(e.name == emp.name for e in self._employees):
            self._set_status(f"⚠  '{name}' is already registered.", error=True)
            return

        self._employees.append(emp)
        self._refresh_employee_table()
        self.ui.leEmployeeName.clear()
        self._set_status(
            f"✓  {name} added — 1st: {prefs[0]}, 2nd: {prefs[1]}, 3rd: {prefs[2]}. "
            f"Total: {len(self._employees)}."
        )

    def _on_load_demo(self) -> None:
        self._employees = make_demo_employees()
        self._refresh_employee_table()
        self._set_status(
            f"✓  Loaded {len(self._employees)} Indian demo employees.")

    def _on_clear_all(self) -> None:
        self._employees = []
        self._refresh_employee_table()
        self.ui.tblSchedule.clearContents()
        self.ui.tblSummary.setRowCount(0)
        self.ui.teWarnings.clear()
        self._set_status("All employees cleared.")

    def _on_remove_employee(self) -> None:
        rows = sorted(
            {idx.row() for idx in self.ui.tblEmployees.selectedIndexes()},
            reverse=True,
        )
        if not rows:
            self._set_status("⚠  Select a row first.", error=True)
            return
        for r in rows:
            del self._employees[r]
        self._refresh_employee_table()
        self._set_status(f"Removed {len(rows)} employee(s).")

    def _on_build_schedule(self) -> None:
        if len(self._employees) < 2:
            QMessageBox.warning(self, "Not Enough Employees",
                                "Please add at least 2 employees before building a schedule.")
            return
        self._scheduler = Scheduler(self._employees)
        self._scheduler.build()
        self._populate_schedule_tab()
        self._populate_summary_tab()
        self._populate_warnings()
        self.ui.tabWidget.setCurrentIndex(1)
        wc = len(self._scheduler.warnings)
        self._set_status(
            f"✓  Schedule built for {len(self._employees)} employees.  Warnings: {wc}.",
            error=wc > 0,
        )

    def _on_about(self) -> None:
        QMessageBox.information(self, "About",
                                "Employee Schedule Manager\n\n"
                                "Python  →  PySide6 + QUiLoader\n"
                                "C++     →  Qt6 + AUTOUIC\n\n"
                                "Shared layout:  shared/ui/main_window.ui\n\n"
                                "  • Drag-and-rank shift preferences\n"
                                "  • Two-pass scheduling algorithm\n"
                                "  • ★ rating per assigned shift\n")

    # ── Table population ──────────────────────────────────────────────────────

    def _refresh_employee_table(self) -> None:
        t = self.ui.tblEmployees
        t.setRowCount(len(self._employees))
        for r, emp in enumerate(self._employees):
            t.setItem(r, 0,
                      _cell(emp.name, QColor("#FFFFFF"), DARK_TEXT, bold=True))
            for c, shift in enumerate(emp.preferences):
                t.setItem(r, c + 1, _cell(
                    f"{SHIFT_EMOJI[shift]}  {shift.capitalize()}",
                    SHIFT_BG[shift], SHIFT_FG[shift],
                    Qt.AlignCenter | Qt.AlignVCenter, bold=True,
                ))
        t.resizeRowsToContents()

    def _populate_schedule_tab(self) -> None:
        t = self.ui.tblSchedule
        sched = self._scheduler.schedule
        for r, day in enumerate(DAYS):
            for c, shift in enumerate(SHIFTS):
                names = sched[day][shift]
                if names:
                    t.setItem(r, c, _cell(
                        "\n".join(names),
                        SHIFT_BG[shift], SHIFT_FG[shift],
                        Qt.AlignCenter | Qt.AlignVCenter,
                    ))
                else:
                    t.setItem(r, c, _cell(
                        "(empty)", EMPTY_BG, EMPTY_FG,
                        Qt.AlignCenter | Qt.AlignVCenter, bold=True,
                    ))
        t.resizeRowsToContents()

    def _populate_summary_tab(self) -> None:
        t = self.ui.tblSummary
        rows = self._scheduler.get_employee_summary()
        t.setRowCount(len(rows))
        for r, row in enumerate(rows):
            t.setItem(r, 0, _cell(row["name"], bold=True))
            t.setItem(r, 1, _cell(str(row["days_worked"]),
                                  align=Qt.AlignCenter | Qt.AlignVCenter))
            t.setItem(r, 2, _cell(
                "  →  ".join(
                    f"{SHIFT_EMOJI[s]} {s.capitalize()}"
                    for s in row["preferences"]
                )
            ))
            day_map = dict(row["days"])
            for d, day in enumerate(DAYS):
                shift = day_map.get(day, "")
                if shift:
                    rank = row["preferences"].index(shift)
                    t.setItem(r, 3 + d, _cell(
                        f"{SHIFT_EMOJI[shift]} {shift.capitalize()}\n{STARS[rank]}",
                        SHIFT_BG[shift], SHIFT_FG[shift],
                        Qt.AlignCenter | Qt.AlignVCenter,
                    ))
                else:
                    t.setItem(r, 3 + d, _cell(
                        "–", fg=DIM_TEXT,
                        align=Qt.AlignCenter | Qt.AlignVCenter,
                    ))
        t.resizeRowsToContents()

    def _populate_warnings(self) -> None:
        self.ui.teWarnings.clear()
        if not self._scheduler.warnings:
            self.ui.teWarnings.setPlainText(
                "✓  No warnings — all shifts fully staffed.")
        else:
            self.ui.teWarnings.setPlainText(
                "\n".join(f"⚠  {w}" for w in self._scheduler.warnings)
            )

    # ── Status strip ──────────────────────────────────────────────────────────
    def _set_status(self, msg: str, error: bool = False) -> None:
        bg = "#FADBD8" if error else "#D5F5E3"
        fg = "#922B21" if error else "#1E8449"
        self.ui.lblStatus.setStyleSheet(
            f"background:{bg};color:{fg};"
            f"padding:4px 12px;font-size:12px;border-top:1px solid #CED4DA;"
        )
        self.ui.lblStatus.setText(msg)
