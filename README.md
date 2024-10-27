# Trick Or Treat

Welcome to the **Trick or Treat Game**! This is a fun gamewhere you play as either the Host or the Trick or Treater. The Host needs to figure out who the real human being is and give them the poisonous treat, while the Trick or Treater tries to convince the Host that they are a zombie and avoid getting the poisonous treat.

## Features
- **Multiplayer Game:** Play with one other person in a fun and interactive setting.
- **AI Integration:** Interact with an AI bot that adds an extra layer of fun and challenge.
- **Halloween Theme:** Enjoy a spooky Halloween-themed game with colorful terminal output.

## How to Play
1. **Setup the Server:**
   - Compile and run the server code to start the game server.
   - The server will wait for players to connect.

2. **Connect Clients:**
   - Compile and run the client code for each player.
   - Enter your username when prompted.

3. **Gameplay:**
   - The Host asks questions to the Trick or Treater and the AI bot.
   - The Trick or Treater and the AI bot respond to the questions.
   - After a series of questions, the Host decides who gets the poisonous treat by entering `GUESS 1` or `GUESS 2`.

4. **Winning the Game:**
   - If the Host correctly identifies the real human by giving them the candy, the Host wins.
   - If the Host fails to identify the real human and gives the caady to the AI BOT, the Trick or Treater wins.



### Dependencies
- **CURL:** For making HTTP requests.
- **cJSON:** For parsing JSON data.
- **pthread:** For handling threads.

### Steps to Install Dependencies

1. **Install CURL**
   ```bash
   sudo apt-get install libcurl4-openssl-dev

2. **Install cJSON:**
   ```bash
   sudo apt-get install libcjson-dev

3. **Install pthread:**
   ```bash
   sudo apt-get install libpthread-stubs0-dev
### Commands for compiling:
```bash
   gcc -c socket_lib.c -o socket_lib.o
   gcc -o server server.c socket_lib.o -lpthread -lcurl -lcjson
   gcc -o client client.c socket_lib.o -lpthread




