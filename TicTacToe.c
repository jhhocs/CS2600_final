#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>

char board[3][3];
char symbol[2] = {'O', 'X'};
int player;
int turn = 9;

void printBoard() {
    printf("Current Board:\n");
    for(int i = 0; i < 3; i++) {
        printf("%c | %c | %c\n", board[i][0], board[i][1], board[i][2]);
        if(i < 2) {
            printf("---------\n");
        }
    }
    printf("\n");
}

void resetGame() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }

    srand(time(NULL));
    player = rand() % 2 + 1;
    printf("Player turn: %d\n", player);
    turn = 1;
}

char checkWin() {
    for(int i = 0; i < 3; i++) {
        if(board[i][i] == ' ') {
            continue;
        }
        if(i == 0) {
            if(board[i][i] == board[i][i + 1] && board[i][i] == board[i][i + 2] || 
            board[i][i] == board[i + 1][i] && board[i][i] == board[i + 2][i]) {
                return board[i][i];
            }
        }
        if(i == 1) {
            if(board[i][i - 1] == board[i][i] && board[i][i] == board[i][i + 1] || 
            board[i - 1][i] == board[i][i] && board[i][i] == board[i + 1][i] ||
            board[i - 1][i - 1] == board[i][i] && board[i][i] == board[i + 1][i + 1]||
            board[i - 1][i + 1] == board[i][i] && board[i][i] == board[i + 1][i - 1]) {
                return board[i][i];
            }
        }
        if(i == 2) {
            if(board[i][i] == board[i][i + 1] && board[i][i] == board[i][i + 2] || 
            board[i][i] == board[i - 1][i] && board[i][i] == board[i - 2][i]) {
                return board[i][i];
            }
        }
    }
    return ' ';
}

void saveGameState() {
    FILE *file = fopen("gameState.txt", "w");
    fprintf(file, "%d", player);
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            fprintf(file, "%c", board[i][j]);
        }
    }

}

int updateBoard(int position) {
    int x, y;

    if(position < 4) {
        x = 0;
    }
    else if(position < 7) {
        x = 1;
    }
    else {
        x = 2;
    }

    if(position == 1 || position == 4 || position == 7) {
        y = 0;
    }
    else if(position == 2 || position == 5 || position == 8) {
        y = 1;
    }
    else {
        y = 2;
    }

    if(board[x][y] == ' ') {
        board[x][y] = symbol[player - 1];
        return 1;
    }
    printf("Square taken!\n");
    return -1;
}

void switchTurns() {
    if(player == 1) {
        player = 2;
    }
    else {
        player = 1;
    }
}

int main(int argc, char *argv[]) {
    FILE *file;

    player = atoi(argv[1]);

    //Check if there is an existing game. If so, open the file
    if(file = fopen("gameState.txt", "r")) {
        printf("Board found\n");
        char nextChar;

        int playerTurn;
        if(fgetc(file) == 49) {
            playerTurn = 1;
        }
        else {
            playerTurn = 2;
        }

        if(playerTurn != player) {
            printf("Player %d tried to make a move on Player %d's turn\n", player, playerTurn);
            return -1;
        }
        for(int i = 0; i < 3; i++) {
            for(int j = 0; j < 3; j++) {
                
                nextChar = fgetc(file);

                if(nextChar == "") {
                    turn -= 1;
                    nextChar = ' ';
                }
                board[i][j] = nextChar;
            }
        }
        fclose(file);
    }
    if(atoi(argv[2]) == 0) {
        printf("New game!\n");
        resetGame();
        saveGameState();
        return 0;
    }

    if (updateBoard(atoi(argv[2])) == -1) {
        return -1;
    }

    if(checkWin() != ' ') {
        return player;
    }

    switchTurns();
    
    printBoard();

    saveGameState();

    return 0;
}