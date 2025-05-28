# Exercise 5

**Update this README with your answers to the questions below.**

## Comparing File Changes

- What's an easy way to identify the changes in source code from one exercise to another?
  - Use `diff` 

## Using Classes

- Here is one way to separate out the code into multiple files
  It makes the code easier to read and reuse. However, it can increase complexity when managing dependencies.
- Is this the best way to do it? 
  It depends on the project size. For small projects, it might be overkill, but for larger ones, it helps keep things organized.
- What are the advantages and disadvantages?  
  Advantages: Better organization, easier debugging.  
  Disadvantages: Harder to manage for small projects, more files to track.

## Introduction to Namespace

- There are different ways namespace is being used in this exercise
- Which way is better? What are the advantages and disadvantages?
  - Using nested namespaces keeps the code organized and avoids name conflicts. However, it can make the code harder to read if overused.

## Abstracting Code into Classes

- Abstract the client and server logic into a `Client` and `Server` class
  - Done!
- **Note**: You don't have to use the code in this exercise as a starting point
- You can use the code you wrote from previous exercises instead
- How should you divide the code into files?
  - I kept each class in its own header/implementation pair. For this, I used:
    - Core networking functions in net/chat-sockets.{h,cc}
    - Client class in client/client.{h,cc}
    - Server class in server/server.{h,cc}
    - Main executables in client-main.cc and server-main.cc

- What namespace and directory structure should you use? Why?
  - I used a nested namespace structure (tt::chat::client, tt::chat::server, tt::chat::net) that mirrors the directory structure (src/client, src/server, src/net). The directories group related files together.

## Programming Sense of Taste

- How do you judge what makes a particular choice of namespace and directory
  structure? 
- How do you judge what makes a good naming convention or programming style?

## "Senses" in Programming

- Have you ever heard of programmers referring to "code smells"? How can code
  smell?
- What does it mean to have a sense of taste in programming? How can code
  taste?
- Is there an analogue for every sense?
- What other code senses can you think of?
- How many senses do humans have?
- When would you want to see something before you touch it?
- When would you want to touch something before you see it?