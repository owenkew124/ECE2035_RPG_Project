// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "speech.h"

GameInputs inputs;
int action;
int init;
int game_over;
int game_over_lost;
int npcX;
int npcY;
int npcM;
int fight;
int original;


// Functions in this file
int get_action (GameInputs inputs);
int update_game (int action);
void draw_game (int init);
void init_main_map ();
int main ();

int go_up(int x, int y);
int go_down(int x, int y);
int go_left(int x, int y);
int go_right(int x, int y);
int check(int x, int y);
void init_second_map();
void draw_opening();


/**
 * The main game state. Must include Player locations and previous locations for
 * drawing to work properly. Other items can be added as needed.
 */
 #define NO_SPELL 0
 #define FIRE_SPELL 1
 #define WATER_SPELL 2
struct {
    int x,y;    // Current locations
    int px, py; // Previous locations
    int has_key;
    // You can add other properties for the player here
    int god_mode;
    unsigned int color; //color for when he used to be a block
    int quest_begin; //whether the quest has begun
    int quest_done; //whether the quest is done
    int is_running; //whether the player is running
    int health; //the health of the player
    int spell_type; //Unused
    int anim; //the animation frame of the player
    int mean; //keeps track of how many hostile responses the player has made
    int talk; //Unused
    int fighting; //who the player is fighting
} Player;

/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6
#define GOD_MODE 7
#define RUN_BUTTON 8
#define YELLOW 0xFFFF00
#define ENCOUNTER 9
int get_action(GameInputs input)
{
    if (fight == 1)   return ENCOUNTER; //if the player is in a fight do whats under encounter
    if (!(input.b4)) return GOD_MODE; //if the player presses b4, activate god mode(omni_mode)
    if (!(input.b2)) return RUN_BUTTON; //if the player presses b2, start running
    if (!(input.b1)) return ACTION_BUTTON; //if the player presses b1, return action
    if (input.ay >= 0.33) return GO_UP; //if the player moves the accelerometer north, go up
    if (input.ay <= -0.33) return GO_DOWN; //if the player moves the accelerometer south, go up 
    if (input.ax <= -0.33)  return GO_LEFT; //if the player moves the accelerometer west, go up
    if (input.ax >= 0.33) return GO_RIGHT;//if the player moves the accelerometer east, go up
    else return NO_ACTION;
}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2

int update_game(int action)
{
    // Save player previous location before updating
    Player.px = Player.x;
    Player.py = Player.y;
    int move = 0; //inialize the npc move variable
    int aM = get_active_map_num(); //gets the active map number
    if (aM == 0) { //if we're in the first map move the npc
        move = update_npc(npcX, npcY, npcM); //check which way he moves and keep track of it with npcX and npcY
        if (move == 1) {
            npcY = npcY - 1;
        } else if (move == 2) {
            npcY = npcY + 1;
        } else if (move == 3) {
            npcX = npcX + 1;
        } else if (move == 4) {
            npcX = npcX - 1;
        }
        npcM = npcM + 1; //does nothing
    } else {
        move = 0;
    }
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
            if (go_up(Player.px,Player.py)) return FULL_DRAW; //check if we can go up and then draw
            else break;
        case GO_LEFT:
            if (go_left(Player.px,Player.py)) return FULL_DRAW; //check if we can go left and then draw
            else break;
        case GO_DOWN:
            if (go_down(Player.px,Player.py)) return FULL_DRAW; //check if we can go down and then draw
            else break;
        case GO_RIGHT:
            if (go_right(Player.px,Player.py)) return FULL_DRAW; //check if we can go right and then draw
            else break;
        case ACTION_BUTTON: 
            if (check(Player.px, Player.py)) { //goes to a separate function which checks if we can interact with other things and then draw
                return FULL_DRAW;
            }
            else break;
        case RUN_BUTTON:
            Player.is_running = !(Player.is_running);//changes the running state of the player
            if (Player.is_running) { //Outdated
                Player.color = YELLOW;
            } else {
                Player.color = RED;
            }
            return FULL_DRAW; //draw
        case MENU_BUTTON: break;
        case GOD_MODE:
            Player.god_mode = !(Player.god_mode); //changes god mode state of the player and draw
            if (Player.god_mode) { //Outdated
                Player.color = BLUE;
            } else {
                Player.color = RED;
            }
            return FULL_DRAW;
        case ENCOUNTER: //fighting a monster
            if(Player.fighting == 1) { //the first monster (surprise attack)
                speech("AHH!", "Suprise attack");
            } else if (Player.fighting == 2) { //second monster (quest)
                speech("This is", "the monster");
                speech("attacking", "the town");
            }
            speech("What spell", "should you");
            speech("use?", ""); 
            while (Player.health) { //while the player is still alive keep fighting
                draw_upper_status(Player.health);
                int att = player_speech("1. Fire", "2. Water"); //chooae either fire or water
                if (att == 1) { //choose water and he's dead
                    speech("CRITICAL HIT", "");
                    speech("The monster", "is dead");
                    break;
                } else { //choose fire and he attacks
                    int hit = rand() % 2; //random chance he misses (50%)
                    speech("Not very", "effective...");
                    speech("The monster", "attacks");
                    if (hit == 1) {
                        speech("Take 1", "damage");
                        Player.health = Player.health - 1; //take away a health if he hitss
                    } else {
                        speech("But he", "misses");
                    }
                }
            }
            if (Player.fighting == 1) { //once the player wins the surprise fight change the monster so you can get by him
                MapItem* newMon = get_south(Player.px,Player.py);
                newMon->walkable = true;
            } else if (Player.fighting == 2) {//once you defeat the quest monster you win
                Player.quest_done = 1;   
            }
            Player.fighting = 0; //no longer fighting
            fight = 0;
            return FULL_DRAW; //draw
        default:        break;
    }
    if(move != 0) { //if the npc moved draw
        return FULL_DRAW;   
    }
    return NO_RESULT;
}

