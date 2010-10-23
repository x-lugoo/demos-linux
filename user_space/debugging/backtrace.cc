#include<execinfo.h> // for backtrace(3), backtrace_symbols(3), backtrace_symbols_fd(3)
#include<signal.h> // for signal(2)
#include<stdio.h> // for perror(3), fprintf(3)
#include<stdlib.h> // for exit(3)

#include"us_helper.hh"

/*
 *      This exapmle shows how to obtain a stack trace for various purposes (mostly
 *      for 'self debugging...'...).
 *
 *      Some notes:
 *      - You DO NOT have to compile with debugging information. You will see any
 *      dynamic symbols (for all you DLL lovers - this means every exported symbol
 *      of the DLL which in linux means everything you didn't put 'static' on...
 *      - The thing is that dynamic symbols (ones which are exported) are not enough.
 *      What about functions inside my code which are static and are on the stack? They
 *      will not be in the trace.
 *      - This is why you need to compile with -rdynamic to get those symbols as well.
 *      - How does the backtrace function work? It gets the return address from the stack
 *      and tries to figure out from which function it came. The return address is NOT the
 *      exact pointer to a know function but a pointer to some position within the function
 *      (from whence you were called) but it is within the functions bounds.
 *      - It repeats this process going up the stack.
 *      - If you do this from within a signal handler you may also see the signal handling
 *      stack concatenated with the real stack depending on how you registered your signal
 *      hanlder (with or without it's own stack, see sigaction(2)...). If you registered it
 *      with it's own stack you will not see the real stack at all...
 *      - This example dumps stack traces both from regular context (before a fault) and from
 *      a fault handler.
 *      - Please note that it is problematic to dump a stack trace from within a fault handler
 *      since the underlying libc functions (backtrace_*) are allocating memory via malloc(3)
 *      which may be corrupt at that point.
 *
 *              Mark Veltzer
 *
 * EXTRA_LIBS=-rdynamic
 *
 * TODO:
 * 	- do cxx name demangling.
 * 	- move to registering the handler via sigaction and show how to specify it's own stack
 * 	and the effect of this on the result.
 * 	- show in another example what happnes if we do a segfault inside a segfault handler.
 * 	- do the recommendations from the gilad ben yossef slide show:
 * 		pipe the data to another process standing by,
 * 		malloc and heap poisoning
 * 		and more.
 */

/*
 *      Please keep this a C function, as it should be usable for C infrastructure as well...
 */
void print_trace(bool full) {
	unsigned int nptrs;
	const unsigned int MAX_FRAMES = 100;
	void               *buffer[MAX_FRAMES];

//	char **strings;
//	unsigned int j;
	nptrs = backtrace(buffer, MAX_FRAMES);
	if(!full) {
		nptrs-=2;
	}
	fprintf(stderr, "backtrace() returned %d addresses\n", nptrs);
	backtrace_symbols_fd(buffer, nptrs, fileno(stderr));

	/*
	 *      strings = backtrace_symbols(buffer, nptrs);
	 *      if (strings == NULL) {
	 *              perror("backtrace_symbols");
	 *              exit(EXIT_FAILURE);
	 *      }
	 *      for (j = 0; j < nptrs; j++)
	 *              fprintf(stderr,"%s\n", strings[j]);
	 *      free(strings);
	 */
}

// this is not really required since we are taking the address of this functino
// which will cause the compiler to make it a "real" one...
//void print_trace(void) __attribute__ ((__noinline__));

/*
 *      static old handler so that we would call the old handling code...
 */
typedef void (*sighandler_t)(int);
static sighandler_t old_handler;

/*
 *      Wrapper for signal handling
 */
//void print_trace_sighandler(int sig, siginfo_t * siginf, void * context) {
void print_trace_sighandler(int sig) {
	fprintf(stderr, "got sig %d\n", sig);
	print_trace(true);
	(*old_handler)(sig);
}

/*
 *      Signal handler registration function
 */
void trace_register(void) {
	old_handler = signal(SIGSEGV, print_trace_sighandler);
	if (old_handler == SIG_ERR) {
		perror("could not register signal...");
		exit(1);
	}

	/*
	 *      struct sigaction sa;
	 *      memset(sa,0,sizeof(struct sigaction));
	 */
}

/*
 *      Simple function that generates a segmentation fault...
 */
void do_fault(void) {
	char *p = (char *)0;

	p[0] = 0;
}

/*
 *      I use several ways to try to force the compiler not to flatten the next function:
 *      1. use asm("").
 *      2. __attribtue__ (( __noinline__ ));
 *      3. The value passed is from the user and not hardcoded and so the compiler cannot
 *      unroll the function calls.
 *      4. complicating the function code with an integer accumulator and recursion.
 *      finally it worked but I think it is number (3) that made it happen...
 *      Need to investigate this further...
 */
int rec_func(unsigned int ncalls) {
	static unsigned int counter=0;
	DEBUG("another call %d",counter++);
//	asm ("");
	if (ncalls > 1) {
		print_trace(false);
		rec_func(ncalls - 1);
	} else {
		//print_trace();
		do_fault();
	}
	return(0);
}
//int rec_func(unsigned int ncalls, unsigned int ret) __attribute__((__noinline__));

int main(int argc, char **argv, char **envp) {
	trace_register();

	unsigned int num;
	if (argc > 1) {
		num = atoi(argv[1]);
	} else {
		num = 3;
	}
	rec_func(num);
	return(0);
}
