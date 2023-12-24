# MTH9815-Bond-Trading-System
A bond trading system for US Treasuries with seven securities: 2Y, 3Y, 5Y, 7Y, 10Y, 20Y, and 30Y

This document provides the instructions for deploying the MTH9815 Bond Trading System.

## Deployment Instructions

Follow these steps to deploy the Bond Trading System on your local machine.

### Step 1: Generate Input Data

First, run the `input_data.py` script to generate the necessary input data. Ensure you have Python 3 installed on your system.

```bash
python3 input_data.py
```

### Step 2: Install CMake
Install CMake using the following commands. This will update your package list and install CMake on your Ubuntu system.

```bash
sudo apt update
sudo apt install cmake
```

### Step 3: Create and Build the Project
Create a build directory for the compilation process, configure the project with CMake, and compile:

```bash
mkdir build
cd build
cmake ..
make
```

### Step 4: Run the Application
After building the project, you can run the Bond Trading System executable:

```bash
./MTH9815_Bond_Trading_System
```

## Additional Instructions

1. Downloading of boost and its configuration is taken care of via the CmakeLists.txt file
2. For testing purposes, 100000 (instead of 1000000) prices were generated, this can be easily changed to 10000000 in the input_dta.py file 
