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
#include<stdlib.h> // for EXIT_SUCCESS, rand(3)

int main(int argc,char** argv,char** envp) {
	int a;
	int i;
	printf("the address of a is %p\n",&a);
	printf("the address of i is %p\n",&i);
	int* pi=&a+1;
	printf("pi is %p\n",pi);
	/*
	if(rand()<RAND_MAX) {
		pi+=1;
		printf("pi is %p\n",pi);
	}
	*/
	for(i=0;i<100;i++) {
		//printf("i is %d\n",i);
		//printf("*pi is %d\n",*pi);
		if(i==10) {
			*pi=700;
		}
		//__sync_synchronize();
		//asm volatile("":::"memory");
		asm volatile("":"=g" (i)::);
	}
	printf("after the loop\n");
	printf("i is %d\n",i);
	printf("*pi is %d\n",*pi);
	return EXIT_SUCCESS;
}