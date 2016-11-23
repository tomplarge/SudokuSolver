//
//  main.cpp
//  SudokuSolver
//
//  Created by Tom Large on 8/18/16.
//  Copyright © 2016 Tom Large. All rights reserved.
//

#include <iostream>
#include <string>
#include <cmath>
#include <ctime>
#include <stack>
using namespace std;

std::stack<clock_t> tictoc_stack;

void tic() {
    tictoc_stack.push(clock());
}

void toc() {
    std::cout << "Time elapsed: "
    << ((double)(clock() - tictoc_stack.top())) / CLOCKS_PER_SEC
    << std::endl;
    tictoc_stack.pop();
}

struct cell{
    string value;
    int position[2]; //(x,y) coordinates, start counting at 0, in matrix fashion [row,column]
    int peerpos[20][2];
    int squarenum;
};

struct board{
    cell* entries[9][9];
    int currcellpos[2];
};

void displayboard(board* b,int starting){ //display the board
    for (int i = 0; i < 9; i++){
        cout << endl;
        if (i == 3 || i == 6){
            cout << "------------------------------+------------------------------+------------------------------" << endl;
        }
        for (int j = 0; j < 9; j++){
            if (j == 3 || j == 6){
                cout << "|";
            }
            if (starting == 0){
                cout << (b->entries[i][j])->value;
                if ((b->entries[i][j])->value.length() < 9){
                    int spaces = 9 - (b->entries[i][j])->value.length();
                    for (int k = 0; k < spaces; k++){
                        cout << " ";
                    }
                }
                cout << " ";
            }
            else{
                if ((b->entries[i][j])->value.length() > 1){
                    cout << ".        ";
                }
                    else{
                        cout << (b->entries[i][j])->value << "        ";
                    }
                    cout << " ";
            }
        }
    }
    cout << endl;
    cout << endl;
}

void initializevalues(board* b){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            b->entries[i][j] = new cell;
            (b->entries[i][j])->position[0] = i;
            (b->entries[i][j])->position[1] = j;
            (b->entries[i][j])->value = "123456789";
        }
    }
}

void setpeers(board* b){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            (b->entries[i][j])->squarenum = (i-i%3) + (j-j%3)/3;
        }
    }
    for (int i = 0; i < 9; i++){ //populate peers for each cell
        for (int j = 0; j < 9; j++){
            int p = 0;
            for (int k = 0; k < 3; k++){
                switch(k){
                    case 0: { //search in square
                        for (int x = 0; x < 9; x++){
                            for (int y = 0; y < 9; y++){
                                if ((b->entries[x][y])->squarenum == (b->entries[i][j])->squarenum && (b->entries[x][y] != (b->entries[i][j]))){
                                    (b->entries[i][j])->peerpos[p][0] = x;
                                    (b->entries[i][j])->peerpos[p][1] = y;
                                    p++;
                                }
                            }
                        }
                        break;
                    }
                    case 1:{ //search along the row
                        for (int l = 0; l < 9; l++){ //index i,j is the entry being added to, i,l moving down the row to add to peers
                            if (b->entries[i][j] != b->entries[i][l] && (b->entries[i][l])->squarenum != (b->entries[i][j])->squarenum){
                                b->entries[i][j]->peerpos[p][0] = i;
                                b->entries[i][j]->peerpos[p][1] = l;
                                p++;
                            }
                        }
                        break;
                    }
                    case 2: {//search along the column
                        for (int l = 0; l < 9; l++){
                            if (b->entries[i][j] != b->entries[l][j] && (b->entries[l][j])->squarenum != (b->entries[i][j])->squarenum){
                                b->entries[i][j]->peerpos[p][0] = l;
                                b->entries[i][j]->peerpos[p][1] = j;
                                p++;
                            }
                        }
                        break;
                    }
                }
            }
        }
    }
}

int eliminate(cell* c, char remval){
    if (c->value.find(remval) < 10){
        c->value.erase(c->value.find(remval),1);
        return 1;
    }
    else return 0;
}

void propagate(board* b){
    int redo = 0;
    do{
        redo = 0;
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                if ((b->entries[i][j])->value.length() == 1){
                    for (int p = 0; p < 20; p++){
                        if (eliminate(b->entries[b->entries[i][j]->peerpos[p][0]][b->entries[i][j]->peerpos[p][1]],(b->entries[i][j])->value[0]) == 1){
                            redo = 1;
                        };
                    }
                }
            }
        }
    } while(redo == 1);
}

void search(board* b){ //search for the first cell with the lowest possible numbers
    int lowest = 9; //lowest keeps track of the lowest possibilities a cell has
    int lowestpos[2] = {0,0};
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            if ((b->entries[i][j])->value.length() > 1 && (b->entries[i][j])->value.length() < lowest){ //if we have found a cell that has more than 1 possibility, meaning it isn't complete, and it's number of possibilities is lower than previous lowest...
                lowest = (b->entries[i][j])->value.length(); //store this length in lowest
                lowestpos[0] = i;
                lowestpos[1] = j;
            }
        }
    }
    b->currcellpos[0] = lowestpos[0];
    b->currcellpos[1] = lowestpos[1];
}

