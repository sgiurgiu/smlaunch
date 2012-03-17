/*
 * player.c
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
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <syslog.h>
#include <string.h>

#include "utils.h"
#include "player.h"
#include "log.h"

DBusConnection *connection=NULL;
void send_dbus_message(pid_t pid,const char* msg);

void player_init()
{
	if(!connection)
	{
		DBusError error;
		dbus_error_init (&error);
		connection = dbus_bus_get (DBUS_BUS_SESSION, &error);
		if (!connection)
		{
		    sm_log (LOG_ERR, "Connection to D-BUS daemon failed: %s\n", error.message);
		    //deallocate error message
		    dbus_error_free (&error);
		}
	}
}

void player_pause(pid_t pid)
{
	send_dbus_message(pid,"Pause");
}

void player_resume(pid_t pid)
{
	send_dbus_message(pid,"Play");
}

pid_t player_play(const char* file, pid_t player_pid)
{
	if(player_pid!=-1)
	{
		player_quit(player_pid);//should just send a message via dbus
	}
	
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
		return -1;//will never be reached
	}
	else
	{
		return child;//we can only have 1 player running at a time
	}	
}

void player_quit(pid_t pid)
{
	if(pid>0)
	{
		if(connection)
		{
			send_dbus_message(pid,"Quit");
		}
		else
		{
			kill(pid,SIGTERM);
		}
	}
}

void send_dbus_message(pid_t pid,const char* msg)
{
	if(connection)
	{
		char pid_path[254];
		bzero(pid_path,254);
		sprintf(pid_path,"/pid/%d",pid);
		DBusMessage *message;
		message=dbus_message_new_signal (pid_path,"com.gnome.mplayer",msg);
		dbus_connection_send(connection,message,NULL);
		dbus_message_unref (message);
	}
}