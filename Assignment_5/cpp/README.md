# RideShare C++ Implementation

A Qt6-based GUI application for a ride-sharing system, part of a cross-language project demonstrating advanced programming concepts.

## Description

This is the C++ implementation of a ride-share application using Qt6 for the graphical user interface. The application provides functionality for managing drivers, riders, and rides through an intuitive desktop interface.

## Prerequisites

Before building the application, ensure you have the following installed:

- **Qt6**: Version 6.10.2 or later (with MinGW 64-bit)
  - Download from: https://www.qt.io/download
  - Install the Qt Creator or Qt Maintenance Tool
- **CMake**: Version 3.16 or later
  - Download from: https://cmake.org/download/
- **MinGW-w64**: GCC compiler for Windows
  - Usually comes with Qt installation (mingw1310_64)
- **Git**: For cloning the repository (optional)

## Building on Windows

1. **Clone or download the project** to a directory on your system.

2. **Navigate to the cpp directory**:
   ```
   cd MSCS632-Advanced-Programming-Languages\Assignment_5\cpp
   ```

3. **Edit build.bat if necessary**:
   - Open `build.bat` in a text editor
   - Verify the `QT_PATH` points to your Qt installation (e.g., `C:\Qt\6.10.2\mingw_64`)
   - Verify the `MINGW_BIN` points to your MinGW bin directory (e.g., `C:\Qt\Tools\mingw1310_64\bin`)
   - Adjust `SHORT_BUILD` if you prefer a different build directory (must be short path to avoid Windows MAX_PATH issues)

4. **Run the build script**:
   ```
   .\build.bat
   ```

   The script will:
   - Clean previous builds
   - Configure the project with CMake
   - Build the application
   - Deploy Qt dependencies using `windeployqt`
   - Copy the executable and UI file to the `deploy/` folder

## Running the Application

After a successful build:

1. Navigate to the `deploy` folder:
   ```
   cd deploy
   ```

2. Run the executable:
   ```
   .\RideshareApp.exe
   ```

The application window should open, providing the ride-share system interface.

## Project Structure

- `src/`: Source code files (.cpp and .h)
- `include/`: Header files for the core classes
- `ui/`: Qt Designer UI files
- `deploy/`: Output directory for the built application and dependencies
- `CMakeLists.txt`: CMake build configuration
- `build.bat`: Windows build script

## Core Components

- **RideSystem**: Manages the overall ride-sharing logic
- **Driver**: Represents drivers in the system
- **Rider**: Represents riders requesting rides
- **Ride**: Represents individual ride instances
- **MainWindow**: Qt-based GUI for user interaction

## Troubleshooting

- **CMake not found**: Ensure CMake is installed and added to your PATH
- **Qt not found**: Verify the paths in `build.bat` match your Qt installation
- **Build fails**: Check that MinGW is properly configured and in PATH
- **Application won't start**: Ensure `MainWindow.ui` is in the same directory as the executable

## Notes

- The build script uses a short build path (`C:\b\rs`) to avoid Windows MAX_PATH limitations
- The application requires Qt6 runtime libraries, which are automatically deployed by `windeployqt`
- This is part of a multi-language assignment; see the parent directory for other implementations

## License

This project is for educational purposes as part of MSCS632-Advanced-Programming-Languages course.