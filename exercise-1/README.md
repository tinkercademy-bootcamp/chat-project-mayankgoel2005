# Exercise 1

**Update this README with your answers to the questions below.**

## Learning How to Learn

- Answer the following questions in this file and commit and push your changes.
- Bonus sections are more difficult and optional.
- How can you find the information required to complete these tasks?
- How can you tell if the source of your information is good?
- How would you define "good" in this situation?

## Learn Basics of g++ CLI

- Compile the TCP client and server using `g++` from command line.
- What are the most important command line arguments to learn for `g++`?
  - -o <OutputFile> : Specifies the name of the output file
  - -c : Compiles our files without linking
  - -Wall : Enables all compiler warnings.
  - -Wextra : Enables some additional warnings
  - -O<n> : Sets the optimisation level to n
  - -std=<standard> : Specifies the C++ standard
- What is the difference between debug vs release versions?
  - Debug Version :
    - Includes debugging info
    - Has no optimisations
    - Has additional checks to find bugs
    - It is slower and larger, but better for debugging
  - Release Version :
    - It is optimised wrt size and speed
    - It has no debugging info and runtime checks
    - It is faster and smaller, but harder to debug
- What are the tradeoffs between debug and release versions?
  - Answered above
- What arguments would you use in a debug build?
  - -g
  - -Wall , -Wextra
  - DDEBUG
- What about for release?
  - O3
  - -Wno
  - DNDEBUG
- What other kinds of build types are useful?
  - Testing build : For unit testing
  - Coverage build : Use --coverage to generate coverage report
  - Profile Build : For analysing performance (function calls, etc)

## Learn Basics of Make

