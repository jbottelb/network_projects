# Competitive Wordle

Josh Bottelberghe, Joseph Sweilem

27 April 2022

CSE 30264

Professor Striegel

# Overview

The project implements our objectified version of Wordle with multiple players competing to score the best of their peers. There are separate directories for running the client and the server. All additional header files are in the top level directory.

The Messages that are supported for communication as specified are:
- Join
- JoinResult
- Chat
- StartInstance
- JoinInstance
- JoinInstanceResult
- StartGame
- StartRound
- PromptForGuess
- Guess
- GuessResponse
- GuessResult
- EndRound
- EndGame

## JSON Library

We decided to use the library called cJSON for handling our JSON objects on the server side. The makefile included takes care of compiling the library (which is one .h file) for the use of the functions and creates the executable for the server.

## Client

Note: All commands ARE case sensitive

The client supports the following arguments:
- name X -> Name of the player
- server X -> IP address or hostname of the server
- port X -> Port number for the lobby
- nonce X -> Nonce to use for joining the game

To invoke the client, the user joins the server using a Join request.

~~~
./client.py Join name NAME server SERVER port PORT
~~~

Once they successfully join the waiting lobby, they will be able to chat by entering any text and hitting enter/return until the server has accumulated enough players to begin the game.

Once the server has enough players to begin, it will send the game port and nonce required for the client to join. The client will be able to exit the waiting lobby by hitting enter/return as prompted.

The client then joins the game using a JoinInstance request.

~~~
./client.py JoinInstance name NAME server SERVER port PORT nonce NONCE
~~~

The server will then accumulate all of the players and proceed to play the game. The players CANNOT chat with each other during the round up phase and can only begin to do so once the server has sent a StartGame message.

Now the game will begin and the players will all be prompted for their guess of the word. The player can submit a guess using a Guess request.

~~~
Guess GUESS
~~~

To which the server will respond with the status of their guess while it gathers all guesses. Before or after guessing, the player can also chat with other players by sending a Chat request.

~~~
Chat
~~~

To which the user will prompted for their chat. After typing and hitting enter/return, it will send the chat to all other players. 

After all guesses are sent in, the server checks all guesses and sends a recap of the round to all the players including if there was a winner and what each players score is. Then a new round will start and the players will repeat the guessing/chatting process.

The game ends when a winner is declared or when there are no more rounds. Winner is determined by score for correct lettering/guesses. There can be ties.

The server will then end the game.

## Server

Note: All commands ARE case sensitive

The server supports the following arguments:
- np X -> Number of players (default is 2)
- lp X -> Lobby port (default is 42000)
- pp X -> The starting number for play ports to use (default is 43000)
- nr X -> Number of rounds (default is 3)
- d DFile -> Dictionary of valid words where the words are listed on a line by line basis (default is word_list.txt)

To invoke the client, the user simply includes whichever arguments they would like to alter.

~~~
./server -np PLAYERS -lp PORT -pp GAMEPORT -nr ROUNDS -d FILE
~~~

or run it with all defaults.

~~~
./server
~~~

Once running, the server produces helpful commentary on the state of each game or lobby that it is running.

## Extensions

For our extensions we added:
- Filtering of Chat using bad_words.txt
- Sophisticated scoring to determine the victor
- Game messages from the server to make a more user friendly experience