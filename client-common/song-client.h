/*
 * pommed - song-client.h
 */
#ifndef __MBP_SONG_CLIENT_H__
#define __MBP_SONG_CLIENT_H__


#define SONG_SWITCH_SYSTEM      "/etc/pommed/songswitch"
#define SONG_SWITCH_USER        "/.songswitch"

typedef enum {
    PLAYPAUSE,
    NEXT,
    PREV
} button;

void
mbp_song_switch(button pressed);


#endif /* !__MBP_SONG_CLIENT_H__ */
