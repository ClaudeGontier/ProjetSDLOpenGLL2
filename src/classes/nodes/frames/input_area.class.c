#include "math/math_util.h"
#include "io/model.h"
#include "storage/node.h"
#include "io/shader.h"
#include "render/render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "window.h"
#include "gui/frame.h"
#include "io/input.h"

class InputArea @promote extends Frame {
    __containerType__ Node *

    void constructor() {
        Frame *frame;
        frame = malloc(sizeof(Frame));
        POINTER_CHECK(frame);

        this->object = frame;
        this->type = __type__; 
        SUPER(initialize_node);
        METHOD(this, init_frame);
    }

    void cast(void ** data) {
        IGNORE(data);
    }

    void load(FILE *file) {
        METHOD_TYPE(this, __type__, constructor);
        Frame *frame = (Frame *) this->object;
        frame->inputArea = malloc(sizeof(InputArea));
        POINTER_CHECK(frame->inputArea);
        frame->relPos[0] = 0.0f;
        frame->relPos[1] = 0.0f;
        frame->scale[0] = 100.0f;
        frame->scale[1] = 100.0f;
        frame->unit[0] = '%';
        frame->unit[1] = '%';
        frame->unit[2] = '%';
        frame->unit[3] = '%';
        frame->flags |= FRAME_CONTENT;
        if (file) {
            fscanf(file, "(%[^,],%c%c)", 
            frame->inputArea->defaultText, &frame->alignment[0], &frame->alignment[1]);
            frame->inputArea->text[0] = '\0';
        }
    }

    void refresh() {
        SUPER(refresh);
        Frame *frame = (Frame *) this->object;
        InputArea *inputArea = (InputArea *) frame->inputArea;

        if (inputArea->text[0]) draw_text(frame->contentSurface, 0, 0, inputArea->text, frame->theme->font.font, frame->theme->textColor, frame->alignment, -1);
        else draw_text(frame->contentSurface, 0, 0, inputArea->defaultText, frame->theme->font.font, (SDL_Color) {frame->theme->textColor.r, frame->theme->textColor.g, frame->theme->textColor.b, frame->theme->textColor.a*0.8}, frame->alignment, -1);
        SUPER(refreshContent);
    }

    void update() {
        Frame *frame = (Frame *) this->object;
        InputArea *inputArea = (InputArea *) frame->inputArea;
        float x,y,w,h;
        x = frame->absPos[0];
        y = frame->absPos[1];
        w = frame->size[0];
        h = frame->size[1];
        if (inputArea->state == BUTTON_STATE_PRESSED) {
            if (input.text_input) {
                sprintf(inputArea->text, "%s", input.inputBuffer);
                METHOD(this, refresh);
            }
            if (input.released_keys & KEY_ENTER || ((input.mouse.released_button == SDL_BUTTON_LEFT || input.mouse.pressed_button == SDL_BUTTON_LEFT) && !(input.mouse.x > x &&
                input.mouse.x < x+w &&
                input.mouse.y > y &&
                input.mouse.y < y+h)))
                inputArea->state = BUTTON_STATE_NORMAL;
        } else {
            if (input.mouse.x > x &&
                input.mouse.x < x+w &&
                input.mouse.y > y &&
                input.mouse.y < y+h) {

                inputArea->state = BUTTON_STATE_HOVERED;

                if (input.mouse.pressed_button == SDL_BUTTON_LEFT) {
                    inputArea->state = BUTTON_STATE_PRESSED;
                    SDL_StartTextInput();
                    strcpy(input.inputBuffer, inputArea->text);
                }

            } else {
                inputArea->state = BUTTON_STATE_NORMAL;
            }
        }
        if (window.resized) frame->flags |= FRAME_NEEDS_REFRESH;
    }

    void is_input_area(bool *result) {
        *result = true;
    }

    void save(FILE *file, Node *editor) {
        IGNORE(editor);
        fprintf(file, "%s", classManager.class_names[this->type]);
    }

    void free() {
        Frame *frame = (Frame *) this->object;
        InputArea *inputArea = frame->inputArea;
        free(inputArea);
        SUPER(free);
    }
 
    
}