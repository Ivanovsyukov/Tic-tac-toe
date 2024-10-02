#ifndef GAME_2024
#define GAME_2024

class Tic_tac_toe{
    unsigned int row;
    unsigned int col;
    unsigned int** game_space;
    unsigned int len_win_line;

    unsigned int first_player_choose();
    bool check_win();
public:
    Tic_tac_toe(unsigned int row, unsigned int col, unsigned int len_win_line);
    ~Tic_tac_toe();
}

#endif