int check(int x, int y) { //checks if there is an interactable object and does different things depending on what it is
    MapItem* objH = get_here(x, y);
    MapItem* objN = get_north(x, y);
    MapItem* objS = get_south(x, y);
    MapItem* objE = get_east(x, y);
    MapItem* objW = get_west(x, y);
    if (objH->type == NPC || objS->type == NPC || objE->type == NPC || objW->type == NPC || objN->type == NPC){ //if its the npc talk to him
         if (!(Player.quest_begin)) {
            speech("Help!",""); 
            speech("There's a", "fire monster"); 
            speech("attacking the", "village!");
            int p1 = player_speech("1. Friendly", "2. Hostile"); //choose to respond, hostile makes him angrier with you
            if (p1 == 0) {
                speech("I'll give", "you this key"); 
                speech("if you're able", "to slay  it.");
            } else {
                Player.mean = Player.mean - 1;
                speech("You could","have just");
                speech("said no","");
            }
            int p2 = player_speech("1. Friendly", "2. Hostile"); //same as before with responses
            if (p2 == 0) {
                speech("Go to that", "blue portal"); 
                speech("down south!","");
            } else {
                Player.mean = Player.mean - 1;
                speech("Why are","you so");
                speech("mean","");
            }
            Player.quest_begin = 1;
         } else if (Player.mean == 1) { //eventually he doesn't want to talk to you
             speech("I don't feel", "like talking"); 
             speech("Just go kill","the monster");
             int p1 = player_speech("1. Friendly", "2. Hostile");
             if (p1 == 1) { //he gets fed up and doesn't want you to do the quest, so you lose
                Player.mean = Player.mean - 1;
                speech("Alright I'm", "fed up with you");
                game_over_lost = 1;
            } 
         } else if (Player.quest_begin && !(Player.quest_done)) { //gives you hints if you're friendly enough
             
             speech("I've heard", "the monster");
             speech("is weak to", "water.");
             if (Player.mean > 2) { //if you're not friendly he won't give you this hint
                speech("Also be", "careful!");
                speech("sometimes", "monsters");
                speech("appear on", "the briges");    
             }             
        } else if (Player.quest_begin && Player.quest_done && !(Player.has_key)) {  //once you talk to him after defeating the monster he gives you the key
            speech("Thank you", "so much!"); 
            speech("As promised,", "here's the key");
            Player.has_key = 1;
            Player.color = YELLOW;
        } else if (Player.has_key) {
            speech("Phew!",""); 
            speech("Thank you", "again");    
        }
        return 1;
    } else if (objH->type == PORTAL || objS->type == PORTAL || objE->type == PORTAL || objW->type == PORTAL || objN->type == PORTAL) {// transports you to the other map 
        if (!(get_active_map_num())) { //to the second map
            init_second_map();
            speech("Travelling to", "the village");
            Player.x = 2;
            Player.y = 2;
        } else { //to the first map
            init_main_map();
            speech("Back to the", "main map");    
        }
        return 1;
    } else if (objH->type == DOOR || objS->type == DOOR || objE->type == DOOR || objW->type == DOOR || objN->type == DOOR) { 
        if (Player.has_key) { //if you have the key, the door opens
            speech("Congrats!",""); 
            speech("The door has", "been unlocked.");
            objN->walkable = true;
            return 1;
        } else { //if not it tells you to get it from the npc
            speech("You need to", "get the key"); 
            speech("from the NPC","");
            return 1;
        }
    } else if (objH->type == MON || objS->type == MON || objE->type == MON || objW->type == MON || objN->type == MON) { //if its a monster, start a fight
        fight = 1;
        Player.fighting = 2;
        return 1;
    } else if (objH->type == FIRE || objS->type == FIRE || objE->type == FIRE || objW->type == FIRE || objN->type == FIRE) { //outdated
        if (Player.quest_done) {
            speech("What are you", "doing!"); 
            speech("He's already", "dead.");
        } else {
            speech("You cast", "a fire spell");
            speech("The spell", "backfired!"); 
            speech("Take 1","damage");
            Player.health = Player.health - 1;
            Player.spell_type = FIRE_SPELL;
        }
        return 1;
    } else if (objH->type == CHEST || objS->type == CHEST || objE->type == CHEST || objW->type == CHEST || objN->type == CHEST) { //the chest at the end of the quest wins the game once you open it
        game_over = 1;
        speech("Chest is open", "");
        return 1;
    }else return 0;
}


