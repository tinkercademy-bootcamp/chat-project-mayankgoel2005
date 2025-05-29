# Exercise 6

**Update this README with your answers to the questions below.**

## Adding Third Party Libraries or Source Code to Your Project

- There is a new file `src/spdlog-hello-world-main.cc`
- It uses the spdlog and fmt libraries. 
  [More info here](https://github.com/gabime/spdlog) - Fast C++ logging 
  library with various sink types and formatting options
- How do you compile this file using the `g++` CLI?
  - After installing the spdlog library, run the following command:
  ```
  g++ -std=c++23 -Wall -Wextra -pedantic -I/usr/local/include src/spdlog-hello-world-main.cc -o spdlog-hello-world -lfmt
  ```
- What do you need to change in your makefile to use this library?
  - Add `-I/usr/local/include` to CPPFLAGS
  - Add `-lfmt` to LDFLAGS
  - Create a build target for the spdlog-hello-world program
- How many different ways can this library be added into your project?
  - apt, brew (System Package Managers)
  - Copy source files directly into project
  - Link with pre-built library files
- What are the tradeoffs in the different ways?
  - System packages: Easy to install but might not have the latest version
  - Source files in project: No external dependencies but increases project size
  - Pre-built libraries: Fast to compile but might have compatibility issues
- Why are there so many different ways to do it?
  - C++ has no official package manager
  - Different projects have different needs
  - Some methods work better in certain environments
  
## Static Linking vs Dynamic Linking

- What are the differences between static linking and dynamic linking?
- What are the tradeoffs?
- How do you enable static linking or dynamic linking in your makefile?

## Git Clone and Building from Source

- Where is `g++` looking for the include files and library files?
- How do you find out?

## C++ Package Managers

- Why are there so many C++ package managers?
- Which one is the best one?
- Which are the most notable ways to manage the compilation of C++ projects?