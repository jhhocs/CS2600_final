#!/bin/bash

broker="test.mosquitto.org"
startTopic="ticTacToe/gameStart"
gameStateTopic="ticTacToe/gameState"
gameStarted=0

while true
do
if(($gameStarted == 0)); then
    echo "Waiting for Player 1 (ESP32) to start game"
fi
while(($gameStarted == 0))
do
    msg=$(mosquitto_sub -h $broker -C 1 -t $startTopic)
    if ((msg == 1)); then #Makes sure a 1 is recieved
        #./TicTacToe 0
        gameStarted=1
        #fileContent=$(head -c 10 gameState.txt)
        #mosquitto_pub -h $broker -t $gameStateTopic -m "$fileContent"
        open -a Terminal ~/Documents/CS2600_final/script.sh
    fi
done

if(($gameStarted == 1)); then
    gameStarted=$(mosquitto_sub -h $broker -C 1 -t $startTopic)
fi

done