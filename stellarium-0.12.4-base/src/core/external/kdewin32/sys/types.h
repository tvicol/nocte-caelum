/* This file is part of the KDE project
   Copyright (C) 2005 Ralf Habacker <ralf.habacker@freenet.de>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KDEWIN_SYS_TYPES_H
#define KDEWIN_SYS_TYPES_H

#ifdef _MSC_BUILD

typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef unsigned int pid_t;
typedef unsigned short mode_t;
typedef int caddr_t;
typedef unsigned int nlink_t;

#else

#include <../include/sys/types.h>

typedef unsigned int uid_t;
typedef unsigned int gid_t;
typedef int caddr_t;

typedef unsigned int nlink_t;

#endif

// uint32
#include <stdint.h>

#endif  // KDEWIN_SYS_TYPES_H

