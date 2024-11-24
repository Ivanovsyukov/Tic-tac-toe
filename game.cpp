#include "game.h"

int Tic_tac_toe::count_char_on_line_(int row, int col, char search, int row_change, int col_change) const{
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

bool Tic_tac_toe::check_win(int row, int col, char win) const{
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

Tic_tac_toe::Tic_tac_toe(int row, int col, int len_win_line, int time_per_move, std::string player_name_1, std::string player_name_2, bool first_step, TcpServer* server, TcpClient* client, bool type_connection): row_(row), col_(col), len_win_line_(len_win_line), time_per_move_(time_per_move),
    player_name_1_(player_name_1), player_name_2_(player_name_2), first_step_(first_step),
    server_(server), client_(client), step_s_player_(first_step), type_connection_(type_connection){
    globalLogger.log("Game initialized: " + player_name_1 + " vs " + player_name_2);
    game_space_=new char*[row];
    for(size_t k=0; k<row; ++k){
        game_space_[k]=new char[col];
        for(size_t p=0; p<col; ++p){
            game_space_[k][p]=none_;
        }
    }
}

int Tic_tac_toe::step(int row, int col, bool player_move){
    if(row<0 || col<0 || game_space_[row][col]!=none_){//если поле зянято
        globalLogger.log("Invalid move: row=" + std::to_string(row) + ", col=" + std::to_string(col));
        return 0;//занятое поле
    }
    char player_char=' ';
    if(player_move){
        player_char='O';
    }else {
        player_char='X';
    }
    game_space_[row][col]=player_char;//вставляем символ хода
    globalLogger.log("Move: player=" + std::string(1, player_char) + ", row=" + std::to_string(row) + ", col=" + std::to_string(col));
    if(check_win(row, col, player_char)){//проверка на успех
        globalLogger.log("Player " + std::string(1, player_char) + " wins!");
        return 2;//победа
    }
    return 1;//ход проведен успешно
}

std::string Tic_tac_toe::out_place()const{
    std::string out="Player ";
    if(step_s_player_){//добавляем в вывод на консоль
        out.append(player_name_1_+'\n');
    } else {
        out.append(player_name_2_+'\n');
    }
    for(int i=0; i<row_; ++i){//добавление поля в вывод
        out.append(std::string(2*col_-1, '_')+'\n');
        out.push_back('|');
        for(int j=0; j<col_; ++j){
            out.push_back(game_space_[i][j]);
            out.push_back('|');
        }
        out.push_back('\n');
    }
    out.append(std::string(2*col_-1, '_')+'\n');
    return out;
}

bool Tic_tac_toe::send_move(int row, int col){
    std::string move="-,-";//если ход пропущен
    if(row>=0 && col>=0){
        move = std::to_string(row) + "," + std::to_string(col);//ход есть
    }
    globalLogger.log("Sending move: " + move);
    bool check_send=true;//проверка на отправление
    if (!type_connection_) {
        check_send=server_->sendMessage(move);
    } else if (client_) {
        check_send=client_->sendMessage(move);
    }
    return check_send;
}

bool Tic_tac_toe::receive_move(int& row, int& col) {
    std::string move="";
    if (!type_connection_) {
        move = server_->receiveMessage();
    } else if (client_) {
        move = client_->receiveMessage();
    }
    if(move.empty()){
        return false;
    }
    globalLogger.log("Received move: " + move);
    if (move == "-,-") {
        row = -1; // Пропуск хода
        col = -1;
    } else {
        size_t delimiter = move.find(',');
        row = std::stoi(move.substr(0, delimiter));
        col = std::stoi(move.substr(delimiter + 1));
    }
    return true;
}

void Tic_tac_toe::game() {
    globalLogger.log("Game started");
    int row = 0;
    int col = 0;
    int res_move = 9;//для записи результата хода
    bool yes_input_time = true;//для проверки времени хода
    bool input_complete=false;
    bool play_char=false; //символ хода Х или О.
    bool tcp_error=false;//ошибка в сети
    std::string input="";
    //выводим информацию о порядке игроков
    if (first_step_) {
        std::cout << "First player is " << player_name_2_ << std::endl;
        std::cout << "Second player is " << player_name_1_ << std::endl;
    } else {
        std::cout << "First player is " << player_name_1_ << std::endl;
        std::cout << "Second player is " << player_name_2_ << std::endl;
    }

    for (size_t i = 0; i < (size_t(row_) * size_t(col_)) && res_move != 2 && !tcp_error; ++i) {
        if(!step_s_player_){//ход игрока, исполняющего ход
            globalLogger.log("Player step of doing");
            res_move = 1;
            std::cout << "You have " << time_per_move_ << " second for turn" << std::endl;
            do {
                yes_input_time = true;
                input_complete=false;
                auto start = std::chrono::steady_clock::now();
                // Запускаем ввод игрока в отдельном потоке
                globalLogger.log("Create new threat");
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
                        globalLogger.log("End doing player time");
                        std::cout << std::endl << "You move time is end" << std::endl;
                        yes_input_time = false;
                        break; // Выходим из цикла while
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                // Если время истекло, завершаем поток, иначе ожидаем завершения ввода
                if (input_thread.joinable()) {
                    globalLogger.log("Close threat");
                    if (yes_input_time) {
                        input_thread.join();
                    } else {
                        // Завершаем поток корректно, если ввод не завершился вовремя
                        input_thread.detach(); // Завершаем поток без ожидания завершения
                    }
                }
                // Обработка результата хода
                globalLogger.log("Work with turn");
                if (yes_input_time) {
                    globalLogger.log("Check turn");
                    res_move = step(row, col, play_char);
                    if(res_move!=0){
                        globalLogger.log("Before send turn");
                        if(!send_move(row, col)){
                            tcp_error=true;
                            res_move=-1;
                        }
                    }
                } else {
                    globalLogger.log("Send time out");
                    if(!send_move(-1, -1)){
                        tcp_error=true;
                        res_move=-1;
                        continue;
                    }
                }
            } while (res_move == 0 && yes_input_time);
        } else {//ход игрока, ожидающего хода
            globalLogger.log("Player step of reading");
            //чтение хода 
            globalLogger.log("Read turn");
            if(!receive_move(row, col)){
                tcp_error=true;
                res_move=-1;
                continue;
            }
            if (row != -1 && col != -1) {
                globalLogger.log("It is turn");
                res_move=step(row, col, play_char);//тут мы уверены, что проблем нет
            } else {
                globalLogger.log("Opponent missed their turn.");
                std::cout << "Opponent missed their turn." << std::endl;
            }
            globalLogger.log("Add in game_space");
        }
        // Вывод текущего состояния поля
        globalLogger.log("Console out game_space");
        std::cout << out_place() << std::endl;//выводим поле в консоль
        globalLogger.log("Change step_s_player and play_char");
        step_s_player_ = !step_s_player_; // Меняем игрока
        play_char=!play_char;//меняем символ с крестика на нолик и наоборот
    }

    // Проверка на победу или ничью
    if (res_move == 1) {
        globalLogger.log("Game ended: Draw");
        std::cout << "No winner" << std::endl;
    } else if (res_move == 2){
        std::cout << "Player ";
        if (!step_s_player_) {
            std::cout << player_name_1_ << " is winner" << std::endl;
        } else {
            std::cout << player_name_2_ << " is winner" << std::endl;
        }
        globalLogger.log("Game ended with winner");
    } else {
        globalLogger.log("Game ended with error");
    }
}

Tic_tac_toe::~Tic_tac_toe(){
    globalLogger.log("Game resources are being cleaned up");
    for(size_t k=0; k<row_; ++k){
        delete[] game_space_[k];
    }
    delete[] game_space_;
}