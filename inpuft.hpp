/* MIT License
#
# Copyright (c) 2020 Ferhat Geçdoğan All Rights Reserved.
# Distributed under the terms of the MIT License.
#
# */

#include <algorithm>
#include <iostream>
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

#define PUT(str)  std::cout << str;
#define INPUT(ch) ch = getchar();

struct termios _old, _new;

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
	std::string data;
public:
	Inpuft(); /* Just initialize */
	Inpuft(bool echo); /* With input as string */

	~Inpuft(); 
	
	void GetInput(bool echo, bool unsupported);
	void SetPosition(pos_t pos);
	pos_t GetPosition();
	
	void InitTermios(bool echo);
	void ResetTermios();
	
	void BuffBack(unsigned int i);
};

Inpuft::Inpuft(bool echo) {
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
	tcgetattr(0, &_old);
    _new = _old;
    _new.c_lflag &= ~ICANON;
    _new.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &_new);
	fflush(stdout);
	
	INPUT(ch)

	switch(ch) {
		case SPACE:
		{
			std::cout << " " << std::flush;
			
			data.append(" ");
			
			break;
		}
	
		case __BACKSPACE || BACKSPACE:
		{
			if(data.length() > 0) {
				std::cout << "\b \b" << std::flush;
			
				data.pop_back();
				
				cursor.x--;
			}
			
			break;
		}

		
		case ARROW_LEFT:
		{
			/* Ignore */
			break;
		}
		
		case ARROW_RIGHT:
		{
			/* Ignore */
			break;
		}
		
		case ESCAPE:
		{
			if(unsupported == true) break;
			
			ch = getchar();
        	ch = getchar();
            
			if(ch == ARROW_UP) {
				BuffBack(data.length());
			}
		
			if(ch == ARROW_DOWN) {
				BuffBack(data.length());
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
			
			std::cout << ch << std::flush;
			
			break;
		}
	}

    tcsetattr(0, TCSANOW, &_old);
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
}
	
void
Inpuft::ResetTermios() {
    tcsetattr(0, TCSANOW, &_old);
}

void
Inpuft::BuffBack(unsigned int i) {
	for(unsigned f = 0; f < i; f++) {
		std::cout << "\b \b" << std::flush;				
	}
}
