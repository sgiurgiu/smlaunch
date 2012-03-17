/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) sergiu 2012 <sergiu@sergiu-pc>
 * 
 * smlaunch is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * smlaunch is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <syslog.h>
#include <pthread.h>

#include "log.h"
#include "utils.h"
#include "client.h"

#define PORT 15016

int sockfd;
struct sockaddr_in serv_addr;

void signal_handler(int sig);
void setup_server();
void start_listening();

int main(int argc, char** argv)
{
	signal(SIGINT,signal_handler);//TODO: OBSOLETE, use sigaction
	signal(SIGTERM,signal_handler);
	openlog("smlaunch",LOG_CONS|LOG_PID,LOG_USER);	
	setlogmask(LOG_DEBUG);
	sm_log(LOG_INFO,"starting %s server\n",argv[0]);

	//TODO: make this an option of some kind
	//actually...not setting this will be an option
	//daemon(0,0);
	
	setup_server();
	start_listening();
	
	exit_program(EXIT_SUCCESS);//most likely will never be reached	
	return (0);
}

void signal_handler(int sig)
{
	sm_log(LOG_DEBUG,"got signal %d, closing...\n",sig);
	exit_program(sig);
}

void setup_server()
{
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0)
	{
		error("ERROR: cannot open socket\n");		
	}
	bzero((char*)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=INADDR_ANY;
	serv_addr.sin_port=htons(PORT);
	if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
	{
		error("ERROR: cannot bind\n");
	}
	if(listen(sockfd,5)<0)
	{
		error("ERROR: cannot listen\n");
	}
}

void start_listening()
{
	int clisockfd;
	socklen_t client_socket_len;
	struct sockaddr_in cli_addr;

	client_socket_len= sizeof(cli_addr);
	while(TRUE)
	{
		sm_log(LOG_DEBUG,"started to listen\n");
		clisockfd = accept(sockfd,(struct sockaddr*)&cli_addr,&client_socket_len);
		if(clisockfd>=0)
		{
			handle_client(clisockfd);
		}
	}
}

