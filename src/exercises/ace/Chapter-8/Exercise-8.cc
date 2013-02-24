/*
 * This file is part of the linuxapi project.
 * Copyright (C) 2011-2013 Mark Veltzer <mark.veltzer@gmail.com>
 *
 * The linuxapi package is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * The linuxapi package is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with the GNU C Library; if not, write to the Free
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 */

#include <firstinclude.h>
#define ACE_NTRACE 1
#include <ace/streams.h>
#include <ace/Log_Msg.h>
#include <stdlib.h>	// for EXIT_SUCCESS

/*
 * EXTRA_COMPILE_CMDS=pkg-config --cflags ACE
 * EXTRA_LINK_CMDS=pkg-config --libs ACE
 */

void FunB() {
	ACE_TRACE(ACE_TEXT("FunB"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("DEBUG: pid=%P Line:%l File=%N\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: pid=%P Line:%l File=%N\n")));
	ACE_DEBUG((LM_NOTICE, ACE_TEXT("NOTICE: pid=%P Line:%l File=%N\n")));
}

void FunA() {
	ACE_TRACE(ACE_TEXT("FunA"));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("DEBUG: pid=%P Line:%l File=%N\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: pid=%P Line:%l File=%N\n")));
	ACE_DEBUG((LM_NOTICE, ACE_TEXT("NOTICE: pid=%P Line:%l File=%N\n")));
	FunB();
}

int ACE_TMAIN(int argc, ACE_TCHAR** argv) {
	ACE_LOG_MSG->open(argv[0], ACE_Log_Msg::SYSLOG, ACE_TEXT("ACE-Course"));
	ACE_DEBUG((LM_INFO, ACE_TEXT("%I a - Eytan Carmiel was here\n")));
	ACE_DEBUG((LM_DEBUG, ACE_TEXT("%I b - Eytan Carmiel Can't be here\n")));
	ACE_DEBUG((LM_INFO, ACE_TEXT("argv[0]=%s\n"), argv[0]));
	ACE_DEBUG((LM_INFO, ACE_TEXT("INFO: pid=%P Line:%l File=%N\n")));
	ACE_LOG_MSG->priority_mask(LM_DEBUG | LM_INFO | LM_NOTICE | LM_TRACE, ACE_Log_Msg::PROCESS);
	ACE_TRACE(ACE_TEXT("main"));
	for(int i=0; i<2; i++) {
		FunA();
	}
	ACE_LOG_MSG->open(argv[0]);
	ACE_DEBUG((LM_INFO, ACE_TEXT("End of Program\n")));
	return EXIT_SUCCESS;
}
