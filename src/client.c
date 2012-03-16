/*
 * client.c
 *
 * Copyright (C) 2012 - sergiu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <pthread.h>

#include "log.h"
#include "utils.h"

#define MAX_PATH 1024

#define START_VIDEO 1

void client_thread(void* clientfd_pointer);
void play_file(const char* file);
void execute_command(unsigned char command,int clientfd);

void client_thread(void* clientfd_pointer)
{
	int clientfd=*((int*)clientfd_pointer);
	unsigned char command=-1;
	
	int readno=read(clientfd,&command,sizeof(unsigned char));
	if(readno<0)
	{
		error("ERROR reading data from client");
	}
	execute_command(command,clientfd);	
	close(clientfd);
}

void execute_command(unsigned char command,int clientfd)
{
	int fileLen=-1; 
	char* path[MAX_PATH];
	int readno;
	switch(command)
	{
		case START_VIDEO:
			readno=read(clientfd,&fileLen,sizeof(int));
			if(readno<0)
			{
				error("ERROR reading data from client");
			}
			readno=read(clientfd,path,fileLen+1);
			if(readno<0)
			{
				error("ERROR reading data from client");
			}
			path[fileLen]=NULL;
			play_file((const char*)path);
			break;
		default:
			sm_log(LOG_DEBUG,"Unkown command %d\n",command);
			break;
	}	
}

void handle_client(int clientFd)
{
	sm_log(LOG_DEBUG,"we got a connection\n");
	pthread_t thread;
	int res=pthread_create(&thread,NULL,
	                       (void*)&client_thread,
	                       (void*)&clientFd);
	if(res)
	{
		error("ERROR: cannot create client thread...");
	}
}

void play_file(const char* file)
{
	sm_log(LOG_DEBUG,"will be playing file %s\n",file);
	pid_t child=fork();	
	if(child==0)
	{
		//the child here
		const char* player="/usr/bin/gnome-mplayer";//TODO: get this in some option of some kind
		char *const args[]={(char*)player,"-q","--fullscreen",(char*)file,(char*)NULL};
		//int res=execl(player,player,file,(char*)NULL);
		int res=execv(player,args);		
		if(res==-1)
		{
			error("ERROR: unable to launch process ");
		}
	}
	else
	{
		//the parent...just go away, maybe log something
	}
}