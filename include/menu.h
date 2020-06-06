#ifndef MENU_H
#define MENU_H

enum menu_cb_type {
	MENU_CB_START_TESTING,
	MENU_CB_EXIT,
};

typedef void (*menu_cb_t)(enum menu_cb_type type);

void menu_init(menu_cb_t cb);
void menu_exit(void);
int menu_start(void);
int menu_inject(const char *data);

#endif /* MENU_H */
