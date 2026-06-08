/* liau config.h — edit this file to customize keybinds and appearance */

#ifndef _10h0ch_config_h
#define _10h0ch_config_h

/* -------------------------------------------------------------------------
 * STATUS BAR
 * Colors are { R, G, B, A } in 0-255.
 * ------------------------------------------------------------------------- */
#define CFG_BAR_BG_R   0
#define CFG_BAR_BG_G   0
#define CFG_BAR_BG_B   0
#define CFG_BAR_BG_A   255

#define CFG_BAR_FG_R   255
#define CFG_BAR_FG_G   255
#define CFG_BAR_FG_B   255
#define CFG_BAR_FG_A   255

#define CFG_BAR_HEIGHT 23
#define CFG_BAR_FONT   "monospace"
#define CFG_BAR_FONTSIZE 11

/* -------------------------------------------------------------------------
 * GLOBAL KEYBINDS  (handled in tools.c before the active tool sees the key)
 * ------------------------------------------------------------------------- */
#define CFG_KEY_QUIT          SDLK_q
#define CFG_KEY_TOOL_NONE     SDLK_ESCAPE
#define CFG_KEY_TOOL_PENCIL   SDLK_p
#define CFG_KEY_TOOL_ERASER   SDLK_e
#define CFG_KEY_TOOL_MARKER   SDLK_m
#define CFG_KEY_TOOL_RECT     SDLK_r
#define CFG_KEY_UNDO          SDLK_z   /* Ctrl + this */
#define CFG_KEY_REDO          SDLK_y   /* Ctrl + this */

/* -------------------------------------------------------------------------
 * SHARED PER-TOOL KEYBINDS
 * Used by pencil, eraser, marker, rect.
 * ------------------------------------------------------------------------- */
#define CFG_KEY_SIZE_DEC      SDLK_LEFTBRACKET
#define CFG_KEY_SIZE_INC      SDLK_RIGHTBRACKET
#define CFG_KEY_COLOR_INPUT   SDLK_c
#define CFG_KEY_ALPHA_INPUT   SDLK_a
#define CFG_KEY_PASTE         SDLK_v   /* Ctrl + this */

/* -------------------------------------------------------------------------
 * PENCIL TOOL
 * ------------------------------------------------------------------------- */
#define CFG_PENCIL_DEFAULT_SIZE  8
#define CFG_PENCIL_DEFAULT_COLOR 0xffffffff /* RRGGBBAA */

/* -------------------------------------------------------------------------
 * ERASER TOOL
 * ------------------------------------------------------------------------- */
#define CFG_ERASER_DEFAULT_SIZE  8

/* -------------------------------------------------------------------------
 * MARKER TOOL
 * ------------------------------------------------------------------------- */
#define CFG_MARKER_DEFAULT_SIZE  12
#define CFG_MARKER_DEFAULT_COLOR 0x000000ff /* RRGGBBAA */

/* -------------------------------------------------------------------------
 * RECT TOOL
 * ------------------------------------------------------------------------- */
#define CFG_RECT_DEFAULT_THICKNESS 1
#define CFG_RECT_DEFAULT_COLOR     0xff0000ff /* RRGGBBAA */
#define CFG_KEY_RECT_TOGGLE_FILL   SDLK_f

/* -------------------------------------------------------------------------
 * MARKER TOOL
 * ------------------------------------------------------------------------- */
#define CFG_KEY_MARKER_NUM_PREV   SDLK_SEMICOLON
#define CFG_KEY_MARKER_NUM_NEXT   SDLK_QUOTE
#define CFG_KEY_MARKER_NUM_RESET  SDLK_PERIOD
#define CFG_KEY_MARKER_AUTO_INC   SDLK_i

#endif
