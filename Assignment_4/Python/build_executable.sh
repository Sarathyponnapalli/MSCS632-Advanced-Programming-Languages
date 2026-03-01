#!/usr/bin/env bash
# scripts/build_python.sh
# ─────────────────────────────────────────────────────────────────────────────
# Builds the Python/PySide6 GUI into a one-folder executable using PyInstaller.
#
# Run from the project root (where CMakeLists.txt lives):
#   bash scripts/build_python.sh
#
# Output:
#   dist/SchedulerApp/SchedulerApp        (Linux / macOS)
#   dist/SchedulerApp/SchedulerApp.exe    (Windows / Git-Bash / WSL)
# ─────────────────────────────────────────────────────────────────────────────
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"


# ── Guard: PySide6 must be importable ────────────────────────────────────────
if ! python -c "import PySide6" 2>/dev/null; then
  echo "ERROR: PySide6 not found."
  echo "       Install with:  pip install -r python/requirements.txt"
  exit 1
fi

# ── Guard: shared .ui must exist ─────────────────────────────────────────────
UI_FILE="shared/ui/main_window.ui"
if [ ! -f "$UI_FILE" ]; then
  echo "ERROR: $UI_FILE not found."
  echo "       Run from the project root directory."
  exit 1
fi

echo ""
echo "═══════════════════════════════════════════════════════════"
echo "  Employee Schedule Manager — PyInstaller Build"
echo "═══════════════════════════════════════════════════════════"
echo "  Root    : $ROOT_DIR"
echo "  UI file : $UI_FILE"
echo ""

# ── Clean previous dist/build artefacts ──────────────────────────────────────
echo "[1/3] Cleaning previous build..."
rm -rf build dist
echo "      Done."
echo ""

# ── Run PyInstaller ───────────────────────────────────────────────────────────
# --add-data "shared/ui:shared/ui"
#   Bundles the .ui file at  _MEIPASS/shared/ui/main_window.ui
#   which is exactly where _ui_path() in main_window_impl.py looks.
#
# Path separator is ':' on Linux/macOS, ';' on Windows.
# Detect and set accordingly so the same script works everywhere.
echo "[2/3] Running PyInstaller..."

if [[ "$OSTYPE" == "msys" || "$OSTYPE" == "win32" || "$OSTYPE" == "cygwin" ]]; then
  SEP=";"
else
  SEP=":"
fi

pyinstaller \
  --noconfirm \
  --clean \
  --name SchedulerApp \
  --windowed \
  --onedir \
  --add-data "shared/ui${SEP}shared/ui" \
  --hidden-import PySide6.QtUiTools \
  --hidden-import PySide6.QtWidgets \
  --hidden-import PySide6.QtCore \
  --hidden-import PySide6.QtGui \
  python/main.py

echo ""
echo "[3/3] Done!"
echo ""
echo "  Executable:  dist/SchedulerApp/SchedulerApp"
echo "  (On Windows: dist/SchedulerApp/SchedulerApp.exe)"
echo ""
echo "═══════════════════════════════════════════════════════════"
