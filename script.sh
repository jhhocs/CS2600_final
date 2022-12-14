#!/bin/bash

broker="test.mosquitto.org"
startTopic="ticTacToe/gameStart"
gameStateTopic="ticTacToe/gameState"
player1Move="ticTacToe/p1Move"
gameStarted=1
output=-1
autoplay=0

cd ~/Documents/CS2600_final #Opens directory where TicTacToe.c is located
                            #Daemon process opens a new terminal which runs this script.

gcc TicTacToe.c -o ticTacToe

./TicTacToe 0
fileContent=$(head -c 10 gameState.txt)
mosquitto_pub -h $broker -t $gameStateTopic -m "$fileContent"

while((gameStarted == 1))
do

#Get player turn
player=$(head -c 1 gameState.txt)
validMove=-1
unset position

if(($player == 2)); then
    while((validMove == -1))
    do
        if((autoplay == 0)); then
            read -p "Enter board position [1 - 9]: " -t 10 position #Read user input, Times out if no input is provided after 10 seconds
        fi

        if [[ $position -eq 0 || autoplay == 1 ]]; then
            if((autoplay == 0)); then
                echo "No move made, automatically making moves for the remainder of the game"
                autoplay=1
            fi
            while((validMove == -1))
            do
                position=$(( ( RANDOM % 9 )  + 1 ))
                ./ticTacToe $player $position
                output=$?
                if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                    validMove=1
                fi
            done
                
        elif (( $position < 1 || $position > 9 )); then
            echo "Invalid position"
        else
            ./ticTacToe $player $position
            output=$?
            if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                validMove=1
            fi
        fi
    done
elif(($player == 1)); then
    echo "Waiting for player 1 to make a move..."
    while((validMove == -1))
    do
        position=$(mosquitto_sub -h $broker -C 1 -t $player1Move)
        if(($position > 0 || $position < 10)); then
            ./ticTacToe $player $position
            output=$?
            if(($output == 0 || $output == 1 || $output == 2 || $output == 3)); then
                validMove=1
            fi
        fi
    done
fi

#Send game data to esp32
fileContent=$(head -c 10 gameState.txt)
mosquitto_pub -h $broker -t $gameStateTopic -m "$fileContent"

if(($output == 1 || $output == 2 || $output == 3)); then
    gameStarted=0
    msg=$(mosquitto_pub -h $broker -t $startTopic -m $gameStarted)
fi

done