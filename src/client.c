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
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "log.h"
#include "utils.h"
#include "player.h"

#define MAX_PATH 1024

#define START_VIDEO 1
#define LIST_FILES 2
#define PAUSE 3
#define RESUME 4


void client_thread(void* clientfd_pointer);
void play_file(const char* file);
void execute_command(unsigned char command,int clientfd);
void list_files(const char* path,int clientfd,int level);

pid_t player_pid=-1;

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
		case LIST_FILES:
			list_files("/mnt/media/Movies",clientfd,0);		
			break;
		case PAUSE:
			player_pause(player_pid);
			break;
		case RESUME:
			player_resume(player_pid);
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
	
	player_pid=player_play(file,player_pid);//we can only have 1 player running at a time	
}

void list_files(const char* path,int clientfd,int level)
{
	struct dirent *dp;
	size_t full_cpath_len=0;
	char* full_cpath=0;
	//sm_log(LOG_DEBUG,"listing %s\n",path);
	size_t pathlen=strlen(path);
	DIR *dir=opendir(path);	
	while ((dp=readdir(dir)) != NULL) {
		unsigned char type=dp->d_type;//Linux and BSD systems
		switch(type)
		{
			case DT_DIR:
				if(!strcmp(".",dp->d_name) ||!strcmp("..",dp->d_name))
				{
					continue;
				}
				//to account for 1 / char
				full_cpath_len=pathlen+strlen(dp->d_name)+2;
				full_cpath=(char*)malloc(full_cpath_len*sizeof(char));
				sprintf(full_cpath,"%s/%s",path,dp->d_name);
				list_files (full_cpath,clientfd,level+1);
				free(full_cpath);
				break;
			case DT_REG:
				full_cpath_len=pathlen+strlen(dp->d_name)+2;
				full_cpath=(char*)malloc(full_cpath_len*sizeof(char));
				sprintf(full_cpath,"%s/%s",path,dp->d_name);
				write(clientfd,&full_cpath_len,sizeof(size_t));
				write(clientfd,full_cpath,full_cpath_len);
				//printf("%s\n", full_cpath);
				free(full_cpath);
				break;
		};		
	}
	closedir(dir);
	full_cpath_len=0;
	if(level==0)
	{
		write(clientfd,&full_cpath_len,sizeof(size_t));
	}
}