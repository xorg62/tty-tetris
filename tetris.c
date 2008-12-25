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

#include "tetris.h"

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


/* Functions */
void
init(void)
{
     int bg;

     /* Init term with ncurses */
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

     /* Init variables */
     score = lines = 0;
     running = True;

     /* Score */
     mvprintw(FRAMEH_NB + 2, FRAMEW + 3, "Score:");
     mvprintw(FRAMEH_NB + 3, FRAMEW + 3, "Lines:");
     DRAW_SCORE();

     return;
}

void
get_key_event(void)
{
     halfdelay(3);

     switch(getch())
     {
     case KEY_LEFT:  shape_move_left();            break;
     case KEY_RIGHT: shape_move_right();           break;
     case KEY_UP:    shape_position_switch(True);  break;
     case KEY_DOWN:  shape_position_switch(False); break;
     case KEY_SPACE: shape_drop();                 break;
     }

     return;
}

void
arrange_score(int l)
{

     /* Standard score count */
     switch(l)
     {
     case 1: score += 40;   break; /* One line */
     case 2: score += 100;  break; /* Two lines */
     case 3: score += 300;  break; /* Three lines */
     case 4: score += 1200; break; /* Four lines */
     }

     lines += l;

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

int
main(int argc, char **argv)
{

     init();
     frame_init();
     frame_nextbox_init();

     current.y = FRAMEW / 2 - 1;
     RAND(current.num, 0, 6);
     RAND(current.next, 0, 6);
     frame_nextbox_refresh();

     while(running)
     {
          get_key_event();
          shape_set();
          frame_refresh();
          shape_go_down();

     }

     endwin();

     return 0;
}
