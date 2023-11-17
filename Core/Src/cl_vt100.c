// Copyright Jim Merkle, 11/17/2023
// File: cl_vt100.c
//
// Example ANSI cursor movement code to be used with a VT100 compatible terminal
//

#include <stdio.h>
#include "command_line.h"
#include "cl_vt100.h"
#include "main.h" // HAL APIs

/*
Cursor Functions:

 *  ESC [ pn A        cursor up pn times - stop at top
 *  ESC [ pn B        cursor down pn times - stop at bottom
 *  ESC [ pn C        cursor right pn times - stop at far right
 *  ESC [ pn D        cursor left pn times - stop at far left
 *  ESC [ pl ; pc H   set cursor position - pl Line, pc Column
 *  ESC [ H           set cursor home
 *  ESC [ pl ; pc f   set cursor position - pl Line, pc Column
 *  ESC [ f           set cursor home
 *  ESC D             cursor down - at bottom of region, scroll up
 *  ESC M             cursor up - at top of region, scroll down
 *  ESC E             next line (same as CR LF)
 *  ESC 7             save cursor position(char attr,char set,org)
 *  ESC 8             restore position (char attr,char set,origin)

 *  ESC [ 2 J         erase entire screen (cursor doesn't move)
*/

// Using DEC Drawing character set, draw a line on row, from col1 to col2
void horizontal_line(int row, int col1, int col2)
{
	// Adjust col1 & col2 such that col1 is <= col2
	if(col1 > col2) {int temp = col2; col2=col1; col1=temp;}
	printf(VT100_CURSOR_Y_X "%d;%dH" VT100_LINE_DRAW,row,col1);
	for(int i=col1;i<=col2;i++) printf("q"); // horizontal line graphic character
	printf(VT100_ASCII);
}

// Using DEC Drawing character set, draw a line on col, from row1 to row2
void vertical_line(int col, int row1, int row2)
{
	// Adjust row1 & row2 such that row1 is <= row2
	if(row1 > row2) {int temp = row2; row2=row1; row1=temp;}
	printf(VT100_LINE_DRAW);
	for(int i=row1;i<=row2;i++) {
		printf(VT100_CURSOR_Y_X "%d;%dHx",i,col); // move cursor, vertical line graphic character
	}
	printf(VT100_ASCII);
}

// VT100 uses row;col order.  Follow suit here - upper left corner, lower right corner
void box(int row1, int col1, int row2, int col2)
{
	// Adjust row1 & row2 such that row1 is <= row2
	if(row1 > row2) {int temp = row2; row2=row1; row1=temp;}
	// Adjust col1 & col2 such that col1 is <= col2
	if(col1 > col2) {int temp = col2; col2=col1; col1=temp;}
	if(row2-row1 <2 || col2-col1 < 2) {
		// There should be at least spot within the box
		printf("box too small!\n");
		return;
	}
	// Draw upper left corner
	printf(VT100_CURSOR_Y_X "%d;%dH" VT100_LINE_DRAW "l",row1,col1);
	// horizontal line
	horizontal_line(row1,col1+1,col2-1);
	// upper right corner
	printf(VT100_LINE_DRAW "k");
	// left vertical line
	vertical_line(col1,row1+1,row2-1);
	// right vertical line
	vertical_line(col2,row1+1,row2-1);
	// lower left corner
	printf(VT100_CURSOR_Y_X "%d;%dH" VT100_LINE_DRAW "m",row2,col1);
	// horizontal line
	horizontal_line(row2,col1+1,col2-1);
	// lower right corner
	printf(VT100_LINE_DRAW "j" VT100_ASCII);
}

// Draw Snake
// Provide initial snake position, followed by null terminated char array using characters:
// U: up, D: down, L: left, R: right for location of next snake segment
// Provide a delay (milliseconds) after drawing each segment
void snake(int row, int col, const char * path, int seg_delay)
{
	// Cursor display off
	printf(VT100_CURSOR_OFF);

	// initial snake position
	printf(VT100_CURSOR_Y_X "%d;%dH" VT100_LINE_DRAW "a",row,col);
	for(;*path;) {
		switch(*path++) {
			case 'U': row--; break;
			case 'D': row++; break;
			case 'L': col--; if(col<1) col=1; break;
			case 'R': col++; if(col>80) col=80; break;
			default: printf(VT100_ASCII "%s error",__func__); return;
		} // switch
		// position cursor, write character
		printf(VT100_CURSOR_Y_X "%d;%dH" VT100_LINE_DRAW "a",row,col);
		HAL_Delay(seg_delay);
	} // for-loop

	// Cursor display on
	printf(VT100_CURSOR_ON);
}

const char snakepath[]={"RRRRRLLDDDDDDDDLLLLLU"}; // draw Capital J

// vt100 example test code
int cl_vt100(void)
{
	// Clear screen, move cursor to 1,1, write "Home"
	printf(VT100_CLEAR_SCREEN VT100_CURSOR_HOME "Home");
#if 0
	// create box 1,10 (upper left) to 15,25 (lower right)
	horizontal_line(10,1,15);
	horizontal_line(25,1,15);
	vertical_line(1,10,25);
	vertical_line(15,10,25);
	// Yes, corner characters would be nice...
#else
	box(10,1,25,15); // Much nicer box
#endif
	printf("\nBelow box " VT100_LINE_DRAW "%c" VT100_ASCII "\n",0x61); // 0x61 Line Draw is a solid box

	// Draw a short "snake", inside the box, growing slowly
	// Remember current cursor position
	printf(VT100_CURSOR_SAVE);

	snake(14, 6, snakepath, 500);

	// Restore cursor position
	printf(VT100_CURSOR_RESTORE);

	return 0;
}
