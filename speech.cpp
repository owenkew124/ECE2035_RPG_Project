#include "speech.h"

#include "globals.h"
#include "hardware.h"

Timer t;
GameInputs inp;
/**
 * Draw the speech bubble background.
 * Use this file to draw speech bubbles, look at the uLCD libraries for printing
 * text to the uLCD (see graphics.cpp note for link)
 */
static void draw_speech_bubble();


/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
int toggle;
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble() //draw a rectangle with a border
{
    uLCD.filled_rectangle(7, 112, 118, 80, 0x000000);
    
    uLCD.filled_rectangle(7,    80, 118,  81, 0xFFFFFF);
    uLCD.filled_rectangle(7,   112, 118, 111, 0xFFFFFF);
    uLCD.filled_rectangle(7,    112,   8, 80, 0xFFFFFF);
    uLCD.filled_rectangle(118,  112, 117, 80, 0xFFFFFF);
}


void draw_speech_line(const char* line, int which) //draws a line either on the top of the bubble or bottom
{
    uLCD.textbackground_color(0x000000);
    uLCD.color(0xFFFFFF);

    int i;
    if (which == TOP) {
        uLCD.locate(2, 11);
    } else {
        uLCD.locate(2, 12);
    }
    
    i = 0;
    while (i < 14 && *line) { //set a limit of 14 character or the line would run off the screen
        uLCD.printf("%c", *line); //go down the string printing 1 character a time
        wait_ms(50);
        line++;
        i++;
    }
}

void speech_bubble_wait()
{
    t.start(); //start a timer
    int time_passed = 0;
    
    while (inp.b1 || !(time_passed > 200 && !(inp.b1))) { //After a certain amount of time you can press a button to continue
        time_passed = t.read_ms();
        wait_ms(100);
        inp = read_inputs();
    }
    return;
}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble(); //draw the bubble
    draw_speech_line(line1, TOP); //do the top line
    draw_speech_line(line2, BOTTOM); //do the bottom line
    speech_bubble_wait();

}
int player_speech(const char* line1, const char* line2)
{
    draw_speech_bubble(); //draw the bubble
    draw_speech_line(line1, TOP); //do the top line
    draw_speech_line(line2, BOTTOM); //do the bottom line
    return pspeech_bubble_wait(); //return which button the player pressed
}
int pspeech_bubble_wait()
{
    t.start(); //start a timer
    int time_passed = 0;
    
    while (!(time_passed > 200 && ( !(inp.b1) || !(inp.b2))) ) { //once a set amount of time has passed you can press a button to move on
        time_passed = t.read_ms();
        wait_ms(100);
        inp = read_inputs();
    }
    if (!(inp.b1)) { //return 0 if b1 is pressed
        return 0;
    } else {//return 1 if b2 is pressed
        return 1; 
    }
}

void long_speech(const char* lines[], int n)
{
}
