#include "tools.h"
#include "history.h"
#include "../config.h"

static tool_t* current_tool;

void tools_init(void) {
    tools_activate_tool(&tools_tool_none);
}

void tools_deinit(void) {
}

char* tools_get_status_bar_text(void) {
    return tool_get_status_bar_text(current_tool);
}

void tools_render_tool_ghost(void) {
    tool_render_ghost(current_tool);
}

void tools_activate_tool(tool_t* tool) {
    if (current_tool == tool) return;
    tool_handle_deactivate(current_tool);
    current_tool = tool;
    tool_handle_activate(current_tool);
}

uint8_t tools_handle_keydown(SDL_KeyboardEvent* evt) {
    uint8_t tool_handler_result = tool_handle_keydown(current_tool, evt);
    if (tool_handler_result <= 0) {
        uint8_t ctrl = (evt->keysym.mod & KMOD_CTRL) != 0;
        switch(evt->keysym.sym) {
            case CFG_KEY_UNDO:
                if (ctrl) { history_undo(); return 1; }
                break;
            case CFG_KEY_REDO:
                if (ctrl) { history_redo(); return 1; }
                break;
            case CFG_KEY_TOOL_NONE:
                tools_activate_tool(&tools_tool_none);
                return 1;
            case CFG_KEY_TOOL_PENCIL:
                tools_activate_tool(&tools_tool_pencil);
                return 1;
            case CFG_KEY_TOOL_ERASER:
                tools_activate_tool(&tools_tool_eraser);
                return 1;
            case CFG_KEY_TOOL_MARKER:
                tools_activate_tool(&tools_tool_marker);
                return 1;
            case CFG_KEY_TOOL_RECT:
                tools_activate_tool(&tools_tool_rect);
                return 1;
        }
    } else {
        return tool_handler_result;
    }
    return 0;
}

uint8_t tools_handle_mouse_motion(SDL_MouseMotionEvent* evt) {
    uint8_t too_handler_result = tool_handle_mouse_motion(current_tool, evt);
    if (too_handler_result <= 0) {

    } else {
        return too_handler_result;
    }

    return 0;
}

uint8_t tools_handle_mouse_click(SDL_MouseButtonEvent* evt) {
    uint8_t tool_handler_result = tool_handle_mouse_click(current_tool, evt);
    if (tool_handler_result <= 0) {

    } else {
        return tool_handler_result;
    }

    return 0;
}
