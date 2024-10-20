#include "game.h"

int Tic_tac_toe::count_char_on_line_(int row, int col, char search, int row_change, int col_change){
    int count=0;
    bool end=false;
    for(int i=row; !end && i>=0 && i<=row; i+=row_change){
        for(int j=col; !end && j>=0 && j<=col; j+=col_change){
            if(game_space_[i][j]==search){
                ++count;
            } else {
                end=true;
            }
        }
    }
    return count;
}

bool Tic_tac_toe::check_win(int row, int col, char win){
    int count=0;
    for(int i=0; i<=1; ++i){
        for(int j=0; j<=1; ++j){
            if(i==j && i==0){
                count=count_char_on_line_(row, col, win, -1, 1)+count_char_on_line_(row, col, win, 1, -1)-1;
            } else {
                count=count_char_on_line_(row, col, win, i, j)+count_char_on_line_(row, col, win, -i, -j)-1;
            }
            if(count>=len_win_line_){
                return true;
            }
        }
    }
    return false;
}

bool Tic_tac_toe::first_player_choose(){
    return bool((std::rand())%2);
}

Tic_tac_toe::Tic_tac_toe(int row, int col, int len_win_line, std::string player_name_1, std::string player_name_2){
    row_=row;
    col_=col;
    len_win_line_=len_win_line;
    player_name_1_=player_name_1;
    player_name_2_=player_name_2;
    game_space_=new char*[row];
    for(size_t k=0; k<col; ++k){
        game_space_[k]=new char[col];
        for(size_t p=0; p<col; ++p){
            game_space_[k][p]=none_;
        }
    }
}

int Tic_tac_toe::step(int row, int col, char player_char){
    if(game_space_[row][col]!=none_){
        return 0;//занятое поле
    }
    game_space_[row][col]=player_char;
    if(check_win(row, col, player_char)){
        return 2;//победа
    }
    return 1;//ход проведен успешно
}

Tic_tac_toe::~Tic_tac_toe(){
    for(size_t k=0; k<col_; ++k){
        delete[] game_space_[k];
    }
    delete[] game_space_;
}