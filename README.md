# Trick Or Treat

Welcome to the **Trick or Treat Game**! This is a fun gamewhere you play as either the Host or the Trick or Treater. The Host needs to figure out who the real human being is and give them the poisonous treat, while the Trick or Treater tries to convince the Host that they are a zombie and avoid getting the poisonous treat.

## Features
- **Multiplayer Game using Socket Programming:** Play with another person on a local network with the help of sockets.
- **AI Integration:** Interact with an AI model that was trained to mimic a zombie.
- **Halloween Theme:** The entire game revolves around the idea of conversing and trying to mimic a zombie.

## How to Play
1. **Setup the Server:**
   - Compile and run the server code to start the game server.
   - Once the server is started on a machine. The two clients wishing to connect to the server need to know the IP address of the server.
   - The server will wait for players to connect.
2. **Connect Clients:**
   - Compile and run the client code for each player.
   - Enter your username when prompted.

3. **Gameplay:**
   - The Host asks questions to the Trick or Treater and the AI bot.
   - The Trick or Treater and the AI bot respond to the questions.
   - After a series of questions, the Host decides who gets the poisonous treat by entering `GUESS 1` or `GUESS 2`.

4. **Game Strategy:**
   - If the Host correctly identifies the real human by giving them the candy, the Host wins.
   - If the Host fails to identify the real human and gives the candy to the AI BOT, the Trick or Treater wins.
   - The goal of the Trick or Treater is to make themselves sound more like a zombie hence making it harder for the host to identify the AI Bot from the human.
   - The Trick or Treater gets to see the previous responses of the AI BOT giving them an oppurtunity to mimic the AI by predicting its speech patterns.
   - The Host gets a maximum of 10 questions to diffrentiate the human response from the AI Bot's response. The Host is allowed to guess the identity of the human at any point of the game.



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




