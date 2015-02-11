/*
 * This file is part of the linuxapi package.
 * Copyright (C) 2011-2014 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * linuxapi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * linuxapi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with linuxapi. If not, see <http://www.gnu.org/licenses/>.
 */

#include <firstinclude.h>
#include <stdio.h>	// for printf(3)
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * This example shows that you cannot redefine preprocessor macros
 * without 'undef'ing them.
 */

#define mymacro() printf("this is the first version of the macro\n")
#undef mymacro
// if you disable the 'undef' above then the next line will cause a compile time error
// the following pragma will not help
//#pragma GCC diagnostic ignored "-Werror"
#define mymacro() printf("this is the second version of the macro\n")
int main(int argc, char** argv, char** envp) {
	mymacro();
	return EXIT_SUCCESS;
}