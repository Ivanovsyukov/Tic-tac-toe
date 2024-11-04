#include "game.h"

int Tic_tac_toe::count_char_on_line_(int row, int col, char search, int row_change, int col_change){
    int count=0;
    bool end=false;
    int j=col;
    for(int i=row; !end && i>=0 && i<row_ && j>=0 && j<col_; i+=row_change, j+=col_change){
        if(game_space_[i][j]==search){
            ++count;
        } else {
            end=true;
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

Tic_tac_toe::Tic_tac_toe(int row, int col, int len_win_line, int time_per_move, std::string player_name_1, std::string player_name_2){
    row_=row;
    col_=col;
    time_per_move_=time_per_move;
    len_win_line_=len_win_line;
    player_name_1_=player_name_1;
    player_name_2_=player_name_2;
    game_space_=new char*[row];
    for(size_t k=0; k<row; ++k){
        game_space_[k]=new char[col];
        for(size_t p=0; p<col; ++p){
            game_space_[k][p]=none_;
        }
    }
    step_s_player_=first_player_choose();
    first_step_=step_s_player_;
}

int Tic_tac_toe::step(int row, int col, bool player_move){
    char player_char=' ';
    if(player_move){
        player_char='O';
    }else {
        player_char='X';
    }
    if(row<0 || col<0 || game_space_[row][col]!=none_){
        return 0;//занятое поле
    }
    game_space_[row][col]=player_char;
    if(check_win(row, col, player_char)){
        return 2;//победа
    }
    return 1;//ход проведен успешно
}

std::string Tic_tac_toe::out_place(){
    std::string out="Player ";
    if(step_s_player_){
        out.append(player_name_1_+'\n');
    } else {
        out.append(player_name_2_+'\n');
    }
    for(int i=0; i<row_; ++i){
        out.append("_______\n");
        out.push_back('|');
        for(int j=0; j<col_; ++j){
            out.push_back(game_space_[i][j]);
            out.push_back('|');
        }
        out.push_back('\n');
    }
    out.append("_______\n");
    return out;
}

/*

    
   
    // Цикл, который проверяет, не истекло ли время, пока идет ввод
    
    std::cout << "Вы выбрали клетку: " << move << '\n';

*/

void Tic_tac_toe::game() {
    int row = 0;
    int col = 0;
    int res_move = 9;
    bool yes_input_time = true;

    if (first_step_) {
        std::cout << "Первый игрок - " << player_name_2_ << std::endl;
        std::cout << "Второй игрок - " << player_name_1_ << std::endl;
    } else {
        std::cout << "First player is " << player_name_1_ << std::endl;
        std::cout << "Second player is " << player_name_2_ << std::endl;
    }

    for (size_t i = 0; i < (size_t(row_) * size_t(col_)) && res_move != 2; ++i) {
        res_move = 1;
        if (step_s_player_) {
            std::cout << "The second player's move: ";
        } else {
            std::cout << "The first player's move: ";
        }
        std::cout << "У вас есть " << time_per_move_ << " секунд для хода." << std::endl;

        do {
            yes_input_time = true;
            auto start = std::chrono::steady_clock::now();

            // Запускаем ввод игрока в отдельном потоке
            std::thread input_thread([&]() {
                std::cout << "Введите ваш ход (координаты клетки): ";
                std::cin >> row >> col;
            });

            // Отслеживаем время на ввод, пока поток не завершится
            while (input_thread.joinable() && yes_input_time) {
                auto now = std::chrono::steady_clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();

                if (elapsed >= time_per_move_) {
                    std::cout << "Время на ход истекло!" << std::endl;
                    input_thread.detach(); // Отключаем поток ввода, если время вышло
                    yes_input_time = false;
                    break; // Выходим из цикла while
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }

            // Завершаем поток, если игрок успел сделать ввод
            if (input_thread.joinable()) {
                input_thread.join();
            }

            // Обработка результата хода
            if (yes_input_time) {
                res_move = step(row, col, step_s_player_);
                if (res_move == 0) {
                    std::cout << "Your move is wrong. Do new: ";
                }
            } else {
                // Если ход просрочен, прерываем цикл do-while
                break;
            }

        } while (res_move == 0 && yes_input_time);

        // Вывод текущего состояния поля
        std::cout << out_place() << std::endl;
        step_s_player_ = !step_s_player_; // Меняем игрока
    }

    // Проверка на победу или ничью
    if (res_move == 1) {
        std::cout << "No winner" << std::endl;
    } else {
        std::cout << "Player ";
        if (!step_s_player_) {
            std::cout << player_name_1_ << " is winner" << std::endl;
        } else {
            std::cout << player_name_2_ << " is winner" << std::endl;
        }
    }
}

Tic_tac_toe::~Tic_tac_toe(){
    for(size_t k=0; k<row_; ++k){
        delete[] game_space_[k];
    }
    delete[] game_space_;
}