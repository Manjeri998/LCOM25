#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <minix/sysutil.h>
#include <lcom/lcf.h>

typedef struct {
int x, y; 
int width, height; 
int xspeed, yspeed; 
unsigned char *map;
} Sprite;

Sprite *create_sprite(xpm_map_t pic, int x, int y, int xspeed, int yspeed);
void destroy_sprite(Sprite *sp);

#endif
