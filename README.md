# COSC 77/177 Computer Graphics Starter Code Manual

## 0. Quick Guide

If you are already experienced with using CMake to generate a C++ project, please read the following paragraphs for a quick guide. If not, you may read Section 1 and 2 first and then get back to check this section as a summary.

### Code Structure

The starter codebase is organized as `ext`, `src`, `assignments` and `tutorials`. We put all external codes (e.g., the Eigen library) in `ext`. We put the common headers that can be shared among different subprojects in `src` (e.g., the classes of mesh, grid, file IO, etc.). The folder `assignments` maintains all assignments. An assignment is dependent on `src` and `ext`, but is independent from other assignments.

Usually, you are asked to write code in one or multiple files in an assignment folder (e.g., in `assignments/a1/LoopSubdivision.h`). You don’t need to change files in ext or src.

### Build and Compile

We use CMake to separate the source code and the build files. `CMakeLists.txt` is maintained in each subproject. 
To generate build files (e.g., an .sln file for Windows or a makefile for Linux), you need to 1) create a `build` folder to hold all subprojects; 
2) use CMake to generate the build files (i.e., run `cmake <path to source>` in the build folder).

### Executable

The executables are generated in the Release or Debug folder under the subdirectory (e.g., `build/a1/Release`) for Windows and just in e.g. `build/a1` for Linux/Mac.


## 1. Compile and run the first assignment:

### Step 1: Clone the source code from GitLab and enter the codebase folder

    git clone https://gitlab.com/boolzhu/dartmouth-cg-starter-code
    cd dartmouth-cg-starter-code

### Step 2: Build the project using CMake:

**Substep 2.0: Install CMake**

- **Windows**: Visit https://cmake.org/download/ and download the latest version.
- **Linux**: `sudo apt-get install cmake`
- **OSX**: `brew install cmake` (using [Homebrew](https://brew.sh/))

**Substep 2.1: Install Dependencies (Linux/OSX only)**
- **Linux**: `sudo apt-get install glew freeglut`
- **OSX**: `brew install glew freeglut`

**Substep 2.2: Run setup.sh/bat**

    .\scripts\setup.bat [Windows]
    ./scripts/setup.sh [Linux/OSX]

### Step 3: Compile and run the code

To test if everything works correctly you can run:

    .\scripts\run_assignment.bat a1 1 [Windows]
    ./scripts/run_assignment.sh a1 1 [Linux/Mac]

This will:

- Compile the code for the assignment 1 and any dependencies
- Run the assignment and generate simulation data for the given test (`1` is the rod simulation for assignment 1)
- Open the viewer load the data that was just generated
	- In the viewer you can press `w` to start recording and `p` to play back the animation

### Step 4: Start implementing!

- **Windows**: Open the .sln project file in the `build` folder.
- **Linux or OSX**: Use your editor of choice to edit the assignment files in `assignments`

## 3. Command line details (optional) 

While the `setup` and `run_assignment` scripts should get you up and running quickly, you might want to peek behind the curtains and run the individual parts manually.

You should familiarize yourself with CMake, which is a tool that allows you to describe your build steps in a platform independent way.


### Setup

During setup, the script creates a directory called `build` in your top level project folder. 
This is standard CMake practice and makes sure that we are not polluting our source code with platform specific build files.

The script enters the directory and executes CMake using `cmake ..`. CMake finds the `CMakeLists.txt' file in the project folder and executes the build setup steps that we configured.

After this runs, you will have a bunch of files and folders in the `build` folder. You might notice that the folder structure mirrors the one in the source folder.

Generally, you can find the build files related to "assignment/aN" in "**build**/assignment/aN". 

### Build

The build step can be done through cmake as well. In the `build` folder, run 

    cmake --build . --config Release
    
This will build all assignments and examples.
    
For more details, you can look at the `setup` and `run_assignment` scripts!
