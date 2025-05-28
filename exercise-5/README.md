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

- How do you judge what makes a particular choice of namespace and directory structure? 
  - I judge namespace/directory structures by how well they support the natural divisions in the codebase. Good structures make it obvious where to find things, reduce the potential for name collisions, and scale well as the project grows. They should match the mental model of the system and isolate components that change together.

- How do you judge what makes a good naming convention or programming style?
  - Good naming and style are about consistency and clarity. I look for names that express intent and avoid abbreviations.
  - Style should reduce mental load. I should be easily able to understand what my code does just by reading it.

## "Senses" in Programming

- Have you ever heard of programmers referring to "code smells"? How can code smell?
  - Yes, "code smells" are patterns that suggest deeper issues in code. Code "smells" when it has signs of poor design, like duplicate and overly complicated code.

- What does it mean to have a sense of taste in programming? How can code taste?
  - Having taste in programming means recognizing elegant solutions. Code "tastes good" when it's clean and intuitive.

- Is there an analogue for every sense?
  - Most senses have programming analogues. We "see" code structure, "feel" its complexity, code can "smell" bad, have good or bad "taste".

- What other code senses can you think of?
  - Balance - when different components have appropriate responsibilities
  - Weight - sensing when code is too complex for its purpose

- How many senses do humans have?
  - Beyond the basic five, we have temperature, pain etc.

- When would you want to see something before you touch it?
  - I'd want to see code before modifying it - understanding its structure before diving in.

- When would you want to touch something before you see it?
  - Sometimes we experiment with code to understand a problem before designing the full solution