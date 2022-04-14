#ifndef SPEECH_H
#define SPEECH_H

/**
 * Display a speech bubble.
 */
void speech(const char* line1, const char* line2);

/**
 * Display a long speech bubble (more than 2 lines).
 * 
 * @param lines The actual lines of text to display
 * @param n The number of lines to display.
 */
void long_speech(const char* lines[], int n);
/**
* Used to wait until the player presses a button, then
* it returns a number representing which button they
* pressed
**/

int pspeech_bubble_wait();
/**
*
*Displays an interactive speech bubble
*
**/
int player_speech(const char* line1, const char* line2);

#endif // SPEECH_H