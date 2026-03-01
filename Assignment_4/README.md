# Employee Schedule Manager

A cross-language scheduling application with a **shared Qt Designer UI file** consumed by both a C++/Qt6 executable and a Python/PySide6 executable.

```
SchedulerProject/
├── shared/
│   └── ui/
│       └── main_window.ui      ← single .ui file used by both languages
├── Cpp/                        ← C++ / Qt6 / CMake implementation
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── employee.h
│   │   ├── scheduler.h
│   │   ├── demo_data.h
│   │   └── main_window.h
│   └── src/
│       ├── main.cpp
│       ├── main_window.cpp
│       └── scheduler.cpp
├── python/                     ← Python / PySide6 implementation
│   ├── main.py
│   ├── requirements.txt
│   ├── models/
│   │   └── employee.py
│   ├── scheduler/
│   │   ├── scheduler.py
│   │   └── demo_data.py
│   ├── gui/
│   │    └── main_window_impl.py
|   ├──build_executable.sh        ← PyInstaller build script
├── CMakeLists.txt              ← root CMake (C++ build)
└── .gitignore
```

---

## How the shared UI works

`shared/ui/main_window.ui` is a standard Qt Designer XML file.

| Language | How it uses the `.ui` file | When |
|---|---|---|
| **C++** | CMake `AUTOUIC` compiles it → `ui_main_window.h` | Build time |
| **Python** | `QUiLoader().load(path)` reads it directly | Runtime |

Both produce identical windows from the same source.

---

## Features

- Drag-to-rank shift preferences (Morning / Afternoon / Evening)
- Two-pass scheduling algorithm — assigns preferred shifts first, fills gaps second
- Preference satisfaction rating (★★★ / ★★☆ / ★☆☆) per assignment
- Minimum staffing enforcement with warnings
- Indian demo roster (10 employees) for quick testing
- GUI mode and `--cli` demo mode in both implementations

---

## C++ Build

### Prerequisites

| Tool | Version | Notes |
|---|---|---|
| CMake | ≥ 3.22 | |
| Qt6 | ≥ 6.2 | Widgets component |
| C++ compiler | C++17 | GCC, MSVC, or Clang |

### Windows (MinGW / MSVC)

Edit `QT_PATH` in `build.bat` to match your Qt installation, then:

```bat
build.bat
```

Or manually:

```bat
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Qt\6.x.x\mingw_64"
cmake --build build --config Release --parallel
build\cpp\SchedulerApp.exe
build\cpp\SchedulerApp.exe --cli
```

### Linux / macOS

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/Cpp/SchedulerApp
./build/Cpp/SchedulerApp --cli
```

---

## Python Build

### Prerequisites

- Python 3.10+
- PySide6

### Install dependencies

```bash
pip install -r python/requirements.txt
```

### Run from source

```bash
# From project root:
python python/main.py          # GUI
python python/main.py --cli    # CLI demo
```

### Build standalone executable (PyInstaller)

```bash
# Install build tools
pip install -r python/requirements.txt

# Build (from project root)
bash ./build_executable.sh

# Run the bundle
dist/SchedulerApp/SchedulerApp          # Linux / macOS
dist\SchedulerApp\SchedulerApp.exe      # Windows
```

---

## CLI Demo output (both languages)

```
================================================================
  Employee Schedule Manager  [CLI demo]
================================================================

         WEEKLY EMPLOYEE SCHEDULE
================================================================
Day          Morning                   Afternoon                 Evening
----------------------------------------------------------------
Monday       Aarav Sharma, ...         ...                       ...
...

         EMPLOYEE SUMMARY
--------------------------------------------------------
  Aarav Sharma            Days worked: 5
    Monday        -> morning     ★★★
    Tuesday       -> morning     ★★★
    ...
```

---
