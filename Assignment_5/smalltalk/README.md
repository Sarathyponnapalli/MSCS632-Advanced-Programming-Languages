# Smalltalk RideShare Implementation

This directory contains the Smalltalk implementation of the RideShare system for MSCS-632 Advanced Programming Languages.

## Prerequisites

To run the Smalltalk code, you need a Smalltalk environment. The recommended option is GNU Smalltalk (gst), as the code is designed to be compatible with it.

### Installing GNU Smalltalk

- **Windows**: Download from [GNU Smalltalk website](https://www.gnu.org/software/smalltalk/) or use a package manager like Chocolatey: `choco install gnu-smalltalk`
- **macOS**: Use Homebrew: `brew install gnu-smalltalk`
- **Linux**: Use your package manager, e.g., `sudo apt install gnu-smalltalk` on Ubuntu

Alternatively, you can use other Smalltalk environments like Pharo or Squeak, but you may need to adapt the execution method.

## Execution Instructions

1. Open a terminal/command prompt.
2. Navigate to the `smalltalk` directory:
   ```
   cd MSCS632-Advanced-Programming-Languages\Assignment_5\smalltalk
   ```
3. Run the Smalltalk script using GNU Smalltalk:
   ```
   gst RideShare.st
   ```

The script will execute and display the demonstration output in the terminal, showing examples of encapsulation, inheritance, and polymorphism in the RideShare system.

## What the Code Does

The `RideShare.st` file defines a complete object-oriented RideShare system with:
- `Ride` base class with subclasses `StandardRide`, `PremiumRide`, and `SharedRide`
- `Driver` and `Rider` classes
- `RideSystem` class to manage the entire system
- A demonstration script that creates sample data and showcases OOP principles

The output includes ride details, driver and rider information, and system summaries.