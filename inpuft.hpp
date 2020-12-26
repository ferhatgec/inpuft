/* MIT License
#
# Copyright (c) 2020 Ferhat Geçdoğan All Rights Reserved.
# Distributed under the terms of the MIT License.
#
# */

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <termios.h>

/* Fegeya Inpuft a work-in-progress & really simple replacement of readline.
   
   * TODO:
     * History initialization.
     * Arrow (Left, Right) support with buffer refreshing.
     * UTF-8 support.
*/

/* Some of ASCII values */
#define ARROW_UP    65
#define ARROW_LEFT  68
#define ARROW_RIGHT 67
#define ARROW_DOWN  66
#define ESCAPE      27
#define BACKSPACE   127
#define __BACKSPACE 8
#define H_TAB       9

#define SPACE       32

#define PUT(str)  std::cout << str << std::flush;
#define INPUT(ch) ch = getchar();

static struct termios _new,
                      _old;

/* For initialize cursor position */ 
typedef struct {
public:
	unsigned int x = 0;
	unsigned int y = 0;
} pos_t;

class Inpuft {
public:
	std::vector<std::string> unsupported_terms = {
		"emacs",
		"xterm-256color",
	};
	
	std::vector<std::string>::iterator iter;
	
	pos_t cursor = GetPosition();

	char ch;
	
	std::string data,
                history_file;
	
	unsigned total_line,
             current_line;
public:
	void Init(bool echo); /* With input as string */

	~Inpuft(); 
	
	void GetInput(bool echo, bool unsupported);
	void SetPosition(pos_t pos);
	pos_t GetPosition();
	
	void InitTermios(bool echo);
	void ResetTermios();
	
	void BuffBack(unsigned int i);
	
	/* For history */ 
	void InitFile(std::string file);
	
	unsigned    GetTotalHistoryLine();
	std::string GetSpecificHistoryLine(unsigned line);
};

void
Inpuft::Init(bool echo) {
	iter = std::find(unsupported_terms.begin(), unsupported_terms.end(), getenv("TERM")); 
	
	if(iter != unsupported_terms.end()) { 
    	/* Arrow keys detection not supported for unsupported term/s */ 
    	do {
			GetInput(echo, true);
		} while(ch != '\n');
	} else {
		do {
			GetInput(echo, false);
		} while(ch != '\n');
	}
}

Inpuft::~Inpuft() {
	data.erase();
}

void
Inpuft::GetInput(bool echo, bool unsupported) {
	InitTermios(false);
	
	INPUT(ch)

	switch(ch) {
		case SPACE:
		{
			PUT(" ")
			
			data.append(" ");
			
			break;
		}
	
		case BACKSPACE:
		{
			if(data.length() > 0) {
				PUT("\b \b")
			
				data.pop_back();
				
				cursor.x--;
			}
			
			break;
		}

		
		case ARROW_LEFT:
		{
			PUT("")
			/* Ignore */
			break;
		}
		
		case ARROW_RIGHT:
		{
			PUT("")
			/* Ignore */
			break;
		}
		
		case ARROW_UP:
		{
			if(unsupported == true) break;
			
			if(current_line != 0) {
				current_line--;
				
				BuffBack(data.length());
				
				data = GetSpecificHistoryLine(current_line);
				
				PUT(data)
				/* TODO:
				   * Colorize if fetched data is available on system
				   * Add Colorized library.
				*/ 
			}
			
			break;
		}
		
		case ARROW_DOWN:
		{
			PUT("")
			if(unsupported == true) break;
			
			if(current_line < total_line) {
				current_line++;
			
				BuffBack(data.length());
				
				data = GetSpecificHistoryLine(current_line);
				
				PUT(data)
			}
				
			break;
		}
		
		case H_TAB:
		{
			/* Ignore */
			break;
		}
		
		default:
		{
			data.push_back(ch);
			
			cursor.x++;
			
			PUT(ch)
			
			break;
		}
	}

	
}

void
Inpuft::SetPosition(pos_t pos) {
	PUT("\033[" + 
		std::to_string(pos.y + 1) 
		+ ";" 
		+ std::to_string(pos.x + 1) + "H")
}

pos_t
Inpuft::GetPosition() {
	pos_t _pos;
	
	scanf("\033[%d;%dR", &_pos.x, &_pos.y);
	
	return _pos;
}
	
void
Inpuft::InitTermios(bool echo) {
	tcgetattr(0, &_old);
    _new = _old;
    _new.c_lflag &= ~ICANON;
    _new.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &_new);

	_new.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
}
	
void
Inpuft::ResetTermios() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &_old);
}

void
Inpuft::BuffBack(unsigned int i) {
	for(unsigned f = 0; f < i; f++) {
		std::cout << "\b \b" << std::flush;				
	}
}

void
Inpuft::InitFile(std::string file) {
	history_file = file;
	total_line   = GetTotalHistoryLine();
	current_line = GetTotalHistoryLine();
}

unsigned 
Inpuft::GetTotalHistoryLine() {
	/* TODO:
	  Check file[0] to if is a file (foo_history.txt) or directory (/home/foo/foo_history.txt) 
	*/
	std::ifstream history_stream(history_file);
	
	std::string line;
	unsigned i;
	
	for (i = 0; std::getline(history_stream, line); ++i)
    ;

	history_stream.close();
	
    return i;	
}

std::string 
Inpuft::GetSpecificHistoryLine(unsigned line) {
	std::ifstream history_stream(history_file);
	
	unsigned __line = 0;
	
	std::string data;
	
	while(getline(history_stream, data)) {
		if(line == __line) {
			return data;
		}
		
		__line++;
	}

	history_stream.close();
	
	return data;
}
