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

#include<stdio.h> // for printf(3)
#include<stdarg.h> // for va_start(3), va_arg(3), va_end(3)
#include<stdlib.h> // for EXIT_SUCCESS

/*
 * This example shows how to code a function that receives a variable
 * number of arguments and iterates them.
 *
 * This example was stolen shamelessly from the stdarg man page.
 *
 * Notes:
 * - va_arg only returns promoted types (these are the widest possible types).
 * This means that if you want a char, get an int and cast it.
 * Same for float <-> double and the like.
 */
void trace(const char *fmt, ...) {
	va_list ap;
	int d;
	char c, *s;
	float f;
	va_start(ap,fmt);
	while(*fmt) {
		switch (*fmt++) {
			case 'f': /* float */
				f = (double)va_arg(ap, double);
				printf("float %f\n", f);
				break;
			case 's': /* string */
				s = va_arg(ap, char *);
				printf("string %s\n", s);
				break;
			case 'd': /* int */
				d = va_arg(ap, int);
				printf("int %d\n", d);
				break;
			case 'c': /* char */
			/* need a cast here since va_arg only
			* takes fully promoted types */
				c = (char)va_arg(ap, int);
				printf("char %c\n", c);
				break;
		}
	}
	va_end(ap);
}

void trace(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

int main(int argc,char** argv,char** envp) {
	trace("%s %d %f\n", "Hello", 5, 3.14);
	return EXIT_SUCCESS;
}
