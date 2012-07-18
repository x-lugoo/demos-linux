/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include<firstinclude.h>
#include<stdio.h> // for printf(3)
#include<string.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h> // for sleep(3)
#include<strings.h> // for bzero(3)
#include<signal.h> // for signal(2)
#include<sys/mman.h> // for mprotect(2)
#include<unistd.h> // for getpagesize(2)

#include<us_helper.h>

/*
* This is an example that shows that you cannot alter code, which is protected
* via the MMU but you can do it if you mprotect it differently...
*/

const int times=10;

/*
* This is the function we want to morph...
*/
static void function(void) __attribute__ ((noinline));
static void function(void) {
	asm("");
	//printf("times is %d\n",times);
	for(int i=0;i<times;i++) {
		printf("i is %d\n",i);
	}
}

/*
* A function that translates an address to it's page boundary
*/
void* page_adr(void* adr) {
	unsigned int iptr=(unsigned int)adr;
	return (void*)(iptr-iptr%getpagesize());
}

/*
* A function to dump a certain portion of the memory for debugging purposes...
*/

void debug(void* ptr) {
	char* p=(char*)ptr;
	for(int i=0;i<40;i++) {
		printf("p[%d]=%d\n",i,p[i]);
	}
}

/*
* A function that scans a memory space and returns the pointer to a cell with a certain
* value...
*/
char* find_cell(void* ptr,char val) {
	char* p=(char*)ptr;
	while(*p!=val) {
		p++;
	}
	return p;
}

/*
* This is a signal handler to handle the segmentation faults we will generate...
*/
void segv_handler(int sig) {
	fprintf(stderr,"in segv_handler, changing protection for the page...\n");
	CHECK_NOT_M1(mprotect(page_adr((void*)function),getpagesize(),PROT_READ|PROT_WRITE|PROT_EXEC));
}

int main(int argc,char** argv,char** envp) {
	// call the function to see what it is doing...
	function();
	// lets install our own SIGSEGV signal handler so that we won't crash...
	CHECK_NOT_SIGT(signal(SIGSEGV,segv_handler),SIG_ERR);
	// find the cell where the number 'times' is writen
	// if you are in gcc 4.6 then search for times
	// if you are in gcc 4.5 then search for times-1
	// if you are in gcc 4.4 then search for times
	// p has to be volatile or else the assignment to it will not happen
	volatile char* p=find_cell((void*)function,times);
	fprintf(stderr,"address of function is %p\n",function);
	fprintf(stderr,"address of p is %p\n",p);
	//fprintf(stderr,"*function is %c\n",*(char*)function);
	//*(char*)(function)=5;
	//fprintf(stderr,"*function is %c\n",*(char*)function);
	// change the value of the function, this will generate a SIGSEGV
	// but we will use mprotect to get around that...
	// we could have used mprotect from the begining...
	*p=5;
	// lets call the function again to see that it has changed it's behaviour...
	function();
	return EXIT_SUCCESS;
}
