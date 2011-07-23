/*
 * pommed - Apple laptops hotkeys handler daemon
 *
 * Copyright (C) 2006-2007 Julien BLACHE <jb@jblache.org>
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <limits.h>

#include <errno.h>

#include <syslog.h>

#include "pommed.h"
#include "conffile.h"
#include "song.h"

void
song_playpause(void)
{
  if (!song_cfg.enabled)
    return;

  song_exec(song_cfg.playpause_cmd);
}

void
song_next(void)
{
  if (!song_cfg.enabled)
    return;

  song_exec(song_cfg.next_cmd);
}

void
song_prev(void)
{
  if (!song_cfg.enabled)
    return;

  song_exec(song_cfg.prev_cmd);
}

void
song_exec(char *cmd)
{
    // TODO Allocate as much space as needed instead of just "lots"
  char *song_argv[30];
  char *search = " ";
  char *buf = malloc(sizeof(cmd));
  int it = 0;
  long max_fd;
  int fd;
  int ret;

  strcpy(buf, cmd);
  if ((song_argv[0] = strtok(buf, search)))
    {
      do
      {
        it++;
      } while ((song_argv[it] = strtok(NULL, search)));
    }
  else
    {
      free(buf);
      return;
    }

  ret = fork();
  if (ret == 0) /* exec object */
    {
      max_fd = sysconf(_SC_OPEN_MAX);

      if (max_fd > INT_MAX)
        max_fd = INT_MAX;

      for (fd = 3; fd < max_fd; fd++)
        close(fd);

      execvp(song_argv[0], song_argv);

      logmsg(LOG_ERR, "Could not execute %s: %s", song_argv[0], strerror(errno));
      exit(1);
    }
  else if (ret == -1)
    {
      logmsg(LOG_ERR, "Could not fork: %s", strerror(errno));
    }
  else
    {
      waitpid(ret, &ret, 0);
      if ((WIFEXITED(ret) == 0) || (WEXITSTATUS(ret) != 0))
      	{
      	  logmsg(LOG_INFO, "%s failed", song_argv[0]);
  	    }
     }
  free(buf);
  return;
}

void
song_fix_config(void)
{
    /* TODO */
    return;
}
