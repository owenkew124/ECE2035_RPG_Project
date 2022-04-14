#ifndef GRAPHICS_H
#define GRAPHICS_H

#define DGREEN 0x006803

/**
 * Draws the player. This depends on the player state, so it is not a DrawFunc.
 */
 
void draw_player(int u, int v, int key, int run, int keyframe);


/**
 * Takes a string image and draws it to the screen. The string is 121 characters
 * long, and represents an 11x11 tile in row-major ordering (across, then down,
 * like a regular multi-dimensional array). The available colors are:
 *      R = Red
 *      Y = Yellow
 *      G = Green
 *      D = Brown ("dirt")
 *      5 = Light grey (50%)
 *      3 = Dark grey (30%)
 *      Any other character is black
 * More colors can be easily added by following the pattern already given.
 */
void draw_img(int u, int v, const char* img);

/**
 * DrawFunc functions. 
 * These can be used as the MapItem draw functions.
 */
void draw_nothing(int u, int v);
//Draw a wall with the wall sprite
void draw_wall(int u, int v);
//Draw a dark green plant
void draw_plant(int u, int v);

/**
 * Draw the upper status bar.
 */
void draw_upper_status(int h);

/**
 * Draw the lower status bar.
 */ 
void draw_lower_status(int x, int y);

/**
 * Draw the border for the map.
 */
void draw_border();

//draw a monster
void draw_mon(int u, int v);
//draw a portal
void draw_portal(int u, int v);
//draw a npc
void draw_npc(int u, int v);
//draw a door
void draw_door(int u, int v);
//draw a chest
void draw_chest(int u, int v);
//draw the side of the bridge
void draw_bridge_wall(int u, int v);
//draw the main part of the bridge
void draw_bridge(int u, int v);
//draw a river
void draw_river(int u, int v);

#endif // GRAPHICS_H