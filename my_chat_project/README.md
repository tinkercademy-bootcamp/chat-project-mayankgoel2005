# My Chat Project

This is a simple command-line chat server and client. Users can connect, register a username, join or create channels, and chat with each other in real time.

## Features

1. **User Registration**
   - When you connect, first type a username (no spaces).
   - The server keeps track of who is connected.

2. **Channels**
   - Create a channel with `/create <channel_name>`.
   - List all existing channels with `/list`.
   - Join a channel with `/join <channel_name>`.
   - Leave your current channel with `/leave`.
   - See which channel you’re in with `/current`.
   - Delete an empty channel with `/delete <channel_name>`.
   - View who is in a channel with `/users <channel_name>`.

3. **Chatting**
   - Once you’ve joined a channel, type any message (press Enter) to send it.
   - Other users in the same channel see: `username: message`.
   - You will not see your own message echoed back.

4. **Private Messages**
   - Send a private note to another user with `/msg <username> <text>`.
   - If that username is online, they see: `[PM from <you>] <text>`.

5. **User List**
   - See all connected users with `/who`.

6. **Rename**
   - Change your username with `/rename <new_username>` (if it’s not already taken).

7. **Help**
   - Show a list of available commands with `/help`.

8. **Keepalive**
   - The server automatically disconnects you if you’re idle (no messages or commands) for more than 60 seconds.
   - You can also manually check if the server is still there by typing `/ping`. The server replies with `/pong`.

9. **Quit**
   - Disconnect cleanly from the server with `/quit`.

## How to Run

1. **Build the code** (in your project directory):
   ```
   make clean
   make all
   ```
   This creates two binaries: `build/server` and `build/client`.

2. **Start the server** in one terminal:
   ```
   ./build/server
   ```
   You’ll see:
   ```
   Server listening on port 8080
   ```

3. **Start a client** in another terminal:
   ```
   ./build/client
   ```
   You’ll see:
   ```
   Connected to server 127.0.0.1:8080
   First, type your username, then type commands or messages.
   ```

4. **Register a username**:
   - Type a name (for example `Alice`) and press Enter.
   - You’ll see: `Welcome, Alice`

5. **Try some commands**:
   - `/list` → shows all channels (initially empty).
   - `/create lobby` → creates a channel named “lobby”.
   - `/join lobby` → joins the “lobby” channel.
   - Now type `Hello everyone` → other users in “lobby” will see:
     ```
     Server: Alice: Hello everyone
     ```

6. **Open another client**:
   - In a new terminal, run `./build/client` again.
   - Register with another username (for example `Bob`).
   - Type `/join lobby` → you will see:
     ```
     Server: [Server] Alice has joined lobby
     Joined lobby
     ```
   - Now type `Hi Alice` → Alice’s client sees:
     ```
     Server: Bob: Hi Alice
     ```

7. **Other commands**:
   - `/leave` → leave “lobby” (no longer receive chat from that channel).
   - `/current` → tells you which channel you’re in (or that you’re in none).
   - `/delete lobby` → if “lobby” has no users, removes it.
   - `/rename Charlie` → change your username to “Charlie”.
   - `/msg Alice How are you?` → sends a private message.
   - `/who` → lists all connected users.
   - `/users lobby` → lists everyone in “lobby”.
   - `/help` → prints all commands.
   - `/quit` → disconnects from the server.
   - `/ping` → server replies with `/pong` (keeps you from timing out).

## Notes

- Channel names and usernames have no spaces.
- If you try to create an existing channel, the server tells you it already exists.
- If you try to delete a channel that still has users, the server refuses.
- The server automatically removes idle clients after 60 seconds of no activity.
- Everything happens in the terminal—no GUI needed.