int go_up(int x, int y) { //checks if you can walk up and then moves you depending on if you're running
    if (get_north(x,y)->walkable || Player.god_mode) {
        if (Player.is_running && get_north(x,y-1)->walkable) {
            Player.y = Player.y - 2;
        } else {
            Player.y = Player.y - 1;
        }
        return 1;
    } else  return 0;
}
int go_right(int x, int y) { //checks if you can walk right and then moves you depending on if you're running
    if (get_east(x,y)->walkable || Player.god_mode) {
        if (Player.is_running && get_east(x+1,y)->walkable) {
            Player.x = Player.x + 2;
        } else {
            Player.x = Player.x + 1;
        }
        return 1;
    } else  return 0;
    
}
int go_down(int x, int y) { //checks if you can walk down and then moves you depending on if you're running
    if (get_south(x,y)->walkable || Player.god_mode) {
        if (Player.is_running && get_south(x,y+1)->walkable) {
            Player.y = Player.y + 2;
        } else {
            Player.y = Player.y + 1;
        }
        if (get_here(x,y)->type == TRAP) { //checks if you set off the trap and then spawns a mosnter below you
            fight = 1; //starts the fight
            Player.fighting = 1;
            MapItem* newMon = get_south(x,y+1); 
            newMon->type = MON; //spawns the monster
            newMon->walkable = false;
            newMon->draw = draw_mon;
            
        }
        return 1;
    } else  return 0;
}
int go_left(int x, int y) { //checks if you can walk left and then moves you depending on if you're running
    if (get_west(x,y)->walkable || Player.god_mode) {
        if (Player.is_running && get_north(x,x-1)->walkable) {
            Player.x = Player.x - 2;
        } else {
            Player.x = Player.x - 1;
        }
        return 1;
    } else  return 0;
}

void init_second_map() //initializing the second map
{
    
    Map* map = set_active_map(1); //set map as the second map
    // "Random" plants
    for(int i = map_width() + 3; i < map_area(); i += 39)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
        
    pc.printf("Adding walls!\r\n");
    //add walls
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    //add a portal block
    add_portal(6, 6);
    //add the monster
    add_mon(10,10);

    print_map();
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    //if you lost all health you lose
    if (Player.health == 0) {
        game_over_lost = 1;
    }
    //if you're fighting start a fight
    if (Player.fighting == 1) {
        fight = 1;
    }
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + Player.x;
            int y = j + Player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + Player.px;
            int py = j + Player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                    draw_player(u, v, Player.has_key, Player.is_running, Player.anim);
                    if((Player.x != Player.px) ||(Player.y != Player.py)) { //if the player hasn't moved don't continue his animation
                        Player.anim  = (Player.anim + 1) % 2;
                    }
                    continue;
            }
            else if (x >= 1 && y >= 1 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {
                            draw = draw_nothing;
                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) draw(u, v);
        }
    }

    // Draw status bars
    if (Player.x != Player.px || Player.y != Player.py) {
        draw_lower_status(Player.x, Player.y);
        draw_upper_status(Player.health);
    }
}


