#include "map.h"

#include "globals.h"
#include "graphics.h"

/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 * In this file you need to define how the map will be structured. IE how will
 * you put values into the map, pull them from the map. Remember a "Map" variable
 * is a hashtable plus two ints (see below) 
 * You will have more than one map variable, one will be the main map with it's own hashtable.
 * Then you'll have a second map with another hashtable
 * You should store objects into the hashtable with different properties (spells
 * etc)
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map[2];
static int active_map;


/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    Map* curr = get_active_map();
    unsigned xy = X * (curr->h) + Y;
    return xy;
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{
    unsigned hash = key % (SIZE);      //key mod arbitrarily picked 250      
    return hash;
    
}
//creates two maps and their own hash tables
void maps_init()
{
    map[0].items = createHashTable(map_hash, SIZE);
    map[0].w = 52;
    map[0].h = 52;
    map[1].items = createHashTable(map_hash, SIZE);
    map[1].w = 20;
    map[1].h = 20;
}
//returns the a pointer to the actual map
Map* get_active_map()
{
    return &map[active_map];
}
//returns the number representation of the map
int get_active_map_num()
{
    return active_map;
}
//sets which map is active
Map* set_active_map(int m)
{
    active_map = m;
    return &map[m];
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}
//gets the width of the current map
int map_width()
{
    Map *map = get_active_map();
    return map->w;
}
//gets the height of the current map
int map_height()
{
    Map *map = get_active_map();
    return map->h;
}
//gets the area of the current map
int map_area()
{
    int area = map_width() *  map_height();
    return area;
}
//finds the block immediately to the north of the given coordinates
MapItem* get_north(int x, int y)
{
    unsigned northKey = XY_KEY(x,y-1);
    MapItem* north = (MapItem*) getItem(get_active_map()->items, northKey);
    return north;
}
//finds the block immediately to the south of the given coordinates
MapItem* get_south(int x, int y)
{
    unsigned southKey = XY_KEY(x,y+1);
    MapItem* south = (MapItem*) getItem(get_active_map()->items, southKey);
    return south;
}
//finds the block immediately to the east of the given coordinates
MapItem* get_east(int x, int y)
{
    unsigned eastKey = XY_KEY(x+1,y);
    MapItem* east = (MapItem*) getItem(get_active_map()->items, eastKey);
    return east;
}
//finds the block immediately to the west of the given coordinates
MapItem* get_west(int x, int y)
{
    unsigned westKey = XY_KEY(x-1,y);
    MapItem* west = (MapItem*) getItem(get_active_map()->items, westKey);
    return west;
}
//finds the block immediately to the north of the given coordinates
MapItem* get_here(int x, int y)
{
    unsigned hereKey = XY_KEY(x,y);
    MapItem* here = (MapItem*) getItem(get_active_map()->items, hereKey);
    return here;
}


void map_erase(int x, int y)
{
    unsigned deleteKey = XY_KEY(x,y);
    deleteItem(get_active_map()->items,deleteKey);
}
//creates a new wall block
void add_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = draw_wall;
        w1->walkable = false;
        w1-> above = 0;
        w1->data = NULL;
        
        
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        void* val = insertItem(get_active_map()->items, key, w1);
        if (val) free(val); // If something is already there, free it
    }
}
//creates a new plant block
void add_plant(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//creates a new monster block
void add_mon(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = MON;
    w1->draw = draw_mon;
    w1->walkable = false;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//creates a new portal block
void add_portal(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PORTAL;
    w1->draw = draw_portal;
    w1->walkable = false;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//creates a new npc block
void add_npc(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = NPC;
    w1->draw = draw_npc;
    w1->walkable = false;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//moves the npc in a random direction
int update_npc(int x, int y, int move)
{
    /**
    if (move <= 4) {
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x + 1, y);
        return 3;
    } else if (move > 4 && move <= 8) {
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x, y + 1);
        return 2;
    } else if (move > 8 && move <= 12) {
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x - 1, y);
        return 4;
    } else if (move > 12) {
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x, y - 1);
        return 1;
    }
    return 0;
    **/
    int random = rand() % 50; //get a random number between 1 and 50
    //if its 0-3 then the npc moves. If it's not he stays put. Made this way to make it easier to talk to him
    if (random == 0 && get_north(x,y)->walkable) { //if random = 0 check if he can walk north and return 1
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x, y - 1);
        return 1;
    } else if (random == 1 && get_south(x,y)->walkable) {//if random = 1 check if he can walk south and return 2
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x, y + 1);
        return 2;
    } else if (random == 2 && get_east(x,y)->walkable) {//if random = 2 check if he can walk east and return 3
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x + 1, y);
        return 3;
    } else if (random == 3 && get_west(x,y)->walkable) {//if random = 3 check if he can walk west and return 4
        deleteItem(get_active_map()->items, XY_KEY(x, y));
        add_npc(x - 1, y);
        return 4;
    }
    return 0; //if he doesn't move return 0
}
//creates a new door block
void add_door(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DOOR;
    w1->draw = draw_door;
    w1->walkable = false;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//creates a new bridge wall block
void add_bridge_wall(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
        {
            MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
            w1->type = BWALL;
            w1->draw = draw_bridge_wall;
            w1->walkable = false;
            w1-> above = 1;
            w1->data = NULL;
            
            
            unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
            void* val = insertItem(get_active_map()->items, key, w1);
            if (val) free(val); // If something is already there, free it
    }
}
//creates a new bridge block
void add_bridge(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
        {
            MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
            w1->type = BRIDGE;
            w1->draw = draw_bridge;
            w1->walkable = true;
            w1-> above = 1;
            w1->data = NULL;
            
            
            unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
            void* val = insertItem(get_active_map()->items, key, w1);
            if (val) free(val); // If something is already there, free it
    }
}
//creates a new river block
void add_river(int x, int y, int dir, int len)
{
    for(int i = 0; i < len; i++)
        {
            MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
            w1->type = RIVER;
            w1->draw = draw_river;
            w1->walkable = false;
            w1-> above = 0;
            w1->data = NULL;
            
            
            unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
            void* val = insertItem(get_active_map()->items, key, w1);
            if (val) free(val); // If something is already there, free it
    }
}
//creates a new chest block
void add_chest(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = CHEST;
    w1->draw = draw_chest;
    w1->walkable = false;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}
//creates a new trap block
void add_trap(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = TRAP;
    w1->draw = draw_bridge; //it looks exactly the same as the bridge it will be on
    w1->walkable = true;
    w1-> above = 0;
    w1->data = NULL;
    
    
    void* val = insertItem(get_active_map()->items, XY_KEY(x, y), w1);
    if (val) free(val); // If something is already there, free it
}