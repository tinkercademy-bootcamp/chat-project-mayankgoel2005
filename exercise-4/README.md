# Exercise 4

**Update this README with your answers to the questions below.**

## Learning Multi-File C++ Projects

- Move code that can be shared between `tcp_echo_server.cc` and 
  `tcp_echo_client.cc` to separate `.h` and `.cc` files
  - Done!
- How would you compile from the command line?
  - For the server:
    ```bash
    g++ -Wall -Wextra -std=c++20 -g -O3 src/tcp_echo_server.cpp src/common.cpp -o build/server
    ```
  - For the client:
    ```bash
    g++ -Wall -Wextra -std=c++20 -g -O3 src/tcp_echo_client.cpp src/common.cpp -o build/client
    ```
- How would you compile using make?
  - Run the following command:
    ```bash
    make
    ```
  - This will build both the server and client binaries in the `build/` directory.
- How would you compile using VS Code?
  - Use the `tasks.json` file in the `.vscode` folder to define build tasks.
  - Press `Cmd+Shift+B` to run the default task.

### Compiling vs Linking

- What is the difference between compiling and linking in C++?
  - Compiling: turns code into object files, checks for errors.
  - Linking: combines object files, creates executable.

- What is the difference between creating an executable and creating a library?
  - Executable: runs directly, has `main()`.
  - Library: reusable code, no `main()`.

- How do you compile a library, and then use that library to compile an executable?
  - Compile library: `g++ -c src/common.cpp -o build/common.o`
\  - Use library: `g++ src/tcp_echo_server.cpp -Lbuild -lcommon -o build/server`

### Reminder 
[Quickstart tutorial to make](https://makefiletutorial.com/) - Learn make 
fundamentals with practical examples and common patterns.

## Experiments in Performance

- Is it better to have many small files spread across many directories, or
  a few large files across fewer directories?
- Is there a difference in compilation time between the two?
- How can you assess performance other than compilation speed?