/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and plants in the background so you can see motion. Note: using the similar
 * procedure you can init the secondary map(s).
 */
void init_main_map()
{
    // "Random" plants
    Map* map = set_active_map(0);
    for(int i = map_width() + 5; i < map_area(); i += 45)
    {
        add_plant(i % map_width(), i / map_width());
    }
    pc.printf("plants\r\n");
        
    pc.printf("Adding walls!\r\n");
    //add walls
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    
    //add the npc
    add_npc(3, 3);
    //add a 4 block portal
    add_portal(25, 45);
    add_portal(26, 45);
    add_portal(26, 46);
    add_portal(25, 46);
    //add the door to the final room
    add_door(25,10);
    
    //add_door(26,10);
    //add_door(24,10);
    
    //add a box around the final room
    add_wall(18, 0,  VERTICAL, 10);
    add_wall(32, 0,  VERTICAL, 10);
    add_wall(18, 10,  HORIZONTAL, 7);
    add_wall(26, 10,  HORIZONTAL, 7);
    
    //add the river in the middle of the map
    add_river(0, 23, HORIZONTAL, 50);
    //add the bridge and its walls
    add_bridge_wall(19, 20, VERTICAL, 9);
    add_bridge_wall(21, 20, VERTICAL, 9);
    add_bridge(20, 20, VERTICAL, 9);
    //add the trap on the bridge
    add_trap(20, 23);
    //add a chest
    add_chest(24, 5);
    add_chest(24, 5);

    print_map();
}

void draw_opening() {
    inputs = read_inputs();
     uLCD.filled_rectangle(0, 0, 128, 128, BLACK); //print out a black rectangle with the words game begin
            uLCD.locate(5, 5);
            uLCD.textbackground_color(0xFFFFFF);
            uLCD.color(0x000000);
            const char* line = "Game Begin";
            while (*line) {
                uLCD.printf("%c", *line);
                line++;
            }
    while (inputs.b1) { //press b1 to continue with the drawing
            inputs = read_inputs();
    }
    
}


/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    //uLCD.filled_rectangle(64, 64, 74, 74, RED); //DELETE OR COMMENT THIS LINE
    // Initialize the maps
    maps_init();
    init_main_map();
    
    // Initialize game state
    //Set the variables of the player and game_over and npc coordinates
    //set the active map to the first 1
    set_active_map(0);
    Player.x = 3;
    Player.y = 2;
    Player.god_mode = 0;
    Player.color = RED;
    Player.is_running = 0;
    Player.health = 3;
    Player.anim = 0;
    Player.mean = 3;
    Player.talk = 0;
    Player.fighting = 0;
    game_over = 0;
    npcX = 3;
    npcY = 3;
    npcM = 0;
    
    //draw the opening screen
    draw_opening();

    // Initial drawing
    draw_game(true);
    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
        // Actually do the game update:
        // 1. Read inputs
        inputs = read_inputs();

        // 2. Determine action (get_action)
        action = get_action(inputs);


        // 3. Update game (update_game)
        init = update_game(action);
        
        // 3b. Check for game over
        if (game_over) { //if this is true you won
            uLCD.filled_rectangle(0, 0, 128, 128, BLACK);
            uLCD.locate(5, 5);
            uLCD.textbackground_color(0xFFFFFF);
            uLCD.color(0x000000);
            const char* line = "Game Over"; //print out game over
            const char* line2 = "You Won!";//You won
            while (*line) {
                uLCD.printf("%c", *line);
                line++;
            }
            uLCD.locate(5,7);
            while (*line2) {
                uLCD.printf("%c", *line2);
                line2++;
            }
        while(1){ //infinitly stay there
            
        }
        } else if (game_over_lost) { //if this is true you lost
             uLCD.filled_rectangle(0, 0, 128, 128, BLACK);
            uLCD.locate(5, 5);
            uLCD.textbackground_color(0xFFFFFF);
            uLCD.color(0x000000);
            const char* line = "Game Over"; //print out game over
            const char* line2 = "You Lost! :("; //You Lost! :(
            while (*line) {
                uLCD.printf("%c", *line);
                line++;
            }
            uLCD.locate(4,7);
            while (*line2) {
                uLCD.printf("%c", *line2);
                line2++;
            }
            while(1){//infinitly stay there
            }
        }
        // 4. Draw frame (draw_game)
        draw_game(init);
        
        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
