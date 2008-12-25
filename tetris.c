/*
 *      tetris.c TTY-TETRIS Main file.
 *      Copyright © 2008 Martin Duquesnoy <xorg62@gmail.com>
 *      All rights reserved.
 *
 *      Redistribution and use in source and binary forms, with or without
 *      modification, are permitted provided that the following conditions are
 *      met:
 *
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following disclaimer
 *        in the documentation and/or other materials provided with the
 *        distribution.
 *      * Neither the name of the  nor the names of its
 *        contributors may be used to endorse or promote products derived from
 *        this software without specific prior written permission.
 *
 *      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <ncurses.h>
#include <unistd.h>

/* Term dimension */
#define MAXW getmaxx(stdscr)
#define MAXH getmaxy(stdscr)
#define KEY_SPACE 32

/* Expension factor of shape */
#define EXP_FACT 2

/* Frame dimension */
#define FRAMEW 23
#define FRAMEH 23

/* Generate Random number */
#define RAND(x, m, M) x = ((rand() % (M - m + 1)) + m)

/* Draw the score.. */
#define DRAW_SCORE() attron(COLOR_PAIR(Score));\
     mvprintw(0, FRAMEW + 10, "%d", score); attroff(COLOR_PAIR(Score));

/* Bool type */
typedef enum { False, True } Bool;

struct shape_t
{
     int num;
     int next;
     int pos;
     int x, y;
} current;

void init(void);
void init_frame(void);
void do_fall_shape(void);
int check_possible_pos(int, int);
void arrange_score(int);
void check_plain_line(void);
void get_key_event(void);
void shape_position_switch(Bool);
void move_left();
void move_right();
void set_shape(Bool);
void draw_frame(void);

/* Color enum */
enum { Black, Blue, Red, Magenta, White, Green, Cyan, Yellow, Border, Score, ColLast };

/* Shapes data */
const int shapes[7][4][4][2] =
{
     /* O */
     {
          {{0,0},{1,0},{0,1},{1,1}},
          {{0,0},{1,0},{0,1},{1,1}},
          {{0,0},{1,0},{0,1},{1,1}},
          {{0,0},{1,0},{0,1},{1,1}}
     },
     /* I */
     {
          {{1,0},{1,1},{1,2},{1,3}},
          {{0,1},{1,1},{2,1},{3,1}},
          {{1,0},{1,1},{1,2},{1,3}},
          {{0,1},{1,1},{2,1},{3,1}}
     },
     /* L */
     {
          {{0,1},{1,1},{2,1},{2,2}},
          {{1,0},{1,1},{1,2},{2,0}},
          {{0,0},{0,1},{1,1},{2,1}},
          {{1,0},{1,1},{1,2},{0,2}}
     },
     /* J */
     {
          {{1,0},{1,1},{1,2},{2,2}},
          {{0,2},{1,2},{2,2},{2,1}},
          {{0,0},{1,0},{1,1},{1,2}},
          {{0,1},{0,2},{1,1},{2,1}}
     },
     /* S */
     {
          {{1,1},{1,2},{2,0},{2,1}},
          {{0,1},{1,1},{1,2},{2,2}},
          {{1,1},{1,2},{2,0},{2,1}},
          {{0,1},{1,1},{1,2},{2,2}}
     },
     /* Z */
     {
          {{0,0},{0,1},{1,1},{1,2}},
          {{0,2},{1,1},{2,1},{1,2}},
          {{0,0},{0,1},{1,1},{1,2}},
          {{0,2},{1,1},{2,1},{1,2}}
     },
     /* T */
     {
          {{0,1},{1,0},{1,1},{1,2}},
          {{0,1},{1,1},{1,2},{2,1}},
          {{1,0},{1,1},{1,2},{2,1}},
          {{1,0},{0,1},{1,1},{2,1}}
     }
};

/* Frame data */
int frame[FRAMEH + 1][FRAMEW + 1] = {{0}};
int next_shape_frame[6][12] = {{0}};
int score = 0;

/* Functions */

void
init(void)
{
     int bg;

     initscr();
     noecho();
     keypad(stdscr, True);
     curs_set(False);

     /* Init colors */
     start_color();

     /* Init background color (term color, else black color). */
     bg = (use_default_colors() == OK) ? -1 : COLOR_BLACK;
     init_pair(Black,   bg, COLOR_BLACK);
     init_pair(White,   bg, COLOR_WHITE);
     init_pair(Blue,    bg, COLOR_BLUE);
     init_pair(Red,     bg, COLOR_RED);
     init_pair(Green,   bg, COLOR_GREEN);
     init_pair(Yellow,  bg, COLOR_YELLOW);
     init_pair(Magenta, bg, COLOR_MAGENTA);
     init_pair(Cyan,    bg, COLOR_CYAN);
     init_pair(Border,  bg, COLOR_WHITE);
     init_pair(Score,   COLOR_WHITE, bg);

     /* Refresh term screen and clean it */
     refresh();
     clear();

     /* Make rand() really random :) */
     srand(getpid());

     return;
}

