# Exercise 3

**Update this README with your answers to the questions below.**

## Re-using Code

- Read the code in `src/tcp_echo_client.cc`
- A new function `check_error()` has been created and `create_socket()` from exercise-2 has been refactored to make use of it
- What are the benefits of writing code in this way?
  - Improves readability by abstracting error handling.
  - Reduces code duplication and ensures consistency.
  - Simplifies debugging and testing by isolating error logic.
  - Enhances maintainability by centralizing error handling.

- Are there any costs to writing code like this?
  - Slight overhead of function calls.
  - May reduce clarity for very simple error checks.
  - Requires discipline to ensure all errors are routed through `check_error`.
- Apply `check_error` to all the code in `src/`

## Introduction to Compiler Explorer

- Try out the old `create_socket()` and the new `check_error()` and 
  `create_socket()` in [Compiler Explorer](https://godbolt.org) - Interactive tool for exploring how C++ code compiles to assembly
  - The old `create_socket()` has inline error handling, leading to repetitive assembly instructions for error checks.
  - The new `check_error()` centralizes error handling, reducing redundancy in the generated assembly.
  - The new approach improves maintainability and readability without significantly impacting performance.

- What is happening here?
  - The `check_error()` function abstracts error handling, reducing redundancy and improving maintainability by centralizing error-checking logic.

- Can you think of any different approaches to this problem?
  - Use macros or inline functions for error handling to reduce function call overhead.
  - Use exceptions for error handling instead of return codes or manual checks.

- How can you modify your Makefile to generate assembly code instead of compiled code?
  - Add the `-S` flag to the compiler command in the Makefile
  - For example: `g++ -S -o output.s main.cpp`.
- **Note**: You can save the generated assembly from Compiler Explorer
- **Bonus**: Can you view assembly code using your IDE?
  - Yes, in VSCode, you can view assembly code by using the "Disassembly" view in the Debugger.
- **Bonus**: How do you see the assembly when you step through each line in debugging mode from your IDE?
  - In VSCode, start debugging, then right-click and select "Show Disassembly" to view the assembly alongside your source code as you step through each line.

## More About Memory Management

- Make sure you have `-fsanitize=address` in both your `CXX_FLAGS` and 
  `LD_FLAGS` in your Makefile
  - Done!
- What do `-fsanitize=address`, `CXX_FLAGS` and `LD_FLAGS` mean?
  - `-fsanitize=address`: Enables AddressSanitizer to detect memory errors like buffer overflows.
  - `CXX_FLAGS`: Compiler flags used during the compilation process to control warnings, optimizations, and debugging.
  - `LD_FLAGS`: Linker flags used during the linking process to specify libraries or additional options for the linker.

- With the new tool of the Compiler Explorer, and keeping in mind what you have learned about how to use debug mode
  - Compiler Explorer shows how your code turns into assembly, so you can see what the computer actually does and how the code gets optimized. Debug mode helps by letting you run your code step by step, so you can watch how variables change, check memory, and see how the program flows while it's running.

- What happens when you look at a `std::string` using the above methods?
- Where is the text in your `std::string`?
- What is `std::optional`?
- How do you find out the memory layout of a `std::optional`?
- Read https://en.cppreference.com/w/cpp/memory#Smart_pointers - Guide to 
  modern C++ memory management using smart pointers
- Which pointer types are the most important to know about?
- Which smart pointer should you use by default if you can?
- Does changing your optimization level in `CXXFLAGS` from `-O0` to `-O3` have
  any impact on the answers to any of the above questions?

## More Thinking About Performance

- After your experiments with Compiler Explorer, do you have any updates for
  your answers in exercise-2?

### Bonus: Do Not Watch Now 

- [More about Compiler Explorer](https://www.youtube.com/watch?v=bSkpMdDe4g4) - 
  Deep dive into compiler optimization and assembly analysis
  - Create a struct which contain many different data types
  - Look at the memory layout in the debugger
  - Create a `char` or `uint8_t` pointer to the beginning of the struct, 
    iterate to the end of the struct, printing out the value of each byte in 
    hexadecimal
  - Try accessing different parts of the struct and see in compiler explorer
    what the assembly looks like
  - What happens if you iterate the pointer to outside the bounds of your
    array?
  - Let's say your struct is called `Foo`
  - What is the difference between `std::vector<Foo>` and `std::vector<Foo*>`?
  - What are the tradeoffs between using `std::vector<Foo>` vs 
    `std::vector<Foo*>`? 
  - Give an example where `std::vector<Foo>` is a better choice than 
    `std::vector<Foo*>`
  - Give another example where the opposite is true
  - Can you create `std::vector<Foo&>`? 
  - Can you create `std::vector<std::optional<Foo>>`?
  - What happens if your struct contains another struct?
  - What is the difference between a struct and a class?