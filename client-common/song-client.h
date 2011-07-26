/*
 * pommed - song-client.h
 */
#ifndef __MBP_SONG_CLIENT_H__
#define __MBP_SONG_CLIENT_H__


#define SONG_SWITCH_SYSTEM      "/etc/pommed/songswitch"
#define SONG_SWITCH_USER        "/.songswitch"

enum button {
    PLAYPAUSE,
    NEXT,
    PREV
}

int
mbp_get_x_vtnum(Display *dpy);

void
mbp_song_switch(enum button pressed);


#endif /* !__MBP_SONG_CLIENT_H__ */
