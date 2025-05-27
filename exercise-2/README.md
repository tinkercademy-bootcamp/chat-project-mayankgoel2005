# Exercise 2

**Update this README with your answers to the questions below.**

## Sources of Information for Questions from Before

### Socket 
- https://man7.org/linux/man-pages/man2/socket.2.html - System call reference
  for creating communication endpoints
- Or type `man socket` in terminal
- https://man7.org/linux/man-pages/man7/socket.7.html - Socket interface 
  overview and protocol families
- Or type `man 7 socket` in terminal
- When would you want to use a `SOCK_RAW` stream?
  - When you need direct access to lower-level network protocols, bypassing the standard transport layers like TCP and UDP.

### TCP and IP Protocols
- [IPv4](https://www.rfc-editor.org/info/rfc791) - Internet Protocol 
  specification defining packet structure and routing
- [IPv6](https://www.rfc-editor.org/info/rfc8200) - Next-generation Internet 
  Protocol with expanded address space
- [TCP](https://datatracker.ietf.org/doc/html/rfc9293) - Transmission Control 
  Protocol providing reliable, ordered data delivery
    
### C++
- [C++23 ISO standard draft](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/n4950.pdf) - 
  Working draft of the C++ language specification
- Is the above the official C++23 spec? 
  - No, the above is not the official C++23 spec
- Where is the official C++23 spec?
  - The official C++23 spec is published by ISO and is available on their website: [ISO C++23 Specification](https://www.iso.org/standard/79358.html).
- Why was this link chosen instead?
  - Because it provides free access to the working draft of the C++23 specification
- Is this a helpful reference for learning C++?
  - Yes, it is helpful for understanding features/specifications of the language. However, it may not be user-friendly for beginners.
- Can the various implementations of C++ compilers be different from the C++ standard?
  - Yes, C++ compilers can have differences due to extensions, optimizations, or incomplete support for the latest standard

- What are the most widely used and most significant C++ compilers?
  - G++ : Open-source and widely used on Linux
  - Clang: Known for its modular design

- Where is the equivalent spec for C++26?
  - It is currently under development and can be found in the working drafts published by the ISO C++ committee. These drafts are available on the [ISO C++ Committee Papers](https://www.open-std.org/jtc1/sc22/wg21/) website.

- Where do you find the spec for the HTTP protocol?
  - In the RFC documents published by the IETF
- What about HTTPS? Is there a spec for that protocol?
  - Yes, HTTPS is specified as HTTP over TLS. The HTTPS specifications can be found in the [RFC 9110](https://www.rfc-editor.org/info/rfc9110)

## Introduction to C++ and Sockets Programming

- Read the code in `src/`
- Are there any bugs in this code? 
  - Yes, `tcp_echo_client.cc` should have `message.size()+1` in line 49
- What can you do to identify if there are bugs in the code?
  - Run gdb or the debugger on my IDE

## Refactoring: Extract Function

- What is different in this code compared to exercise-1?
  - This code is more modular
- Is this code better or worse than exercise-1?
  - This code is easier to read and debug
- What are the tradeoffs compared to exercise-1?
  - I think that would be that number of lines of code is larger, though I wouldnt really count it as a bad thing. Also, I would need to jump through multiple functions to get an idea of what's happening
- Are you able to spot any mistakes or inconsistencies in the changes?
  - Not that I could find
  
## Thinking About Performance

- Does writing code this way have any impact on performance?
- What do we mean when we say performance?
- How do we measure performance in a program?

## Play with Git

- There isn't necessarily a single correct answer for how to abstract the 
  code from exercise-1 into functions
- Try different ways to refactor the code from exercise-1 to make it more
  readable.
- Make sure to commit each change as small and self-contained commit
- This will make it easier to revert your code if you need to
- What is `git tag`? How is `git tag` different from `git branch`?
- How can you use `git tag` and `git branch` to make programming easier and
  more fun?

## Learn Basics of Debugging in Your IDE

- How do you enable debug mode in your IDE?
- In debug mode, how do you add a watch?
- In debug mode, how do you add a breakpoint?
- In debug mode, how do you step through code?

### Memory Management and Debug Mode in Your IDE

- How do you see the memory layout of a `std::string` from your IDE debug mode?
- How do you see the memory layout of a struct from your IDE debug mode?