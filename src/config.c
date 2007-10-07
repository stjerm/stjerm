/*
 * config.c
 * This file is part of Stjerm
 *
 * Copyright (C) 2007 - Stjepan Glavina
 * Copyright (C) 2007 - Markus Groß
 * 
 * Stjerm is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Stjerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Stjerm; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include "stjerm.h"

extern int sargc;
extern char **sargv;
extern GtkWidget *mainwindow;
static char _font[200];
static float _opacity;
static GdkColor _bg;
static GdkColor _fg;
static int _border;
static unsigned int _mod;
static KeySym _key;
static int _width;
static int _height;
static int _pos;
static int _posx;
static int _posy;
static int _scrollpos;
static char _shell[200];
static int _lines;
static int _showtab;
static char _termname[200];
static GtkPositionType _tabpos;
static GdkColor _palette[16];
static int read_colors;
static gboolean _tabfill;
static gboolean _allowbold;
static GdkModifierType _keymod;
static gboolean _autohide;
static char _bgimage[200];
static gboolean _scrolloutput;

static void set_border(char*);
static void set_mod(char*);
static void set_key(char*);
static void set_pos(char *v);
static GtkPositionType read_pos(char *v);
static gboolean parse_hex_color(char *value, GdkColor *color);
static gboolean parse_bool_str(char *value, gboolean def);
static GdkModifierType parse_mod(char *value);
static pid_t get_stjerm_pid(void);

void read_value(char *name, char *value);
void init_default_values(void);
void conf_init(void);
char* conf_get_font(void);
float conf_get_opacity(void);
GdkColor conf_get_bg(void);
GdkColor conf_get_fg(void);
int conf_get_border(void);
unsigned int conf_get_mod(void);
KeySym conf_get_key(void);
int conf_get_width(void);
int conf_get_height(void);
void conf_get_position(int*, int*);
int conf_get_scrollbar(void);
char* conf_get_shell(void);
int conf_get_lines(void);
int conf_get_show_tab(void);
char* conf_get_term_name(void);
GtkPositionType conf_get_tab_pos(void);
GdkColor* conf_get_color_palette(void);
gboolean conf_get_tab_fill(void);
gboolean conf_get_allow_bold(void);
GdkModifierType conf_get_key_mod(void);
gboolean conf_get_auto_hide(void);
char* conf_get_bg_image(void);
gboolean conf_get_scroll_on_output(void);

Option options[OPTION_COUNT] = {
        { "key", "-k", "KEY", "Shortcut key (eg: f12)." },
        { "mod", "-m", "MODIFIER", "meta modifier key: shift, control, alt, windows, none." },
        { "keymod", "-km", "MODIFIER", "Modifier for keyboard shortcuts. Can be a combination (with +) of modifiers (eg: shift+control)." },
        { "autohide", "-ah", "BOOLEAN", "Whether or not to hide stjerm when it looses focus. Default: true." },
        { "font", "-fn", "FONT", "Terminal font and size (eg: Sans 10). Default: Bistream Vera Sans 10." },
        { "background", "-bg", "COLOR", "Background color. Default: Black." },
        { "foreground", "-fg", "COLOR", "Foreground color. Default: White." },
        { "allowbold", "-ab", "BOOLEAN", "Allow bold fonts or not. Default: true." },
        { "border", "-b", "TYPE", "Border type: thin, thick, none. Default: none." },
        { "opacity", "-o", "NUMBER", "Opacity (range: 10 - 100). Default: 100." },
        { "bgimage", "-bgimg", "FILE", "Background image to use on terminal." },
        { "width", "-w", "NUMBER", "Window width. Default: 800." },
        { "height", "-h", "NUMBER", "Window height. Default: 400." },
        { "position", "-p", "POSITION", "Window position: top, bottom, left, right. Default: top." },
        { "scrollbar", "-s", "POSITION", "Scrollbar position: left, right, none. Default: none." },
        { "shell", "-sh", "STRING", "Terminal Shell. Default: the user's default shell." },
        { "lines", "-l", "NUMBER", "Scrollback lines. 0 to disable scrollback. Default: 1000." },
        { "showtab", "-st", "VALUE", "Tabbar visibility (one: only visible when > 1 tabs): never, one, always." },
        { "tabpos", "-tp", "POSITION", "Tabbar position: top, bottom, left, right. Default: bottom." },
        { "tablabel", "-tl", "STRING", "Label of the tabs. Default: term." },
        { "tabfill", "-tf", "BOOLEAN", "Whether tabs fill whole tabbar space. Default: true." },
        { "scroll", "-sc", "BOOLEAN", "Whether to scroll the terminal on output. Default: true." },
        { "colorX", "-cX", "COLOR", "Specify color X of the terminals color palette" }
};

pid_t get_stjerm_pid(void) {
    FILE *p = popen("pidof stjerm", "r");
    if (p == NULL) {
        fprintf(stderr, "error: unable to get stjerm pid\n");
        exit(1);
    }
    char buffer[100];
    fgets(buffer, sizeof(buffer), p);
    pclose(p);
    char **list;
    list = g_strsplit_set(buffer, " ", -1);
    int i = 0;
    while (list[i] != NULL)
        i++;
    pid_t pid = (pid_t) atoi(list[i - 1]);
    g_strfreev(list);
    return pid;
}

void set_border(char *v) {
    if (!strcmp(v, "thin"))
        _border = BORDER_THIN;
    else if (!strcmp(v, "thick"))
        _border = BORDER_THICK;
    else
        _border = BORDER_NONE;
}

void set_mod(char *v) {
    v[0] = tolower(v[0]);
    if (!strcmp(v, "shift"))
        _mod = ShiftMask;
    else if (!strcmp(v, "control"))
        _mod = ControlMask;
    else if (!strcmp(v, "ctrl"))
        _mod = ControlMask;
    else if (!strcmp(v, "alt"))
        _mod = Mod1Mask;
    else if (!strcmp(v, "mod1"))
        _mod = Mod1Mask;
    else if (!strcmp(v, "windows"))
        _mod = Mod4Mask;
    else if (!strcmp(v, "win"))
        _mod = Mod4Mask;
    else
        _mod = 0;
}

void set_key(char *v) {
    _key = 0;

    v[0] = tolower(v[0]);
    _key = XStringToKeysym(v);

    if (!_key) {
        v[0] = toupper(v[0]);
        _key = XStringToKeysym(v);
    }
}

void set_pos(char *v) {
    if (!strcmp(v, "top"))
        _pos = POS_TOP;
    else if (!strcmp(v, "bottom"))
        _pos = POS_BOTTOM;
    else if (!strcmp(v, "left"))
        _pos = POS_LEFT;
    else if (!strcmp(v, "right"))
        _pos = POS_RIGHT;
    else if (!strcmp(v, "topleft"))
        _pos = POS_TOPLEFT;
    else if (!strcmp(v, "topright"))
        _pos = POS_TOPRIGHT;
    else if (!strcmp(v, "bottomleft"))
        _pos = POS_BOTTOMLEFT;
    else if (!strcmp(v, "bottomright"))
        _pos = POS_BOTTOMRIGHT;
}

GtkPositionType read_pos(char *v) {
    if (!strcmp(v, "top"))
        return GTK_POS_TOP;
    else if (!strcmp(v, "left"))
        return GTK_POS_LEFT;
    else if (!strcmp(v, "right"))
        return GTK_POS_RIGHT;
    else
        return GTK_POS_BOTTOM;
}

gboolean parse_hex_color(char *value, GdkColor *color) {
    if (!gdk_color_parse(value, color)) {
        char *value2 = g_strconcat("#", value, NULL);
        gboolean res = gdk_color_parse(value2, color);
        free(value2);
        return res;
    } else
        return TRUE;
}

gboolean parse_bool_str(char *value, gboolean def) {
    g_strstrip(value);
    gboolean res = def;
    if (!strcasecmp("false", value) || !strcasecmp("0", value) 
            || !strcasecmp("no", value))
        res = FALSE;
    if (!strcasecmp("true", value) || !strcasecmp("1", value) 
            || !strcasecmp("yes", value))
        res = TRUE;
    return res;
}

GdkModifierType parse_mod(char *value) {
    g_strstrip(value);
    if (!strcasecmp("control", value) || !strcasecmp("ctrl", value))
        return GDK_CONTROL_MASK;
    else if (!strcasecmp("alt", value))
        return GDK_MOD1_MASK;
    else if (!strcasecmp("shift", value))
        return GDK_SHIFT_MASK;
    else if (!strcasecmp("windows", value))
        return GDK_SUPER_MASK;
    else
        return 0;
}

void init_default_values(void) {
    strcpy(_font, "Bitstream Vera Sans Mono 10");
    gdk_color_parse("black", &_bg);
    gdk_color_parse("white", &_fg);
    _scrollpos = -1;
    _border = BORDER_NONE;
    _opacity = 100.0f;
    strcpy(_bgimage, "");
    _width = 800;
    _height = 400;
    _pos = POS_TOP;
    _mod = 0;
    _key = 0;
    strcpy(_shell, getpwuid(getuid())->pw_shell);
    _lines = 1000;
    _showtab = TABS_ONE;
    _tabpos = GTK_POS_BOTTOM;
    strcpy(_termname, "term");
    read_colors = 0;
    _tabfill = FALSE;
    _allowbold = TRUE;
    _keymod = GDK_CONTROL_MASK | GDK_SHIFT_MASK;
    _autohide = TRUE;
    _scrolloutput = TRUE;
}

void read_value(char *name, char *value) {
    if (name != NULL && value != NULL) {
        if (name[0] == '#')
            return;
        g_strstrip(name);
        g_strstrip(value);
        if (!strcmp("font", name) || !strcmp("-fn", name))
            strcpy(_font, value);
        else if (!strcmp("background", name) || !strcmp("-bg", name)) {
            if (!parse_hex_color(value, &_bg))
                gdk_color_parse("black", &_bg);
        } else if (!strcmp("foreground", name) || !strcmp("-fg", name)) {
            if (!parse_hex_color(value, &_fg))
                gdk_color_parse("white", &_fg);
        } else if (!strcmp("scrollbar", name) || !strcmp("-s", name)) {
            if (!strcmp(value, "true"))
                _scrollpos = POS_RIGHT;
            else if (!strcmp(value, "left"))
                _scrollpos = POS_LEFT;
            else if (!strcmp(value, "right"))
                _scrollpos = POS_RIGHT;
            else
                _scrollpos = -1;
        } else if (!strcmp("border", name) || !strcmp("-b", name))
            set_border(value);
        else if (!strcmp("opacity", name) || !strcmp("-o", name))
            _opacity = atof(value);
        else if (!strcmp("bgimage", name) || !strcmp("-bgimg", name))
            strcpy(_bgimage, value);
        else if (!strcmp("width", name) || !strcmp("-w", name))
            _width = atoi(value);
        else if (!strcmp("height", name) || !strcmp("-h", name))
            _height = atoi(value);
        else if (!strcmp("position", name) || !strcmp("-p", name))
            set_pos(value);
        else if (!strcmp("mod", name) || !strcmp("-m", name))
            set_mod(value);
        else if (!strcmp("key", name) || !strcmp("-k", name))
            set_key(value);
        else if (!strcmp("shell", name) || !strcmp("-sh", name))
            strcpy(_shell, value);
        else if (!strcmp("lines", name) || !strcmp("-bl", name))
            _lines = atoi(value);
        else if (!strcmp("showtab", name) || !strcmp("-showtab", name)) {
            if (!strcasecmp(value, "always"))
                _showtab = TABS_ALWAYS;
            else if (!strcasecmp(value, "never"))
                _showtab = TABS_NEVER;
        } else if (!strcmp("tabpos", name) || !strcmp("-tabpos", name))
            _tabpos = read_pos(value);
        else if (!strcmp("tablabel", name) || !strcmp("-tablabel", name))
            strcpy(_termname, value);
        else if (g_str_has_prefix(name, "color") || g_str_has_prefix(name, "-c")) {
            g_strcanon(name, "0123456789", ' ');
            g_strchug(name);
            parse_hex_color(value, &_palette[atoi(name)]);
            read_colors++;
        } else if (!strcmp("tabfill", name) || !strcmp("-tf", name))
            _tabfill = parse_bool_str(value, _tabfill);
        else if (!strcmp("allowbold", name) || !strcmp("-ab", name))
            _allowbold = parse_bool_str(value, _allowbold);
        else if (!strcmp("keymod", name) || !strcmp("-km", name)) {
            char **list;
            list = g_strsplit_set(value, "+", -1);
            GdkModifierType tmp = 0;
            int i = 0;
            while (list[i] != NULL)
                tmp = tmp | parse_mod(list[i++]);
            g_strfreev(list);
            if (tmp != 0)
                _keymod = tmp;
        } else if (!strcmp("autohide", name) || !strcmp("-ah", name))
            _autohide = parse_bool_str(value, _autohide);
        else if (!strcmp("scroll", name) || !strcmp("-sc", name))
            _scrolloutput = parse_bool_str(value, _scrolloutput);
    }
}

void conf_init(void) {
    init_default_values();
    gboolean keyoption = FALSE;

    Display *dpy = gdk_x11_display_get_xdisplay(gdk_display_get_default());
    XrmInitialize();
    char basename[12] = "/.Xdefaults";
    char *filename = strcat(getpwuid(getuid())->pw_dir, basename);
    XrmSetDatabase(dpy, XrmGetFileDatabase(filename));

    char *op;
    int i;
    Option o;
    for (i = 0; i < OPTION_COUNT - 1; i++) {
        o = options[i];
        if ((op = XGetDefault(dpy, "stjerm", o.long_name)))
            read_value(o.long_name, op);
    }
    char color[8];
    for (i = 0; i < 16; i++) {
        sprintf(color, "color%d", i);
        if ((op = XGetDefault(dpy, "stjerm", color)))
            read_value(color, op);
    }

    for (i = 1; i < sargc; i++) {
        if (sargv != NULL) {
            if (!strcmp(sargv[i], "--help")) {
                print_help();
                exit(1);
            } else if (!strcmp(sargv[i], "--info")) {
                print_info();
                exit(1);
            } else if (!strcmp("--toggle", sargv[i])) {
                kill(get_stjerm_pid(), SIGUSR1);
                exit(1);
            }
        }
        if (i + 1>= sargc)
            break;

        read_value(sargv[i], sargv[i + 1]);
    }

    if (keyoption == FALSE && _key == 0) {
        printf("hint: you started stjerm without specifying a shortcut key\n"
               "      to show/hide stjerm run stjerm with the toggle option\n"
               "      like this: stjerm --toggle");
    }

    struct stat st;
    if (stat(_shell, &st) != 0) {
        fprintf(stderr, "error: the shell '%s' can't be opened\n", _shell);
        exit(1);
    }

    if (_lines < 0) {
        fprintf(stderr, "error: a scrollback line count < 0 is not allowed\n");
        exit(1);
    }

    if (keyoption == TRUE && _key == 0) {
        fprintf(stderr, "error: wrong shortcut key is defined\n");
        exit(1);
    }

    if (read_colors > 0 && read_colors < 16) {
        fprintf(stderr, "error: read only %d colors, not 16\n"
                "specify a complete color palette\n", read_colors);
        exit(1);
    }

    signal(SIGUSR1, (__sighandler_t) mainwindow_toggle);

    int scrw = gdk_screen_get_width(gdk_screen_get_default());
    int scrh = gdk_screen_get_height(gdk_screen_get_default());

    if (_pos == POS_TOP) {
        _posx = (scrw - _width) / 2;
        _posy = 0;
    } else if (_pos == POS_BOTTOM) {
        _posx = (scrw - _width) / 2;
        _posy = scrh - _height;
    } else if (_pos == POS_LEFT) {
        _posx = 0;
        _posy = (scrh - _height) / 2;
    } else if (_pos == POS_RIGHT) {
        _posx = scrw - _width;
        _posy = (scrh - _height) / 2;
    } else if (_pos == POS_TOPLEFT) {
        _posx = 0;
        _posy = 0;
    } else if (_pos == POS_TOPRIGHT) {
        _posx = scrw - _width;
        _posy = 0;
    } else if (_pos == POS_BOTTOMLEFT) {
        _posx = 0;
        _posy = scrh - _height;
    } else if (_pos == POS_BOTTOMRIGHT) {
        _posx = scrw - _width;
        _posy = scrh - _height;
    }
}

char* conf_get_font(void) {
    return _font;
}

float conf_get_opacity(void) {
    return _opacity;
}

GdkColor conf_get_bg(void) {
    return _bg;
}

GdkColor conf_get_fg(void) {
    return _fg;
}

int conf_get_border(void) {
    return _border;
}

unsigned int conf_get_mod(void) {
    return _mod;
}

KeySym conf_get_key(void) {
    return _key;
}

int conf_get_width(void) {
    return _width;
}

int conf_get_height(void) {
    return _height;
}

void conf_get_position(int *x, int *y) {
    *x = _posx;
    *y = _posy;
}

int conf_get_scrollbar(void) {
    return _scrollpos;
}

char* conf_get_shell(void) {
    return _shell;
}

int conf_get_lines(void) {
    return _lines;
}

int conf_get_show_tab(void) {
    return _showtab;
}

char* conf_get_term_name(void) {
    return _termname;
}

GtkPositionType conf_get_tab_pos(void) {
    return _tabpos;
}

GdkColor* conf_get_color_palette(void) {
    if (read_colors <= 0)
        return NULL;
    else
        return _palette;
}

gboolean conf_get_tab_fill(void) {
    return _tabfill;
}

gboolean conf_get_allow_bold(void) {
    return _allowbold;
}

GdkModifierType conf_get_key_mod(void) {
    return _keymod;
}

gboolean conf_get_auto_hide(void) {
    return _autohide;
}

char* conf_get_bg_image(void) {
    if (!strcmp(_bgimage, ""))
        return NULL;
    else
        return _bgimage;
}

gboolean conf_get_scroll_on_output(void) {
    return _scrolloutput;
}
