#!/bin/bash

start_time=$(date +%s)

skip_tests=false

while [ "$#" -gt 0 ]; do
    case $1 in
        --skip-tests) skip_tests=true ;;
        *) echo "Unknown parameter passed: $1"; exit 1 ;;
    esac
    shift
done

CUR_DIR=$(cd "$(dirname "$0")"; pwd)
cd ${CUR_DIR}/..
echo "Current directory: $CUR_DIR"

echo "Checking prerequisites..."

# List of packages to install
apt_packages=$(cat scripts/prerequisites-apt.txt)
echo "Required packages: $apt_packages"

missing_packages_count=0

# Check if each package is installed
for package in $apt_packages; do
    if ! dpkg -l | grep -q "^ii  $package"; then
        echo "Package $package is not installed."
        missing_packages_count=$((missing_packages_count + 1))
    else
        echo "Package $package is already installed."
    fi
done

# List of Python packages to install
pip_packages=$(cat scripts/prerequisites-pip.txt)
echo "Required Python packages: $pip_packages"

# Check if each Python package is installed
for package in $pip_packages; do
    if ! pip3 show $package > /dev/null 2>&1; then
        echo "Python package $package is not installed"
        missing_packages_count=$((missing_packages_count + 1))
    else
        echo "Python package $package is already installed."
    fi
done

if [ $missing_packages_count -gt 0 ]; then
    echo "Missing $missing_packages_count packages detected. Please run 'scripts/install-prerequisites.sh' to install them."
    echo "Current packages installed:"
    apt list --installed
    exit 1
else
    echo "All required packages are installed."
fi

# Check gcovr version
gcovr_version=$(gcovr --version | grep -oP '\d+\.\d+')
required_version=7.0

if (( $(echo "$gcovr_version < $required_version" | bc -l) )); then
    echo "gcovr version is $gcovr_version, which is lower than $required_version"
    echo "Please run 'pip3 install --upgrade gcovr' to install a newer version."
    exit 1
else
    echo "gcovr version is $gcovr_version, which meets the requirement."
fi

# Get versions of g++, gcc, and gcov
gpp_version=$(g++ -dumpversion)
gcc_version=$(gcc -dumpversion)
gcov_version=$(gcov -dumpversion | grep -oP '\d+\.\d+.\d+' | head -n 1 | cut -d'.' -f1) # only major version

# Check if g++, gcc, and gcov versions are the same
if [ "$gpp_version" != "$gcc_version" ] || [ "$gcc_version" != "$gcov_version" ]; then
    echo "Versions mismatch detected:"
    echo "g++ version: $gpp_version"
    echo "gcc version: $gcc_version"
    echo "gcov version: $gcov_version"
    echo "g++, gcc, and gcov must have the same version."
    exit 1
else
    echo "g++, gcc, and gcov versions match: $gpp_version"
fi

if [ "$skip_tests" = true ]; then
    echo "Skipping tests as --skip-tests was specified."
    elapsed_time=$(($(date +%s) - start_time))
    minutes=$((elapsed_time / 60))
    seconds=$((elapsed_time % 60))
    echo "Prerequisites checked in $minutes minutes and $seconds seconds."
    exit 0
fi

# Run tests for 32-bit build and test
echo "Running tests for 32-bit build and test..."

export NON_AMD64_BUILD=1
./tests/main.py --clean --report build test
if [ $? -eq 0 ]; then
    echo "32-bit tests passed."
    rm -rf ./tests/report-32bit
    mv ./tests/report ./tests/report-32bit
else
    echo "32-bit tests failed!"
    exit 1
fi

# Run tests for 64-bit build and test
echo "Running tests for 64-bit build and test..."

unset NON_AMD64_BUILD
./tests/main.py --clean --report build test
if [ $? -eq 0 ]; then
    echo "64-bit tests passed."
    rm -rf ./tests/report-64bit
    mv ./tests/report ./tests/report-64bit
else
    echo "64-bit tests failed!"
    exit 1
fi

new_png_files=$(git status --porcelain | grep "\?\? .*\.png")
if [ -n "$new_png_files" ]; then
    echo "New untracked PNG files detected:"
    echo "$new_png_files"
    echo "Please add them to the repository and commit them."
    exit 1
else
    echo "No new untracked PNG files detected."
fi

elapsed_time=$(($(date +%s) - start_time))
minutes=$((elapsed_time / 60))
seconds=$((elapsed_time % 60))
echo "Tests completed in $minutes minutes and $seconds seconds."

echo "See gcovr reports in:"
echo "  $PWD/tests/report-32bit/index.html"
echo "  $PWD/tests/report-64bit/index.html"

echo "LVGL all tests passed."
exit 0
