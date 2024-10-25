#include <iostream>
#include <string>
#include "game.h"

int main(){
    int row=3;
    int col=3;
    int len_win_line=3;
    std::string playername_1="1";
    std::string playername_2="2";
    std::cout<<"Input count rows, cols, len win line, player 1 name, player 2 name"<<std::endl;
    std::cin >> row >> col >> len_win_line >> playername_1 >> playername_2;
    Tic_tac_toe mainest(row, col, len_win_line, playername_1, playername_2);
    mainest.game();
    return 0;
}