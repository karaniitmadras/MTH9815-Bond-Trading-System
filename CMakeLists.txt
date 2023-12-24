cmake_minimum_required(VERSION 3.10)

# Set your project name
project(MTH9815_Bond_Trading_System)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories for header files
include_directories(${CMAKE_SOURCE_DIR}/Header_Files)
include_directories(${CMAKE_BINARY_DIR}/_deps/boost-src/)

# Find Boost library
find_package(Boost 1.82 QUIET) # Specify the version of Boost you need
# include_directories(${Boost_INCLUDE_DIRS})

if(Boost_FOUND)
    message(STATUS "Boost found: ${Boost_INCLUDE_DIRS}")
else()
    message(STATUS "Boost not found, downloading and building Boost")

    include(FetchContent)
    FetchContent_Declare(
        Boost
        URL https://boostorg.jfrog.io/artifactory/main/release/1.82.0/source/boost_1_82_0.tar.gz
    )

    FetchContent_GetProperties(Boost)
    if(NOT Boost_POPULATED)
        FetchContent_Populate(Boost)
        set(BOOST_ROOT ${boost_SOURCE_DIR})
        set(Boost_INCLUDE_DIR ${boost_SOURCE_DIR})
    endif()
endif()

# Add source files
add_executable(MTH9815_Bond_Trading_System main.cpp
    bondalgoexecutionservice.hpp
    bondalgostreamingservice.hpp
    bondexecutionhistoricaldataservice.hpp
    bondexecutionservice.hpp
    bondinquiryservice.hpp
    bondmarketdataservice.hpp
    bondpositionhistoricaldataservice.hpp
    bondpositionservice.hpp
    bondpricestreamshistoricaldataservice.hpp
    bondpricingservice.hpp
    bondproductservice.hpp
    bondriskhistoricaldataservice.hpp
    bondriskservice.hpp
    bondstreamingservice.hpp
    bondtradebookingservice.hpp
    executionservice.hpp
    formatting.hpp
    GUIService.hpp
    historicaldataservice.hpp
    inputfileconnector.hpp
    inquiryservice.hpp
    marketdataservice.hpp
    outputfileconnector.hpp
    positionservice.hpp
    pricingservice.hpp
    products.hpp
    riskservice.hpp
    soa.hpp
    streamingservice.hpp
    tradebookingservice.hpp
    # Add other .cpp files as needed
)

# Link Boost libraries if needed
# target_link_libraries(MTH9815_Bond_Trading_System ${Boost_LIBRARIES})

# Link Boost libraries if needed
if(Boost_FOUND)
    target_include_directories(MTH9815_Bond_Trading_System PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_libraries(MTH9815_Bond_Trading_System ${Boost_LIBRARIES})
endif()

# Copy resource files to build directory
configure_file(${CMAKE_SOURCE_DIR}/inquiries.csv ${CMAKE_BINARY_DIR}/inquiries.csv COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/marketdata.csv ${CMAKE_BINARY_DIR}/marketdata.csv COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/prices.csv ${CMAKE_BINARY_DIR}/prices.csv COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/trades.csv ${CMAKE_BINARY_DIR}/trades.csv COPYONLY)
