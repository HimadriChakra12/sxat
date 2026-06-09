/* liau config.h — edit this file to customize keybinds and appearance */

#ifndef _10h0ch_config_h
#define _10h0ch_config_h

/* -------------------------------------------------------------------------
 * SAVE / COPY
 * CFG_SAVE_DIR : directory for saved annotated images (tilde expanded).
 * Ctrl+S saves, Ctrl+W copies the saved path to clipboard.
 * ------------------------------------------------------------------------- */
#define CFG_SAVE_DIR   "~/.sxa"
#define CFG_KEY_SAVE   SDLK_s   /* Ctrl + this -> save to CFG_SAVE_DIR      */
#define CFG_KEY_COPY   SDLK_w   /* Ctrl + this -> copy saved path to clipboard */

/* -------------------------------------------------------------------------
 * STATUS BAR
 * Colors as 0xRRGGBBAA hex literals.
 * ------------------------------------------------------------------------- */
#define CFG_BAR_BG     0x000000ff
#define CFG_BAR_FG     0xffffffff
#define CFG_BAR_HEIGHT   23
#define CFG_BAR_FONT     "monospace"
#define CFG_BAR_FONTSIZE 11

/* helpers used by status_bar.c — do not edit */
#define _CFG_R(c) (((c)>>24)&0xff)
#define _CFG_G(c) (((c)>>16)&0xff)
#define _CFG_B(c) (((c)>> 8)&0xff)
#define _CFG_A(c) (((c)    )&0xff)

/* -------------------------------------------------------------------------
 * GLOBAL KEYBINDS
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
#define CFG_KEY_MARKER_NUM_PREV  SDLK_SEMICOLON
#define CFG_KEY_MARKER_NUM_NEXT  SDLK_QUOTE
#define CFG_KEY_MARKER_NUM_RESET SDLK_PERIOD
#define CFG_KEY_MARKER_AUTO_INC  SDLK_i

/* -------------------------------------------------------------------------
 * RECT TOOL
 * ------------------------------------------------------------------------- */
#define CFG_RECT_DEFAULT_THICKNESS 1
#define CFG_RECT_DEFAULT_COLOR     0xff0000ff /* RRGGBBAA */
#define CFG_KEY_RECT_TOGGLE_FILL   SDLK_f

#endif