- Create a Makefile that will speed up the process.
- [Quickstart tutorial to make](https://makefiletutorial.com/) - Learn make 
  fundamentals with practical examples and common patterns.
- How else can you learn about make?
- How can you tell if the resource you are using is correct?
- Create a makefile such that when you run `make` with no arguments, it will:
  - Create `build/` directory if it does not exist
  - Create executables **client** and **server** in `build/`, if needed
  - How does make know when it needs to rebuild the executables?
  - Change your Makefile such that `make clean` will remove `build/` and all
    its contents
- What are the most important command line arguments to learn for make?
  - -n : Prints commands that would be executed without actually running them
  - -C \<dir\> : Changes to the specified directory before running make
  - -B : Forces a rebuild of all targets
- What are the most important directives to learn about in Makefile?
  - .PHONY : Declares the targets that are not actual files
  - all : Common default target that builds everything
  - clean : Target to remove current builds
- What are the most important commands to implement in your Makefile?
  - all : Common default target that builds everything
  - clean : Target to remove current builds
  - test : Runs unit tests
  - debug : Builds project with debug flags
- Which ones are essential, which ones are nice to haves?
  - Essential : all, clean
  - Debug : test, debug, release

## Learn Basics of Git

- Read through the code in `src/`
- Answer any `#Questions` as a comment
  - Done!
- Commit and push your changes to git
  - Done!
- Each commit should be responding to a single task or question
- Why is it important to keep your commit to a single task or question?
  - Easier to understand what I changed in that particular commit
- Is it better to have a lot of very small commits, or one big commit when 
  everything is working?
  - Better to have multiple small commits, so we can later underatnd the purpose each small change, and revert individual changes if needed
- What are the most important commands to know in git?
  - init : Initialise a rew repo
  - clone : Clone existing repo
  - add <file> : Stage changes to local for next commit
  - commit -m "message" : Coomits staged changes as a new node to the local
  - push : Push local changes to a remote repo
  - status : Shows status of working dir and staging area
  - checkout <branch> : Switch to different branch
  - merge <branch> : Merge branch into current branch
  - pull : Fetch and integrate changes from remote
  - diff : Show changes between commits/branches
  - stash : Temporarily stash changes

## Introduction to Sockets

- Read the code in `src/tcp-echo-client.cc` and add a way to change the 
  message sent using command line arguments
  - Done!
- **Example**: `./client "hello message from the command prompt"` should send
  `"hello message from the command prompt"` to the server
- Commit your changes into git
- What do all these headers do?
  - `<arpa/inet.h>`: IP address conversion functions
  - `<netinet/in.h>`: Internet address structures
  - `<sys/socket.h>`: Socket functions/types
  - `<sys/types.h>`: Basic system data types.
  - `<unistd.h>`: POSIX functions

- How do you find out which part of the below code comes from which header?
  - Check the documentation or man pages for each function/type

- How do you change the code so that you are sending messages to servers
  other than localhost?
  - Change the server address in the client code from `"127.0.0.1"` to the IP address of the target server

- How do you change the code to send to a IPv6 address instead of IPv4?
  - Use `sockaddr_in6` instead of `sockaddr_in`, set `AF_INET6` as the address family, and use an IPv6 address

- **Bonus**: How do you change the client code to connect by hostname instead of IP address?
  - Use `getaddrinfo()` to resolve the hostname to an IP address, then use the returned address info to connect instead of hardcoding the IP address.
  
## Introduction to Memory Management

- What is happening in line 26 of `tcp-echo-client.cc`? 
  `if (inet_pton(AF_INET, kServerAddress.c_str(), &address.sin_addr) <= 0) {`
  - The code is converting the server's IPv4 address from string format to binary format using `inet_pton`

- What is happening in line 31 of `tcp-echo-client.cc`?
  `if (connect(my_sock, (sockaddr *)&address, sizeof(address)) < 0) {`
  - The code is trying to connect the client socket to the server using the address info. If `connect` returns a value less than 0, the connection failed.

- What is the difference between a pointer and a reference?
  - A pointer holds the memory address of another variable and can be reassigned or set to null.
  - A reference is an alias for another variable, must be initialized when declared, cannot be changed to refer to something else or be null.
- When is it better to use a pointer?
  - When we need to allow null values, dynamic memory allocation, or want to change what the pointer points to at runtime
- When is it better to use a reference?
  - When you always want to refer to an existing variable, do not need null values, and do not need to change what is referenced
- What is the difference between `std::string` and a C-style string?
  - `std::string` is a C++ class that manages strings with built-in functions and automatic memory management.
  - A C-style string is a null-terminated array of characters (`char*` or `char[]`) in C/C++.
- What type is a C-style string?
  - `char*` (pointer to char) OR `char[]` (array of chars)
- What happens when you iterate a pointer?
  - When you iterate a pointer, it moves to the next memory location based on the size of the data type it points to
- What are the most important safety tips to know when using pointers?
  - Initialize pointers before use.  
  - Avoid dangling pointers by ensuring valid memory.  
  - Free dynamically allocated memory to prevent leaks.  
  - Be cautious with pointer arithmetic to avoid invalid accesses.

## Learn Basics of Creating a C++ Project in Your IDE

- How do you compile and run your project in your IDE?
  - In VS Code, use the built-in terminal to compile with `g++`

## Improving Interactions with LLMs

- What is the most authoritative source of information about `socket()`
  from `<sys/socket.h>`?
- What is the most authoritative source of information about the TCP and IP protocols?
  - POSIX man pages or the official POSIX specification.

- What is the most authoritative source of information about the C++
  programming language?
  - `https://cppreference.com/`
- What information can you find about using Markdown when structuring prompts to LLMs?
  - Markdown helps structure prompts for LLMs by using headings, lists, and code blocks to organize information, improve readability and ensure clarity.

- What is the difference between LLM and AI?
  - LLM is a specific type of AI designed for NLP tasks, such as text generation and understanding.  
  - AI is a broader field that includes various technologies, including ML, robotics, etc.
- Is it grammatically correct in English to say "a LLM" or "an LLM"? Why?
  - "An LLM", because LLM starts with a vowel sound