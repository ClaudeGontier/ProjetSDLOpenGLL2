#include "../types.h"
#include <SDL2/SDL.h>
#include <stdio.h>
#include "../math/math_util.h"
#include "model.h"
#include "../render/framebuffer.h"
#include "../node.h"
#include "../window.h"
#include "../render/camera.h"
#include "input.h"



void init_input(Input *input) {
    input->active_keys = 0;
    input->released_keys = 0;
    input->pressed_keys = 0;
    input->mouse.x = 0;
    input->mouse.y = 0;
    input->mouse.pressed_button = 0;
    input->mouse.released_button = 0;
    input->mouse.active_button = 0;
    strcpy(input->inputBuffer, "");
}



/**
 * Updates the camera's position and orientation based on user input.
 * 
 * @param c {Camera*} Pointer to the Camera structure to be updated.
 * @param input {Input*} Pointer to the Input structure that stores current key states.
 * @return {int} Returns -1 if the escape key is pressed (to indicate an exit), otherwise returns 0.
 * 
 * This function processes keyboard events using SDL to modify the camera's target position 
 * and rotation based on key presses and releases. It also adjusts the movement speed based 
 * on whether the shift key is held down. The camera's direction is calculated based on its 
 * current rotation, and the target position is updated according to the pressed keys. 
 * Additionally, the function smoothly approaches the target position and rotation.
 */

int update_input(Input *input) {
    input->mouse.lastX = input->mouse.x;
    input->mouse.lastY = input->mouse.y;
    input->released_keys = 0;
    input->pressed_keys = 0;
    input->mouse.pressed_button = 0;
    input->mouse.released_button = 0;
	SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
        switch( event.type ){
            /* Look for a keypress */
            case SDL_KEYDOWN:
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ){
					case SDLK_q:
                    case SDLK_LEFT:
                        HANDLE_KEY_PRESSED(KEY_LEFT);
                        break;
					case SDLK_d:
                    case SDLK_RIGHT:
                        HANDLE_KEY_PRESSED(KEY_RIGHT);
                        break;
					case SDLK_z:
                    case SDLK_UP:
						HANDLE_KEY_PRESSED(KEY_UP);
                        break;
					case SDLK_s:
                    case SDLK_DOWN:
                        HANDLE_KEY_PRESSED(KEY_DOWN);
                        break;
                    case SDLK_SPACE:
                        HANDLE_KEY_PRESSED(KEY_JUMP);
                        break;
                    case SDLK_LSHIFT:
                        HANDLE_KEY_PRESSED(KEY_SHIFT);
                        break;
                    case SDLK_LCTRL:
                        HANDLE_KEY_PRESSED(KEY_CROUCH);
                        break;
                    case SDLK_ESCAPE:
						HANDLE_KEY_PRESSED(KEY_MENU);
                        break;

					case SDLK_f:
						HANDLE_KEY_PRESSED(KEY_F);
                        break;
					case SDLK_g:
						HANDLE_KEY_PRESSED(KEY_G);
                        break;

					case SDLK_RETURN:
						HANDLE_KEY_PRESSED(KEY_ENTER);
                        break;
					case SDLK_BACKSPACE:
						input->inputBuffer[strlen(input->inputBuffer)-1] = 0;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ){
					case SDLK_q:
                    case SDLK_LEFT:
                        HANDLE_KEY_RELEASED(KEY_LEFT);
                        break;
					case SDLK_d:
                    case SDLK_RIGHT:
                        HANDLE_KEY_RELEASED(KEY_RIGHT);
                        break;
					case SDLK_z:
                    case SDLK_UP:
						HANDLE_KEY_RELEASED(KEY_UP);
						break;
					case SDLK_s:
                    case SDLK_DOWN:
                        HANDLE_KEY_RELEASED(KEY_DOWN);
                        break;
                    case SDLK_SPACE:
                        HANDLE_KEY_RELEASED(KEY_JUMP);
                        break;
                    case SDLK_LSHIFT:
                        HANDLE_KEY_RELEASED(KEY_SHIFT);
                        break;
                    case SDLK_LCTRL:
                        HANDLE_KEY_RELEASED(KEY_CROUCH);
                        break;
                    case SDLK_ESCAPE:
						HANDLE_KEY_RELEASED(KEY_MENU);
                        break;

					case SDLK_f:
						HANDLE_KEY_RELEASED(KEY_F);
                        break;
					case SDLK_g:
						HANDLE_KEY_RELEASED(KEY_G);
                        break;

					case SDLK_RETURN:
						HANDLE_KEY_RELEASED(KEY_ENTER);
                        break;
                    default:
                        break;
                }
                break;
            case SDL_TEXTINPUT:
                if (strlen(input->inputBuffer)+1 < 100)
                    strcat(input->inputBuffer, event.text.text);
                break;
            case SDL_MOUSEMOTION:
                if (input->mouse.x == 0 && input->mouse.y == 0) {
                    input->mouse.lastX = event.motion.x;
                    input->mouse.lastY = event.motion.y;
                }
				input->mouse.x = event.motion.x;
				input->mouse.y = event.motion.y;
                break;
            case SDL_MOUSEBUTTONDOWN:
                input->mouse.pressed_button = event.button.button;
                input->mouse.active_button |= event.button.button;
                break;
            case SDL_MOUSEBUTTONUP:
                input->mouse.released_button = event.button.button;
                input->mouse.active_button &= ~event.button.button;
                break;
            case SDL_QUIT:
                return -1;
                break;
        }
    }

	return 0;

}