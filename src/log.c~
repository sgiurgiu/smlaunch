/*
 * log.c
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

#include <syslog.h>
#include <stdio.h>
#include <stdarg.h>
#include "log.h"

#define DEBUG 1

void sm_log(int priority, const char* format, ...)
{
	va_list arguments;
	va_start(arguments, format);
#ifdef DEBUG	
	vprintf (format, arguments);
#endif
	vsyslog(priority,format, arguments);
	va_end(arguments);
}