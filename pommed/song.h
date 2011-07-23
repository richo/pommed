/* 
 * pommed - song.h
 */

#ifndef __SONG_H__
#define __SONG_H__

void
song_playpause(void);

void
song_next(void);

void
song_prev(void);

void
song_exec(char *cmd);

void
song_fix_config(void);

#endif /* !__SONG_H__ */
