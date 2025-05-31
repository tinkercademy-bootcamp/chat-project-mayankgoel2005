# Exercise 7

**Update this README with your answers to the questions below.**


## Resources for Learning About EPOLL

- https://suchprogramming.com/epoll-in-3-easy-steps/ 
- https://github.com/onestraw/epoll-example/
- https://github.com/joakimthun/io-uring-echo-server
- https://man7.org/linux/man-pages/man7/epoll.7.html
- What other resources can you find about epoll?
  - "The Linux Programming Interface" book has a great chapter on epoll
  - https://devarea.com/linux-io-multiplexing-select-vs-poll-vs-epoll/
  - https://medium.com/@copyconstruct/the-method-to-epolls-madness-d9d2d6378642
  - Stackoverflow's epoll examples and discussions
- Are the above code repos examples of good code or bad code?
  - `onestraw/epoll-example` is simple, but it doesn’t have much error handling or comments to explain what’s going on.
  - `joakimthun/io-uring-echo-server` is better structured and shows how to use `io_uring`, but it mixes modern C++ with old-style C code.  
  - Neither repo has proper testing or documentation, so they require a lot of work for using in a proper project. They useful for understanding the concepts, but not something I can directly copy for proper use.
- How do you decide what is good code or bad code?
  - Good code is easy to read, has comments, handles errors, has proper documentation, and is organized. 
  - Bad code is hard to follow, messy, or missing important checks.

## Creating a Chat Client/Server Using EPOLL

- Use the above resources and what you have learned to start a brand new C++ 
  project
- The server should be able to accept multiple connections at a time, as many
  as you can, to chat with each other
- Each client should be able to communicate with the server in some way so 
  that each client can be assigned a **username**
- Each client can send at least the following commands:
  - List channels
  - Go to channel
  - Create channel
- Whenever a user types in a message, it should be displayed to everyone else 
  in the channel
- You are free to come up with any other commands that you like
- You can use any directory structure or any makefile that you like
- You can use any 3rd party open source library that you like
- However for the purposes of this bootcamp, please stay with make and bash 
  scripts
- `cmake`, `ninja`, `conan` and `vcpkg` are all useful tools but are out of 
  scope for this bootcamp
- Some libraries that are likely to be useful:
- `sudo apt get ncurses-dev`
- For the more ambitious: `sudo apt get libsdl2-dev`

## Adding a Third Party Library from Source

- Not all useful open source libraries will be available as a debian package
- For the purposes of this workshop, your only options are to install a 
  shared library into `usr/include` and `usr/lib`, or to git clone
  the source code and build it
- What are the tradeoffs between using a library from `usr/lib` vs git cloning
  the source and using that instead?
- What is the difference between `-l` and `-L` when linking using `g++`?