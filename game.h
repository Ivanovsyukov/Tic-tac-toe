#ifndef GAME_2024
#define GAME_2024

#include <string>
#include <iostream>
#include <thread>
#include <chrono>

class Tic_tac_toe{
    const char one_char_='X';
    const char two_char_='O';
    const char none_='*';
    const int row_;
    const int col_;
    const int time_per_move_;
    char** game_space_;
    const int len_win_line_;
    std::string player_name_1_;
    std::string player_name_2_;
    const bool first_step_;
    bool step_s_player_;

    const bool type_connection_;//0 - сервер, 1 - клиент.
    TcpServer* server_; // Сетевой сервер
    TcpClient* client_; // Сетевой клиент

    bool check_win(int row, int col, char win);
    int count_char_on_line_(int row, int col, char search, int row_change, int col_change);
    std::string out_place();
    int step(int row, int col, bool player_move);
    void send_move(int row, int col);
    void receive_move(int& row, int& col);
public:
    Tic_tac_toe(int row=3, int col=3, int len_win_line=3, int time_per_move=30, std::string player_name_1="1", std::string player_name_2="2", bool first_step, TcpServer* server, TcpClient* client, bool type_connection);
    void game();
    ~Tic_tac_toe();
};

#endif