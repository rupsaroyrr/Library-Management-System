cmake_minimum_required(VERSION 3.16)
project(BookNest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt (Qt6 preferred, fallback to Qt5)
find_package(Qt6 COMPONENTS Core Gui Widgets Sql QUIET)
if(Qt6_FOUND)
    message(STATUS "Found Qt6: ${Qt6_VERSION}")
    set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
else()
    find_package(Qt5 COMPONENTS Core Gui Widgets Sql REQUIRED)
    message(STATUS "Found Qt5: ${Qt5_VERSION}")
    set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Sql)
endif()

set(INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(SOURCES
    src/main.cpp
    src/DatabaseManager.cpp
    src/CoverArtGenerator.cpp
    src/BookCardWidget.cpp
    src/BookDetailDialog.cpp
    src/SidebarWidget.cpp
    src/LoginDialog.cpp
    src/MainWindow.cpp
)

set(HEADERS
    include/Models.h
    include/Theme.h
    include/DatabaseManager.h
    include/CoverArtGenerator.h
    include/BookCardWidget.h
    include/BookDetailDialog.h
    include/SidebarWidget.h
    include/LoginDialog.h
    include/MainWindow.h
)

add_executable(BookNest
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(BookNest PRIVATE ${INCLUDE_DIRS})
target_link_libraries(BookNest PRIVATE ${QT_LIBS})

# On Windows with MSVC or MinGW, configure as GUI app while supporting console if needed
if(WIN32)
    set_target_properties(BookNest PROPERTIES
        OUTPUT_NAME "BookNest"
    )
endif()

# Automated test executable
add_executable(test_database
    tests/test_main.cpp
    src/DatabaseManager.cpp
)
target_include_directories(test_database PRIVATE ${INCLUDE_DIRS})
target_link_libraries(test_database PRIVATE Qt6::Core Qt6::Sql)

