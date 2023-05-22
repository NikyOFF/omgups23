#ifndef CURSOR_H
#define CURSOR_H

#define MOVE_CURSOR_TO_HOME printf("\x1b[H")
#define MOVE_CURSOR_TO_XY(line, column) printf("\x1b[%i;%iH", line, column)
#define MOVE_CURSOR_UP(value) printf("\x1b[%zuA", value)
#define MOVE_CURSOR_DOWN(value) printf("\x1b[%iB", value)
#define MOVE_CURSOR_RIGHT(value) printf("\x1b[%iC", value)
#define MOVE_CURSOR_LEFT(value) printf("\x1b[%iD", value)

#define ERASE_DISPLAY printf("\x1b[J")
#define ERASE_FROM_CURSOR_UNTIL_END_OF_SCREEN printf("\x1b[0J")
#define ERASE_FROM_CURSOR_TO_BEGINNING_OF_SCREEN printf("\x1b[1J")
#define ERASE_ENTIRE_SCREEN printf("\x1b[2J")

#define SAVE_CURSOR_POSITION_1 printf("\033[s")
#define RESTORES_CURSOR_POSITION_1 printf("\033[u")

#endif //CURSOR_H
