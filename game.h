#ifndef GAME_2024
#define GAME_2024

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

class Tic_tac_toe{
    char one_char_='X';
    char two_char_='O';
    char none_='*';
    int row_;
    int col_;
    int time_per_move_;
    char** game_space_;
    int len_win_line_;
    std::string player_name_1_;
    bool type_player_;// 0 - server, 1 - client
    std::string player_name_2_;
    bool first_step_;
    bool step_s_player_;

    bool first_player_choose();//0 - первый, 1 - второй.
    bool check_win(int row, int col, char win);
    int count_char_on_line_(int row, int col, char search, int row_change, int col_change);
    std::string out_place();
    int step(int row, int col, bool player_move);
    bool playerMove(); // Метод для выполнения хода игрока с учетом времени
public:
    Tic_tac_toe(int row=3, int col=3, int len_win_line=3, int time_per_move=30, std::string player_name_1="1", std::string player_name_2="2", std::string type_player="player");
    void game();
    ~Tic_tac_toe();
};

#endif