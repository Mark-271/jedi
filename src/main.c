#include <menu.h>
#include <tools.h>
#include <test.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define INPUT_BUF_LEN	80

static char input_buf[INPUT_BUF_LEN + 1];
static bool testing_started;

/**
 * Read input line from user.
 *
 * @return Read string or NULL on error
 */
static char *read_input(void)
{
	return fgets(input_buf, INPUT_BUF_LEN, stdin);
}

static void handle_menu_event(enum menu_cb_type type)
{
	switch (type) {
	case MENU_CB_START_TESTING:
		testing_started = true;
		break;
	case MENU_CB_EXIT:
		exit(EXIT_SUCCESS);
		break;
	default:
		fprintf(stderr, "Error: Unknown menu callback type %d\n", type);
		exit(EXIT_FAILURE);
	}
}

int main(void)
{
	int err;

	menu_init(handle_menu_event);
	err = menu_start();
	if (err)
		return EXIT_FAILURE;
	while (!testing_started) {
		err = menu_inject(read_input());
		if (err)
			puts("Try again\n");
	}
	menu_exit();

	printf("Testing started: please enter correct "
		"answer for each question\n"); 
	
	testing();
	
	return EXIT_SUCCESS;
}
