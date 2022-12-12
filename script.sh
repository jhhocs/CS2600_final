#!/bin/bash

# msg=$(mosquitto_sub -h test.mosquitto.org -C 1 -t "random_unique_test")
# echo $msg

gcc TicTacToe.c -o ticTacToe


#./TicTacToe [player #] [square 1-9]
# if square = 0, start new game

./TicTacToe 1 0


#mosquitto_pub