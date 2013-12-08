#include "screen.h"

static void _screen_getTermSize(int* cols, int* rows);
static void _screen_setDefaultColor();
static int _screen_setColor(t_screen* screen, int textColor, int backColor);

t_screen* screen_create() {
	int width = 0, height = 0;
	_screen_getTermSize(&width, &height);

	if (width < 80 || height < 24)
		return NULL;

	t_screen* this = new(t_screen);
	_screen_getTermSize(&this->width, &this->height);

	this->mainWin = initscr();
	noecho();
	start_color();
    box(stdscr, 0, 0);
    refresh();

	this->colorPairs = list_create();
    this->colorCount = 0;
    _screen_setColor(this, COLOR_WHITE, COLOR_BLACK);

	this->secWin = newwin(this->height, this->width, 0, 0);
	box(this->secWin, 0, 0);

	screen_refresh(this);
    return this;
}

void screen_preRefresh(t_screen* screen) {
    werase(screen->secWin);
    box(screen->secWin, 0, 0);
    wbkgd(screen->secWin, COLOR_PAIR(1));
}

void screen_renderItem(t_screen* screen, i_drawable* item) {
	char** lines = string_split(item->text, "\n");
	char** textColors = string_split(item->colors_text, "\n");
	char** backColors = string_split(item->colors_back, "\n");

	int _getColorCode(char code) {
		char str[2]; str[0] = code; str[1] = '\0';
		return atoi(str);
	}

	for(int i=0; lines[i] != NULL; i++) {
		for(int j=0; lines[i][j] != '\0'; j++) {
			if (lines[i][j] == '~') continue;
			//(transparency)

			int color = _screen_setColor(
				screen,
				_getColorCode(textColors[i][j]),
				_getColorCode(backColors[i][j])
			);
			wmove(screen->secWin, item->position.y - drawable_height(item) + i, item->position.x + j);
			waddch(screen->secWin, lines[i][j] | COLOR_PAIR(color));
		}
	}

	free(lines); free(textColors); free(backColors);
	_screen_setDefaultColor();
}

void screen_refresh(t_screen* screen) {
    wrefresh(screen->mainWin);
    wrefresh(screen->secWin);
}

void screen_destroy(t_screen* screen) {
	list_destroy_and_destroy_elements(screen->colorPairs, free);

    //delwin(screen->mainWin);
    delwin(screen->secWin);
    endwin();
    refresh();
	free(screen);
	clear();
}

static void _screen_setDefaultColor() {
	color_set(1, NULL);
}

static int _screen_setColor(t_screen* screen, int textColor, int backColor) {
	bool _findPair(t_color_pair* colorPair) {
		return colorPair->text == textColor && colorPair->back == backColor;
	}
	t_color_pair* color = list_find(screen->colorPairs, (void*) _findPair);

	if (color == NULL) {
		color = new(t_color_pair);
		color->id = ++screen->colorCount;
		color->text = textColor;
		color->back = backColor;
		list_add(screen->colorPairs, color);

		init_pair(color->id, color->text, color->back);
	}

	color_set(color->id, NULL);
	return color->id;
}

static void _screen_getTermSize(int* cols, int* rows) {
    struct winsize ws;
    ioctl(0, TIOCGWINSZ, &ws);
    *rows = ws.ws_row;
    *cols = ws.ws_col;
}