int existscontradiction(board* b){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++) {
            for (int p = 0; p < 20; p++) {
                if ((b->entries[i][j])->value.length() == 1 && (b->entries[i][j])->value == b->entries[b->entries[i][j]->peerpos[p][0]][b->entries[i][j]->peerpos[p][1]]->value){
                    return 1;
                }
                if ((b->entries[i][j])->value.length() == 0 || (b->entries[i][j])->value == ""){
                    return 1;
                }
            }
        }
    }
    return 0;
}

string compress(board* b){
    if (existscontradiction(b) == 1){
        cout << "Error in board. Cannot compress. Should not compress. FIX IT." << endl;
        return "";
    }
    string compress = "";
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            compress += (b->entries[i][j])->value + ";";
        }
    }
    compress += to_string(b->currcellpos[0]);
    compress += to_string(b->currcellpos[1]);
    return compress;
}

void decompress(board* b, string str){
    int strcounter = 0;
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            string acc = "";
            while (str[strcounter] != ';'){
                acc += str[strcounter];
                strcounter++;
            }
            (b->entries[i][j])->value = acc;
            (b->entries[i][j])->position[0] = i;
            (b->entries[i][j])->position[1] = j;
            strcounter++;
        }
    }
    b->currcellpos[0] = str[strcounter] - '0';
    b->currcellpos[1] = str[strcounter + 1] - '0';
    setpeers(b);
}

void saveboard(board* b,string* &savedboards,int &size){
    string compressed = compress(b);
    string* biggerarray = new string[size+1];
    for (int i = 0; i < size; i++){
        biggerarray[i] = savedboards[i];
    }
    biggerarray[size] = compressed;
    delete[] savedboards;
    savedboards = biggerarray;
    size++;
}

void backtrack(board* b,string* &savedboards,int &size){
    decompress(b, savedboards[size-1]);
    string* smallerarray = new string[size-1];
    for (int i = 0; i < size-1; i++){
        smallerarray[i] = savedboards[i];
    }
    delete[] savedboards;
    savedboards = smallerarray;
    size--;
}

int issolved(board* b){
    if (existscontradiction(b) == 0){
        for (int i = 0; i < 9; i++){
            for (int j = 0; j < 9; j++){
                if (b->entries[i][j]->value.length() == 1){
                    continue;
                }
                else{
                    return 0;
                }
            }
        }
    }
    else {
        return 0;
    }
    return 1;
}

int main(){
    tic();
    board* currboard = new board;
    initializevalues(currboard);
    setpeers(currboard);
    //TEMPORARY BOARD TO DEBUG WITH*************************************************************************************************
    string testboard[9];
    testboard[0] = "123456789";
    testboard[1] = "000000000";
    testboard[2] = "000000000";
    testboard[3] = "000000000";
    testboard[4] = "000000000";
    testboard[5] = "000000000";
    testboard[6] = "000000000";
    testboard[7] = "000000000";
    testboard[8] = "000000000";

    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            if (testboard[i][j] == '0'){
                continue;
            }
            else{
                (currboard->entries[i][j])->value = testboard[i][j];
            }
        }
    }
    
    propagate(currboard);
    displayboard(currboard,1);
    search(currboard);
    cout << currboard->entries[currboard->currcellpos[0]][currboard->currcellpos[1]]->value << endl;
    if (existscontradiction(currboard) == 1){ //if there is a contradiction, the board was entered incorrectly
        cout << "Error in Board. Stop and try again." << endl;
    }
    
    int depth = 0; //keeps track of how deep in the solving tree we are
    string* prevboards = new string[depth];
    search(currboard);
    while(issolved(currboard) == 0){
        if (existscontradiction(currboard) == 1){
            backtrack(currboard, prevboards, depth);
            eliminate(currboard->entries[currboard->currcellpos[0]][currboard->currcellpos[1]],currboard->entries[currboard->currcellpos[0]][currboard->currcellpos[1]]->value[0]);
            propagate(currboard);
            continue;
        }
        else{
            search(currboard);
            saveboard(currboard, prevboards, depth);
            currboard->entries[currboard->currcellpos[0]][currboard->currcellpos[1]]->value = currboard->entries[currboard->currcellpos[0]][currboard->currcellpos[1]]->value[0];
            propagate(currboard);
            continue;
        }
    }
    displayboard(currboard,0);
    cout << "TADA!" << endl;
    
    delete[] prevboards;
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++){
            delete currboard->entries[i][j];
        }
    }
    delete currboard;
    toc();
}






