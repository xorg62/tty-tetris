/*
 *      shapes.c
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

void
shape_set(void)
{
     int i, j;

     for(i = 0; i < 4; ++i)
          for(j = 0; j < EXP_FACT; ++j)
               frame[current.x + shapes[current.num][current.pos][i][0]]
                    [current.y + shapes[current.num][current.pos][i][1] * EXP_FACT + j]
                    = current.num + 1;

     if(current.x < 1)
          for(i = 0; i < FRAMEW + 1; ++i)
               frame[0][i] = Border;

     return;
}

void
shape_unset(void)
{
     int i, j;

     for(i = 0; i < 4; ++i)
          for(j = 0; j < EXP_FACT; ++j)
               frame[current.x + shapes[current.num][current.pos][i][0]]
                    [current.y + shapes[current.num][current.pos][i][1] * EXP_FACT + j] = 0;

     if(current.x < 1)
          for(i = 0; i < FRAMEW + 1; ++i)
               frame[0][i] = Border;
     return;
}

void
shape_new(void)
{
     /* Draw the previous shape for it stay there */
     shape_set();
     /* Check if there is plain line */
     check_plain_line();
     /* Set the new shape property */
     current.x = 0;
     current.y = FRAMEW / 2 - 1;
     /* Set the current.num the current.next */
     current.num = current.next;
     /* Set the current.next for the next time */
     current.next = RAND(0, 6);
     /* Refresh nextbox */
     frame_nextbox_refresh();
     sleep(0.5);

     return;
}

void
shape_go_down(void)
{
     shape_unset();

     /* Lose conditions */
     if(current.x == 0
        && check_possible_pos(current.x + 1, current.y))
          running = False;

     /* Fall the shape else; collision with the ground or another shape
      * then stop it and create another */
     if(!check_possible_pos(current.x + 1, current.y))
          current.x += 1;
     else
          shape_new();

     return;
}

void
shape_set_position(int p)
{
     int old = current.pos;

     shape_unset();

     current.pos = p ;

     if(check_possible_pos(current.x, current.y))
          current.pos = old;

     return;
}


void
shape_move(int n)
{

     shape_unset();

     if(!check_possible_pos(current.x, current.y + n))
          current.y += n;

     return;
}

void
shape_drop(void)
{
     while(!check_possible_pos(current.x + 1, current.y))
     {
          shape_unset();
          ++current.x;
     }
     score += FRAMEH - current.x;

     return;
}
