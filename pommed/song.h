/*
 * pommed - song.h
 */

#ifndef __SONG_H__
#define __SONG_H__

enum button {
    PLAYPAUSE,
    NEXT,
    PREV
};

void
song_next(void);

void
song_prev(void);

void
song_playpause(void);

int
song_vt_active(int vt);


#endif /* !__SONG_H__ */

