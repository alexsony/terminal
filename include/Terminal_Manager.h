#ifndef TERMINAL_MANAGER_H
#define TERMINAL_MANAGER_H

//keys
#define KEY_ENTER 10
#define KEY_SPACEBAR 32
#define KEY_END 70
#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_RIGHT 67
#define KEY_LEFT 68
#define KEY_BACKSPACE 127
#define DELIMITER 27

//colors

//Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define BLU "\e[0;34m"
#define WHT "\e[0;37m"

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BBLU "\e[1;34m"
#define BWHT "\e[1;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define BLUB "\e[44m"
#define WHTB "\e[47m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HBLU "\e[0;94m"
#define HWHT "\e[0;97m"

//Reset
#define RESET "\e[0m"

int getch();

#endif