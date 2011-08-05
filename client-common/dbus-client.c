/*
 * dbus-client.c -- shared DBus client routines for pommed clients
 *
 * Copyright (C) 2006-2007, 2009 Julien BLACHE <jb@jblache.org>
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

#include <dbus/dbus.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
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

#include "dbus-client.h"


static DBusError *err;
static DBusConnection *conn;

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

/* Method calls */
/* WARNING: method calls are synchronous for now with a 250ms timeout */
int
mbp_call_lcd_getlevel(DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/lcdBacklight",
				     "org.pommed.lcdBacklight", "getLevel");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}

int
mbp_call_kbd_getlevel(DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/kbdBacklight",
				     "org.pommed.kbdBacklight", "getLevel");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}

int
mbp_call_ambient_getlevel(DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/ambient",
				     "org.pommed.ambient", "getLevel");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

#if 0 /* Needs more work, using dispatch & stuff */
  ret = dbus_pending_call_set_notify(pending, cb, NULL, NULL);
  if (!ret)
    {
      printf("Failed to set callback\n");

      dbus_pending_call_unref(pending);

      return -1;
    }
#endif /* 0 */

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}

int
mbp_call_audio_getvolume(DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/audio",
				     "org.pommed.audio", "getVolume");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}

int
mbp_call_audio_getmute(DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/audio",
				     "org.pommed.audio", "getMute");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}

int
mbp_call_video_getvtstate(int vtnum, DBusPendingCallNotifyFunction cb, void *userdata)
{
  DBusMessage *msg;
  DBusPendingCall *pending;

  int ret;

  msg = dbus_message_new_method_call("org.pommed", "/org/pommed/video",
				     "org.pommed.video", "getVTState");

  if (msg == NULL)
    {
      printf("Failed to create method call message\n");

      return -1;
    }

  ret = dbus_message_append_args(msg,
				 DBUS_TYPE_UINT32, &vtnum,
				 DBUS_TYPE_INVALID);
  if (ret == FALSE)
    {
      printf("Failed to add arguments\n");

      dbus_message_unref(msg);

      return -1;
    }

  ret = dbus_connection_send_with_reply(conn, msg, &pending, 250);
  if (ret == FALSE)
    {
      printf("Could not send method call\n");

      dbus_message_unref(msg);

      return -1;
    }

  dbus_connection_flush(conn);

  dbus_message_unref(msg);

  dbus_pending_call_block(pending);

  cb(pending, userdata);

  return 0;
}


/* Error checking, mainly for replies to method calls */

int
mbp_dbus_check_error(DBusMessage *msg)
{
  DBusMessageIter iter;

  char *errmsg;

  if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_ERROR)
    {
      dbus_message_iter_init(msg, &iter);
      dbus_message_iter_get_basic(&iter, &errmsg);

      printf("DBus error: %s\n", errmsg);

      return 1;
    }

  return 0;
}


/* Connection init and cleanup */

static int
bus_add_match(DBusConnection *conn, char *match)
{
  dbus_bus_add_match(conn, match, err);
  dbus_connection_flush(conn);

  if (dbus_error_is_set(err))
    {
      printf("Match error: %s\n", err->message);

      return -1;
    }

  return 0;
}


void
mbp_dbus_cleanup(void)
{
  if (conn != NULL)
    {
      dbus_error_free(err);
      dbus_connection_unref(conn);

      conn = NULL;
    }
}


DBusConnection *
mbp_dbus_init(DBusError *error, unsigned int signals)
{
  err = error;

  dbus_error_init(err);

  conn = dbus_bus_get(DBUS_BUS_SYSTEM, err);
  if (dbus_error_is_set(err))
    {
      printf("DBus system bus connection failed: %s\n", err->message);

      dbus_error_free(err);

      conn = NULL;

      return NULL;
    }

  dbus_connection_set_exit_on_disconnect(conn, FALSE);

  if ((signals & MBP_DBUS_SIG_LCD)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/lcdBacklight',interface='org.pommed.signal.lcdBacklight'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_KBD)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/kbdBacklight',interface='org.pommed.signal.kbdBacklight'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_VOL)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/audioVolume',interface='org.pommed.signal.audioVolume'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_MUTE)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/audioMute',interface='org.pommed.signal.audioMute'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_LIGHT)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/ambientLight',interface='org.pommed.signal.ambientLight'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_EJECT)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/cdEject',interface='org.pommed.signal.cdEject'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_SONG)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/song',interface='org.pommed.signal.song'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  if ((signals & MBP_DBUS_SIG_VIDEO)
      && (bus_add_match(conn, "type='signal',path='/org/pommed/notify/videoSwitch',interface='org.pommed.signal.videoSwitch'") < 0))
    {
      mbp_dbus_cleanup();
      return NULL;
    }

  return conn;
}
