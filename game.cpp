#include "game.h"
#include "TcpServer.h"
#include "TcpClient.h"

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

Tic_tac_toe::Tic_tac_toe(int row, int col, int len_win_line, int time_per_move, std::string player_name_1, std::string player_name_2, bool first_step, TcpServer* server, TcpClient* client): row_(row), col_(col), len_win_line_(len_win_line), time_per_move_(time_per_move),
      player_name_1_(player_name_1), player_name_2_(player_name_2), first_step_(first_step),
      server_(server), client_(client), step_s_player_(first_step){
    game_space_=new char*[row];
    for(size_t k=0; k<row; ++k){
        game_space_[k]=new char[col];
        for(size_t p=0; p<col; ++p){
            game_space_[k][p]=none_;
        }
    }
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
        out.append(std::string(1+2*col_, '_')+'\n');
        out.push_back('|');
        for(int j=0; j<col_; ++j){
            out.push_back(game_space_[i][j]);
            out.push_back('|');
        }
        out.push_back('\n');
    }
    out.append(std::string(1+2*col_, '_')+'\n');
    return out;
}

void Tic_tac_toe::game() {
    int row = 0;
    int col = 0;
    int res_move = 9;
    bool yes_input_time = true;
    bool input_complete=false;
    bool play_char=false; //символ хода Х или О.

    if (first_step_) {
        std::cout << "First player is " << player_name_2_ << std::endl;
        std::cout << "Second player is " << player_name_1_ << std::endl;
    } else {
        std::cout << "First player is " << player_name_1_ << std::endl;
        std::cout << "Second player is " << player_name_2_ << std::endl;
    }

    for (size_t i = 0; i < (size_t(row_) * size_t(col_)) && res_move != 2; ++i) {
        if(!step_s_player_){//ход игрока, исполняющего ход
            res_move = 1;
            std::cout << "You have " << time_per_move_ << " second for turn" << std::endl;
            do {
                yes_input_time = true;
                input_complete=false;
                auto start = std::chrono::steady_clock::now();

                // Запускаем ввод игрока в отдельном потоке
                std::thread input_thread([&]() {
                    if (step_s_player_) {
                        std::cout << "The second player's move: ";
                    } else {
                        std::cout << "The first player's move: ";
                    }
                    std::cin >> row >> col;
                    input_complete=true;
                });

                // Отслеживаем время на ввод, пока поток не завершится
                while (!input_complete && yes_input_time) {
                    auto now = std::chrono::steady_clock::now();
                    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - start).count();
                    if (elapsed >= time_per_move_) {
                        std::cout << std::endl << "You move time is end" << std::endl;
                        yes_input_time = false;
                        break; // Выходим из цикла while
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }

                // Если время истекло, завершаем поток, иначе ожидаем завершения ввода
                if (input_thread.joinable()) {
                    if (yes_input_time) {
                        input_thread.join();
                    } else {
                        // Завершаем поток корректно, если ввод не завершился вовремя
                        input_thread.detach(); // Завершаем поток без ожидания завершения
                    }
                }

                // Обработка результата хода
                if (yes_input_time) {
                    res_move = step(row, col, play_char);
                    send_move(row, col); // Отправляем ход
                } else {
                    send_move(-1, -1); // Время вышло, пропуск хода
                }
            } while (res_move == 0 && yes_input_time);
        } else {//ход игрока, ожидающего хода
            //чтение хода (row, col, resmove)
            receive_move(row, col);
            if (row != -1 && col != -1) {
                step(row, col, play_char);
            } else {
                std::cout << "Opponent missed their turn.\n";
            }
            if(play_char){
                game_space_[row][col]='O';
            } else {
                game_space_[row][col]='X';
            }
        }
        // Вывод текущего состояния поля
        std::cout << out_place() << std::endl;
        step_s_player_ = !step_s_player_; // Меняем игрока
        play_char=!play_char;
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