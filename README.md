# Tic-tac-toe
Проект по ПЭВМ. Крестики-нолики.
Особенности: возможность сетевой игры, выбор размера поля, длины линии для победы, имени.
Файл конфига может иметь имя только "config.ttt"
Туда нужно записать: имя(username), тип игрока(type),размер поля(row_count, col_count), длина линии для победы(win_line_len), время на ход(step_time), данные для подключения по сети(port, ip)
Для чего нужен каждый тип файлов:
1. Logger - логика для работы с лог файлом.
2. Global_logger - запуск лог файла для остальных типов.
3. TcpClient - сетевая часть игрока, который подключается к игре.
4. TcpServer - сетевая часть игрока, который создает игру.
5. Game - сама игра, необходимая для неё логика.
6. Main - подготовка перед игрой, чтение конфига, создание сетевой игры.

Уточняю, что, несмотря на создание названия лог-файла с временной меткой, в случае если лог-файл не откроется, то логов не будет. Об этом выведется сообщение.