<img src="https://dalihub.github.io/images/DaliLogo320x200.png">

# Table of Contents

   * [Build Instructions](#build-instructions)
      * [1. Building for Ubuntu desktop](#1-building-for-ubuntu-desktop)
         * [Minimum Requirements](#minimum-requirements)
         * [Building the Repository](#building-the-repository)
         * [Building and executing test cases](#building-and-executing-test-cases)
      * [2. GBS Builds](#2-gbs-builds)
         * [NON-SMACK Targets](#non-smack-targets)
         * [SMACK enabled Targets](#smack-enabled-targets)
         * [DEBUG Builds](#debug-builds)
      * [3. Building for MS Windows](#3-building-for-ms-windows)
         * Build with the Visual Studio project.
         * Build with CMake.
      * [4. Building for MacOS](#4-building-for-macos)
   * [DALi Scene3D](#dali-scene3d)

# Build Instructions

## 1. Building for Ubuntu desktop

### Requirements

 - Ubuntu 16.04 or later
 - Environment created using dali_env script in dali-core repository
 - GCC version 9
 - Cmake version 3.8.2 or later

DALi requires a compiler supporting C++17 features.

### Building the Repository

To build the repository enter the 'build/tizen' folder:

         $ cd dali-toolkit/build/tizen

Then run the following command to set up the build:

         $ cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX .

If a Debug build is required, then add -DCMAKE_BUILD_TYPE=Debug

To build run:

         $ make install -j8

### Building and executing test cases

See the README.md in dali-toolkit/automated-tests.

## 2. GBS Builds

### NON-SMACK Targets

         $ gbs build -A [TARGET_ARCH]

### SMACK enabled Targets

         $ gbs build -A [TARGET_ARCH] --define "%enable_dali_smack_rules 1"

### DEBUG Builds

         $ gbs build -A [TARGET_ARCH] --define "%enable_debug 1"


## 3. Building for MS Windows

Third party dependencies are built using vcpkg. Instructions on how to install vcpkg can be found in the
vcpkg-script folder in the windows-dependencies repository.

- Download the windows-dependencies repository from DaliHub

         $ git clone https://github.com/dalihub/windows-dependencies.git

- Read the windows-dependencies/vcpkg-script/Readme.md file for more instructions on how to build and install the third-party dependencies.

### Build with the Visual Studio project
  Read the windows-dependencies/README.md file for more instructions on how to build and run DALi for MS Windows.

### Build with CMake

  * Requirements
    It's required the version 3.12.2 of CMake and a Git Bash Shell.

  * Notes and troubleshoting:
    It should be possible to use the MS Visual studio Developer Command Prompt (https://docs.microsoft.com/en-us/dotnet/framework/tools/developer-command-prompt-for-vs) to build DALi from the command line.
    However, the CMake version installed with MS Visual Studio 2017 is a bit out of date and some VCPKG modules require a higher version.
    This instructions have been tested with CMake 3.12.2 on a Git Bash shell.

  * Define an environment variable to set the path to the VCPKG folder

    $ export VCPKG_FOLDER=C:/Users/username/Workspace/VCPKG_TOOL

  * Define an environment variable to set the path where DALi is going to be installed.

    $ export DALI_ENV_FOLDER=C:/Users/username/Workspace/dali-env

  * Execute the following commands to create the makefiles, build and install DALi.

    $ cmake -g Ninja . -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/vcpkg/scripts/buildsystems/vcpkg.cmake -DENABLE_PKG_CONFIGURE=OFF -DENABLE_LINK_TEST=OFF -DCMAKE_INSTALL_PREFIX=$DALI_ENV_FOLDER -DINSTALL_CMAKE_MODULES=ON -DUSE_DEFAULT_RESOURCE_DIR=OFF
    $ cmake --build . --target install


  * Options:
    - CMAKE_TOOLCHAIN_FILE     ---> Needed to find packages installed by VCPKG.
    - ENABLE_PKG_CONFIGURE     ---> Whether to install pkg configure files (not currently working on MS Windows. CMake modules used instead).
    - ENABLE_LINK_TEST         ---> Whether to enable the link test (not currently working on MS Windows).
    - CMAKE_INSTALL_PREFIX     ---> Were DALi is installed.
    - INSTALL_CMAKE_MODULES    ---> Whether to install the CMake modules (Used by the CMake command find_package() to find previously installed libraries).
    - ENABLE_DEBUG             ---> Whether to build with debug enabled.
    - USE_DEFAULT_RESOURCE_DIR ---> Whether to use the default resource folders. Otherwise set environment variables for DALI_IMAGE_DIR, DALI_SOUND_DIR, DALI_STYLE_DIR, DALI_STYLE_IMAGE_DIR and DALI_DATA_READ_ONLY_DIR

## 4. Building for MacOS

It is assumed that the DALi environment has been set up & both DALi Core & Adaptor have been built accordingly.

To build the repository enter the 'build/tizen' folder:
```zsh
% cd dali-toolkit/build/tizen
```
Then run the following command to set up the build:
```zsh
% cmake -DCMAKE_INSTALL_PREFIX=$DESKTOP_PREFIX -DCMAKE_TOOLCHAIN_FILE=$VCPKG_FOLDER/scripts/buildsystems/vcpkg.cmake -DINSTALL_CMAKE_MODULES=ON
```
If a Debug build is required, then add `-DCMAKE_BUILD_TYPE=Debug -DENABLE_DEBUG=ON`

To build, run:
```zsh
% make install -j8
```

# DALi Scene3D

For information about the DALi Scene3D library, refer to dali-scene3d/README.md.
