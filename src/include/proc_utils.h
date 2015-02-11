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

#ifndef __proc_utils_h
#define __proc_utils_h

/*
 * This is a collection of helper function to help with dealing
 * with the Linux /proc filesystem API
 */

/* THIS IS A C FILE, NO C++ here */

#include <firstinclude.h>
#include <sys/types.h>	// for getpid(2), pid_t:type
#include <unistd.h>	// for getpid(2), pid_t:type
#include <stdio.h>	// for snprintf(3), printf(3), fopen(3), fgets(3), fclose(3), feof(3), FILE:type, getline(3), sscanf(3)
#include <proc/readproc.h>	// for get_proc_stats(3), proc_t, look_up_our_self(3)
#include <sys/time.h>	// for getrusage(2), rusage:struct
#include <sys/resource.h>	// for getrusage(2), rusage:struct
#include <string.h>	// for strstr(3)
#include <multiproc_utils.h>	// for my_system()
#include <err_utils.h>	// for CHECK_NOT_NULL_FILEP(), CHECK_NOT_NULL(), CHECK_NOT_M1(), CHECK_ZERO_ERRNO(), CHECK_ASSERT()

/*
 * Function to print the resident memory of the current process as
 * well as the number of minor page faults.
 */
static inline void getrusage_show_vmem() {
	struct rusage usage;
	CHECK_NOT_M1(getrusage(RUSAGE_SELF, &usage));
	printf("usage.ru_maxrss=%lu\n", usage.ru_maxrss);
	printf("usage.ru_minflt=%lu\n", usage.ru_minflt);
}

/*
 * Function to print the current processes /proc maps file
 */
static inline void proc_print_mmap(const char *filter) {
	pid_t pid=getpid();
	if (filter==NULL) {
		my_system("cat /proc/%d/maps", pid);
	} else {
		my_system("cat /proc/%d/maps | grep %s", pid, filter);
	}
}

/*
 * Function to print the current processes /proc maps file
 */
static inline void proc_print_mmap_self() {
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/proc/self/maps", "r"));
	while ((read = getline(&line, &len, fp)) != -1) {
		printf("%s", line);
	}
	if(!feof(fp)) {
		CHECK_NOT_M1(read);
	}
	free(line);
	CHECK_ZERO_ERRNO(fclose(fp));
}

/*
 * Function to get a certain line from /proc/self/maps
 */
static inline void proc_get_self_filter(const char* filter, char* out, size_t size) {
	char *line = NULL;
	size_t len = 0;
	ssize_t read;
	bool found=false;

	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/proc/self/maps", "r"));
	while ((read = getline(&line, &len, fp)) != -1) {
		if(strstr(line, filter)) {
			found=true;
			strncpy(out, line, size);
		}
	}
	if(!feof(fp)) {
		CHECK_NOT_M1(read);
	}
	free(line);
	CHECK_ZERO_ERRNO(fclose(fp));
	CHECK_ASSERT(found);
}

/*
 * Function to print the current processes /proc maps file with filter
 */
static inline void proc_print_mmap_self_filter(const char* filter) {
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	FILE* fp=CHECK_NOT_NULL_FILEP(fopen("/proc/self/maps", "r"));
	while ((read = getline(&line, &len, fp)) != -1) {
		if(strstr(line, filter)) {
			printf("%s", line);
		}
	}
	if(!feof(fp)) {
		CHECK_NOT_M1(read);
	}
	free(line);
	CHECK_ZERO_ERRNO(fclose(fp));
}

/*
 * Function to print the current processes /proc maps only for the exe itself
 */
static inline void proc_print_mmap_self_only() {
	extern const char* __progname;
	proc_print_mmap_self_filter(__progname);
}

/*
 * Print memory stats for the current process
 */
static inline void proc_print_mem_stats(pid_t pid) {
	proc_t myproc;
	get_proc_stats(pid, &myproc);
	printf("size is %ld, min_flt is %ld, state is %c\n", myproc.rss, myproc.min_flt, myproc.state);
}

/*
 * Print memory stats for the current process
 */
static inline void proc_print_mem_stats_self(void) {
	proc_t myproc;
	look_up_our_self(&myproc);
	printf("size is %ld, min_flt is %ld\n", myproc.rss, myproc.min_flt);
}

/*
 * Print the name of any process according to pid
 */
static inline void my_print_process_name_from_proc(pid_t pid) {
	my_system("cat /proc/%d/comm", pid);
}

/*
 * Print the name of our process via /proc
 */
static inline void my_print_process_name_from_proc_self() {
	my_system("cat /proc/self/comm");
}

/*
 * Print the current threads name from /proc
 */
static inline void print_thread_name_from_proc() {
	my_system("cat /proc/%d/comm", gettid());
}

/*
 * get the current stack from /proc
 */
static inline void* proc_get_start_stack() {
	proc_t myproc;
	look_up_our_self(&myproc);
	return (void*)myproc.start_stack;
}

/*
 * get the stack address from /proc/[pid]/maps
 */
void* proc_get_stack_adr() {
	const unsigned int bufsize=1024;
	char buf[bufsize];
	proc_get_self_filter("[stack]", buf, bufsize);
	unsigned long adr_start, adr_end;
	CHECK_ASSERT(sscanf(buf, "%lx-%lx ", &adr_start, &adr_end)==2);
	return (void*)adr_end;
}

/*
 * get stack info from /proc/[pid]/maps
 */
void proc_get_stack_info(unsigned long* adr_start, unsigned long* adr_end) {
	const unsigned int bufsize=1024;
	char buf[bufsize];
	proc_get_self_filter("[stack]", buf, bufsize);
	CHECK_ASSERT(sscanf(buf, "%lx-%lx ", adr_start, adr_end)==2);
}

#endif	/* !__proc_utils_h */