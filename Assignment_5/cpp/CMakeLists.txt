cmake_minimum_required(VERSION 3.16)
project(RideShare 
        VERSION 1.0 
        LANGUAGES CXX
        DESCRIPTION "Cross-language Ride Share App — C++ / Qt6 implementation")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


set(Qt6_DIR "/usr/lib/x86_64-linux-gnu/cmake/Qt6")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets UiTools)

include_directories(${PROJECT_SOURCE_DIR}/include)

# ── OOP core ──────────────────────────────────────────────────────────────────
add_library(rideshare_core STATIC
    src/Ride.cpp src/RideTypes.cpp
    src/Driver.cpp src/Rider.cpp src/RideSystem.cpp
)
target_include_directories(rideshare_core PUBLIC include)

# ── Qt6 executable ────────────────────────────────────────────────────────────
qt_add_executable(RideshareApp
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)
target_include_directories(RideshareApp PRIVATE include src)
target_link_libraries(RideshareApp PRIVATE
    rideshare_core
    Qt6::Core Qt6::Gui Qt6::Widgets Qt6::UiTools
)

# ── Copy MainWindow.ui next to the executable so QUiLoader finds it ───────────
add_custom_command(TARGET RideshareApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_SOURCE_DIR}/ui/MainWindow.ui"
        "$<TARGET_FILE_DIR:RideshareApp>/MainWindow.ui"
    COMMENT "Copying MainWindow.ui to output directory"
)

install(TARGETS RideshareApp DESTINATION bin)
install(FILES ui/MainWindow.ui DESTINATION bin)