void
init_frame(void)
{
     int i;

     /* Frame border */
     for(i = 0; i < FRAMEW + 1; ++i)
     {
          frame[0][i] = Border;
          frame[FRAMEH][i] = Border;
     }
     for(i = 0; i < FRAMEH; ++i)
     {
          frame[i][0] = Border;
          frame[i][1] = Border;
          frame[i][FRAMEW] = Border;
          frame[i][FRAMEW - 1] = Border;
     }

     /* Score */
     mvprintw(0, FRAMEW + 3, "Score:");
     DRAW_SCORE();

     /* Next shape box */
     attron(COLOR_PAIR(Border));
     for(i = 0; i < 14; ++i)
     {
          mvaddch((i / 2) + 3, FRAMEW + 3, ' ');
          mvaddch((i / 2) + 3, FRAMEW + 4, ' ');
          mvaddch((i / 2) + 3, FRAMEW + 16, ' ');
          mvaddch((i / 2) + 3, FRAMEW + 17, ' ');
          mvaddch(3, FRAMEW + 4 + i, ' ');
          mvaddch(9, FRAMEW + 4 + i, ' ');
     }
     attroff(COLOR_PAIR(Border));

     return;
}

void
draw_next_shape(void)
{
     int i, j;

     for(i = 1; i < 6; ++i)
          for(j = 1; j < 12; ++j)
               next_shape_frame[i][j] = 0;

     for(i = 0; i < 4; ++i)
          for(j = 0; j < EXP_FACT; ++j)
               next_shape_frame[2 + shapes[current.next][0][i][0]]
                    [3 + shapes[current.next][0][i][1] * EXP_FACT + j] = current.next + 1;

     for(i = 1; i < 6; ++i)
          for(j = 1; j < 12; ++j)
          {
               attron(COLOR_PAIR(next_shape_frame[i][j]));
               mvaddch(i + 3, j + FRAMEW + 4, ' ');
               attroff(COLOR_PAIR(next_shape_frame[i][j]));
          }

     return;
}

void
do_fall_shape(void)
{
     set_shape(True);

     /* Fall the shape */
     if(!check_possible_pos(current.x + 1, current.y))
          current.x += 1;

     /* else stop it and create another */
     else
     {
          set_shape(False);
          check_plain_line();
          current.x = 0;
          current.y = 8;
          current.num = current.next;
          RAND(current.next, 0, 6);
          draw_next_shape();
     }

     return;
}

void
get_key_event(void)
{
     halfdelay(2);

     switch(getch())
     {
     case KEY_LEFT:  move_left();                  break;
     case KEY_RIGHT: move_right();                 break;
     case KEY_UP:    shape_position_switch(True);  break;
     case KEY_DOWN:  shape_position_switch(False); break;

     case KEY_SPACE:
          while(!check_possible_pos(current.x + 1, current.y))
          {
               set_shape(True);
               ++current.x;
          }
          break;

     }

     return;
}

void
arrange_score(int lines)
{

     /* Standard score count */
     switch(lines)
     {
     case 1: score += 40;   break;
     case 2: score += 100;  break;
     case 3: score += 300;  break;
     case 4: score += 1200; break;
     }

     DRAW_SCORE();

     return;
}

void
check_plain_line(void)
{
     int i, j, k, f, c = 0, nl = 0;

     for(i = 1; i < FRAMEH; ++i)
     {
          for(j = 1; j < FRAMEW; ++j)
               if(frame[i][j] == 0)
                    ++c;
          if(!c)
          {
               ++nl;
               for(k = i - 1; k > 1; --k)
                    for(f = 1; f < FRAMEW; ++f)
                         frame[k + 1][f] = frame[k][f];
          }
          c = 0;
     }
     arrange_score(nl);

     return;
}

int
check_possible_pos(int x, int y)
{
     int i, j, c = 0;

     for(i = 0; i < 4; ++i)
          for(j = 0; j < EXP_FACT; ++j)
               if(frame[x + shapes[current.num][current.pos][i][0]]
                  [y + shapes[current.num][current.pos][i][1] * EXP_FACT + j] != 0)
                    ++c;

     return c;
}

void
set_shape(Bool e)
{
     int i, j;

     for(i = 0; i < 4; ++i)
          for(j = 0; j < EXP_FACT; ++j)
               frame[current.x + shapes[current.num][current.pos][i][0]]
                    [current.y + shapes[current.num][current.pos][i][1] * EXP_FACT + j]
                    = (e) ? 0 : current.num + 1;

     /* Redraw top border if the shape is on it */
     if(current.x < 1)
          for(i = 0; i < FRAMEW + 1; ++i)
               frame[0][i] = Border;

     return;
}

/* b True -> next position;
 * b False -> previous postion; */
void
shape_position_switch(Bool b)
{
     int old_pos = current.pos;

     set_shape(True);

     if(b)
          current.pos = ((current.pos < 3) ? current.pos + 1 : 0);
     else
          current.pos = ((current.pos > 0) ? current.pos - 1 : 3);

     if(check_possible_pos(current.x, current.y))
          current.pos = old_pos;

     return;
}

void
move_left(void)
{
     set_shape(True);
     if(!check_possible_pos(current.x, current.y - EXP_FACT))
        current.y -= EXP_FACT;

     return;
}

void
move_right(void)
{
     set_shape(True);
     if(!check_possible_pos(current.x, current.y + EXP_FACT))
          current.y += EXP_FACT;

     return;
}

void
refresh_frame(void)
{
     int i, j;

     for(i = 0; i < FRAMEH + 1; ++i)
          for(j = 0; j < FRAMEW + 1; ++j)
          {
               attron(COLOR_PAIR(frame[i][j]));
               mvaddch(i, j, ' ');
               attroff(COLOR_PAIR(frame[i][j]));
          }

     refresh();

     return;
}

int
main(int argc, char **argv)
{
     init();
     init_frame();
     RAND(current.num, 0, 6);
     RAND(current.next, 0, 6);
     draw_next_shape();

     for(;;)
     {
          get_key_event();
          set_shape(False);
          refresh_frame();
          do_fall_shape();
     }

     endwin();

     return 0;
}
