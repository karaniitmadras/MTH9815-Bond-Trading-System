# MTH9815-Bond-Trading-System
A bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y

This document provides the instructions for deploying the MTH9815 Bond Trading System.

## Deployment Instructions

Follow these steps to deploy the Bond Trading System on your local machine.

### Step 1: Generate Input Data

First, run the `input_data.py` script to generate the necessary input data. Ensure you have Python 3 installed on your system.

#### Please ensure that the four input files are in the same location as the cpp and hpp files

```bash
python3 input_data.py
```


### Step 2: Install CMake
Install CMake using the following commands. This will update your package list and install CMake on your Ubuntu system.

```bash
sudo apt update
```
```bash
sudo apt install cmake
```

### Step 3: Create and Build the Project
Create a build directory for the compilation process, configure the project with CMake, and compile:

```bash
mkdir build
```
```bash
cd build
```
```bash
cmake ..
```
```bash
make
```

### Step 4: Run the Application
After building the project, you can run the Bond Trading System executable:

```bash
./MTH9815_Bond_Trading_System
```
#### As per current parameters the complete run can take up to 30 minutes, please reduce the number of prices for faster completion, details below in additional instructions
#### The output files can be found inside the build directory

## Additional Instructions

1. Downloading of boost and its configuration is taken care of via the `CMakeLists.txt` file, if boost is not detected, downloading and installing can take up to 5 minutes.
2. For testing purposes, 100000 (instead of 1000000) prices were generated, this can be easily changed to 10000000 in the `input_data.py` file under the variable name `num_rows` inside the `generate_prices` and `generate_market_data` functions, as mentioned above if complete run takes time please reduce the number of prices
