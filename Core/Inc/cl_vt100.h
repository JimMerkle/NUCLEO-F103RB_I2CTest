// Copyright Jim Merkle, 11/17/2023
// File: cl_vt100.h
//
// Example ANSI cursor movement code to be used with a VT100 compatible terminal
//
// Notes: Cursor positions begin at upper left corner of terminal, 1,1

#ifndef _CL_VT100_H_
#define _CL_VT100_H_

#define VT100_CLEAR_SCREEN 		"\033[2J"
#define VT100_CURSOR_HOME 		"\033[H"
#define VT100_CURSOR_Y_X    	"\033["   // ESC [ pl ; pc H   set cursor position - pl Line, pc Column
#define VT100_CURSOR_SAVE   	"\0337"
#define VT100_CURSOR_RESTORE	"\0338"
#define VT100_CURSOR_OFF		"\033[?25l" // not standard, but supported by TeraTerm
#define VT100_CURSOR_ON			"\033[?25h" // not standard, but supported by TeraTerm

#define VT100_LINE_DRAW     	"\033(0"  // Select DEC Line Drawing character set
#define VT100_ASCII         	"\033(B"  // Restore to ASCII character set



int cl_vt100(void);

#endif /* _CL_VT100_H_ */

