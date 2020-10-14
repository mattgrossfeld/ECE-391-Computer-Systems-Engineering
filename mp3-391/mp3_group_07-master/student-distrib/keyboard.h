#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "lib.h"

#define buffer_limit 128

// helper function to initialize capital letters & shift symbols
extern void init_capital();
// helper function to initialize scancode
extern void init_scancode();
// helper function to convert scancodes to ascii
extern void echochar();
// helper function to decrement backspace endpoint
extern void decrement_screen();
// helper function to return index
extern int return_index();
//helper function to return keyboard buffer
extern void return_buf(char* buf);
//helper function to set keyboard buffer
extern void set_buf(char* buf);
//helper function to return keyboardflag
extern void return_keyboardflag(int* b);
//helper function to change keyboardflag
extern void clear_keyboard();
//extern void change_keyboardflag(int* b);
//helper function to change index
extern void set_index();
#endif
