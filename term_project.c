#include <stdio.h>
#include "string.h"
#include <curses.h>

struct node {
	char statement[40];  // max. 40 characters
	int next;  // points to the textbuffer[] index of the next statement
};
	
	struct node textBuffer[200];
	int free_head, inuse_head;
	
	void edit(char* filename) {
		struct node Line;
		int nextline = 0, lastline = 0;
		inuse_head = 0;
		free_head = 0;
		
		FILE* fp;
		fp = fopen(filename, "a+");	//opens the given file
		while (free_head < 200 && fgets(Line.statement, 40, (FILE*)fp)){	// adds first 25 lines in a file to textbuffer
			if (Line.statement[strlen(Line.statement)-1] == '\n')
				Line.statement[strlen(Line.statement)-1] = '\0';	// replaces eol/eof with terminating null
			Line.next = inuse_head + 1;
			textBuffer[inuse_head++] = Line;
			free_head++;
		}
			if (free_head != 0)
				textBuffer[inuse_head-1].next = -1;
			fclose(fp);	// closes the file
			inuse_head = 25;
	}
	
	void insert(int line, char* stat) {
		if (free_head < 200) {
			
			if (line != free_head){
				
				for (int i = free_head; i > line; i--) {
					textBuffer[i] = textBuffer[i-1];
					textBuffer[i].next += 1;
				}
			}
			else
				textBuffer[line-1].next = line;
			
			strncpy(textBuffer[line].statement, stat, 40);
			textBuffer[line].next = line+1;
			textBuffer[free_head].next = -1;
			free_head++;
		}
	}
	
	void delete(int line){
		if (free_head > 0 && inuse_head < free_head) {
			
			for (int i = line; i < free_head; i++){
				textBuffer[i] = textBuffer[i+1];
				textBuffer[i].next -= 1;
			}
			free_head--;
			textBuffer[free_head-1].next = -1;
		}
    }
	
	void save(char* filename){ 
        FILE *fptr;
        struct node temp = textBuffer[inuse_head];
        fptr = fopen(filename,"w");
        if (temp.statement != NULL && inuse_head < free_head){
                while(temp.next != -1){
                        fputs(strcat(temp.statement, "\n"), fptr);
                        temp = textBuffer[temp.next];
                }
				if (temp.statement[strlen(temp.statement)-1] == '\n')
					temp.statement[strlen(temp.statement)-1] = '\0';
                fputs(temp.statement, fptr);
        }
        fclose(fptr);
}
	
int main(int argc, char* argv[]) {
	if (argc != 2)
		return 1;
	
	char inp;
	char inpStr[40] = "";
	struct node tempNode;
	int page_head = 0;
	
	edit(argv[1]);
	
	initscr();
	move(0,0);
	while (inp != 'X') {
		tempNode = textBuffer[page_head];
		if (textBuffer[page_head].next != NULL && free_head > 0){
			while (tempNode.next != -1 && tempNode.next < page_head + 25){
			printw("\t%s\n", tempNode.statement);
			tempNode = textBuffer[tempNode.next];
			}
			printw("\t%s", tempNode.statement);
		}
		move(inuse_head - page_head, 8);
		refresh();
		
		scanf("%c", &inp);
		switch (inp){
		case 'w':
			if (inuse_head > 0){
				move(--inuse_head, 8);
				if (page_head > 0 && inuse_head < page_head)
					page_head--;
			}
			break;
		case 'z':
			if (inuse_head < 200 && inuse_head < free_head){
				move(++inuse_head, 8);
				if (page_head+24 < 200 && page_head+24 < free_head && page_head+24 < inuse_head)
					page_head = inuse_head;
			}
			break;
		case 'I':
			inpStr[0] = '\0';
			clear();
			move(0, 0);
			refresh();
			scanf("%c", &inp);
			while (inp != '\r'){
				if (inp == 0x7f){
					if (strlen(inpStr)) // if string is not 0
						inpStr[strlen(inpStr)-1] = '\0';
						clear();
						printw("%s", inpStr);
				}
				else {
					inpStr[strlen(inpStr)+1] = '\0';
					inpStr[strlen(inpStr)] = inp;
					printw("%c", inp);
					
				}
				refresh();
				scanf("%c", &inp);
			}
			insert(inuse_head, inpStr);
			break;
		case 'D':
			delete(inuse_head);
			break;
		case 'S':
			save(argv[1]);
			break;
		default:
			break;
		}
		clear();
	}
	endwin();
}