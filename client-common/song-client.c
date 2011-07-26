/*
 * song-client.c -- shared video switch routines for pommed clients
 *
 * Copyright (C) 2007, 2009 Julien BLACHE <jb@jblache.org>
 *
 * Some code below taken from GDM where noted.
 *
 * Some code contributed by Rich Healey <richo@psych0tik.net> and released
 * under the GNU General Public License as noted below.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <errno.h>

#include <linux/vt.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include "song-client.h"


static char *vsw_user = NULL;


/*
 * Get the VT number X is running on
 * (code taken from GDM, daemon/getvt.c, GPLv2+)
 */
int
mbp_get_x_vtnum(Display *dpy)
{
  Atom prop;
  Atom actualtype;
  int actualformat;
  unsigned long nitems;
  unsigned long bytes_after;
  unsigned char *buf;
  int num;

  prop = XInternAtom (dpy, "XFree86_VT", False);
  if (prop == None)
      return -1;

  if (XGetWindowProperty (dpy, DefaultRootWindow (dpy), prop, 0, 1,
			  False, AnyPropertyType, &actualtype, &actualformat,
			  &nitems, &bytes_after, &buf))
    {
      return -1;
    }

  if (nitems != 1)
    {
      XFree (buf);
      return -1;
    }

  switch (actualtype)
    {
      case XA_CARDINAL:
      case XA_INTEGER:
      case XA_WINDOW:
	switch (actualformat)
	  {
	    case 8:
	      num = (*(uint8_t  *)(void *)buf);
	      break;
	    case 16:
	      num = (*(uint16_t *)(void *)buf);
	      break;
	    case 32:
	      num = (*(uint32_t *)(void *)buf);
	      break;
	    default:
	      XFree (buf);
	      return -1;
	  }
	break;
      default:
	XFree (buf);
	return -1;
    }

  XFree (buf);

  return num;
}


/*
 * NOTE: you MUST install a SIGCHLD handler if you use this function
 */
void
mbp_song_switch(enum button pressed)
{
  struct passwd *pw;
  char *vsw = NULL;

  int ret;

  if (vsw_user == NULL)
    {
      pw = getpwuid(getuid());
      if (pw == NULL)
        {
          fprintf(stderr, "Could not get user information\n");

          return;
        }

      vsw_user = (char *) malloc(strlen(pw->pw_dir) + strlen(SONG_SWITCH_USER) + 1);
      if (vsw_user == NULL)
        {
          fprintf(stderr, "Could not allocate memory\n");

          return;
        }

      strncpy(vsw_user, pw->pw_dir, strlen(pw->pw_dir) + 1);
      strncat(vsw_user, SONG_SWITCH_USER, strlen(SONG_SWITCH_USER));
    }

  if (access(vsw_user, R_OK | X_OK) == 0)
    {
      vsw = vsw_user;
    }
  else if (access(SONG_SWITCH_SYSTEM, R_OK | X_OK) == 0)
    {
      vsw = SONG_SWITCH_SYSTEM;
    }
  else
    {
      fprintf(stderr, "No song switch script available\n");
      return;
    }

  ret = fork();
  if (ret == 0) /* exec video switch script */
    {
      switch (pressed) {
        PLAYPAUSE:
          execl(vsw, "songswitch", "toggle", NULL);
          break;
        NEXT:
          execl(vsw, "songswitch", "next", NULL);
          break;
        PREV:
          execl(vsw, "songswitch", "prev", NULL);
          break;
      }

      fprintf(stderr, "Could not song video switch script: %s", strerror(errno));
      exit(1);
    }
  else if (ret == -1)
    {
      fprintf(stderr, "Could not fork: %s\n", strerror(errno));
      return;
    }
}

