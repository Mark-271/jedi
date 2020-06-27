/**
 * @file
 *
 * Implementation of Jedi Testing System menu.
 *
 * The module is implemented using Moore FSM (Finite State Machine) internally.
 * It provides object-style API, keeping one single object inside (aka
 * "Singleton"), so only one menu object can exist in the system; it can be
 * viewed as a "service".
 *
 * How to use it:
 *   1. Initialize menu module; it will register callback for "menu exit" events
 *   2. Start menu module
 *   2. While callback for some "menu exit" event is not emitted, keep
 *      providing the menu module with user input
 *   3. When you catch "menu exit" event -- stop asking user for new input, and
 *      handle received callback.
 *   4. De-initialize menu module when it's not needed anymore (e.g. on "menu
 *      exit" event).
 */

#include <menu.h>
#include <tools.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define JEDI_CODE_DELAY	1000	/* msec */

typedef void (*menu_handle_state_func_t)(void);

/* Keep 0 as undefined state, so it can be used as an error value */
enum menu_state {
	STATE_UNDEFINED,
	STATE_MAIN_MENU,
	STATE_JEDI_CODE,
	STATE_JEDI_ORDER,
	STATE_START_TESTING,
	STATE_EXIT,
	/* --- */
	STATE_NUM
};

/* Keep EVENT_START at 0, so 'n' integer can be easily mapped to EVENT_n */
enum menu_event {
	EVENT_START,
	EVENT_1,
	EVENT_2,
	EVENT_3,
	EVENT_4,
	/* --- */
	EVENT_NUM
};

struct menu {
	menu_cb_t cb;			/* callback to run on FSM exit */
	enum menu_state state;		/* current state of FSM */
};

static const char * const jedi_code[5] = {
	"There is no emotion, there is peace.",
	"There is no ignorance, there is knowledge.",
	"There is no passion, there is serenity.",
	"There is no chaos, there is harmony.",
	"There is no death, there is the Force.",
};

static const char * const jedi_order =
	"The Jedi Order was a noble religious order of protectors united in\n"
	"their devotion to the light side of the Force. With a history dating\n"
	"back thousands of years before the rise of the Galactic Empire, the\n"
	"Jedi Knights - noted for their lightsabers and natural ability to\n"
	"harness the powers of the Force - stood as the guardians of peace\n"
	"and justice in the Galactic Republic. The Order was originally\n"
	"established on Ahch-To in the Unknown Regions. In later years, the\n"
	"Jedi Temple on the Core World Coruscant served as the hub of all\n"
	"Jedi activity in the galaxy. The Temple was also a training school\n"
	"for younglings and Padawans, who learned the ways of the Force under\n"
	"the supervision of Jedi Masters, twelve of whom were elected to\n"
	"serve on the Jedi High Council - the Order's highest governing\n"
	"authority.";

static const char * const menu_msg[STATE_NUM] = {
	"[undefined state]",
	"Hello, young Padawan! "
		"This is testing system for Jedi Code knowledge.\n"
		"Please select one of next items:\n"
		"(1) See Jedi Code\n"
		"(2) See information about Jedi Order\n"
		"(3) Start testing\n"
		"(4) Exit\n",
	"Please select one of next items:\n"
		"(1) See information about Jedi Order\n"
		"(2) Back to main menu\n",
	"Please select one of next items:\n"
		"(1) See Jedi Code\n"
		"(2) Back to main menu\n",
	"",
	"",
};

/**
 * Transitions lookup table for Moore FSM.
 *
 * First index -- current state. Second index -- event.
 *
 * Contains state where transition (specified by two array indexes) leads.
 * If such transition doesn't exist -- element contains STATE_UNDEFINED (0).
 *
 * Such table allows us to perform a transaction map operation with O(1) speed,
 * by sacrificing some memory.
 */
static const enum menu_state menu_transitions[STATE_NUM][EVENT_NUM] = {
	{ /* STATE_UNDEFINED */
		STATE_MAIN_MENU,		/* EVENT_START */
		0,				/* EVENT_1 */
		0,				/* EVENT_2 */
		0,				/* EVENT_3 */
		0,				/* EVENT_4 */
	},
	{ /* STATE_MAIN_MENU */
		0,				/* EVENT_START */
		STATE_JEDI_CODE,		/* EVENT_1 */
		STATE_JEDI_ORDER,		/* EVENT_2 */
		STATE_START_TESTING,		/* EVENT_3 */
		STATE_EXIT,			/* EVENT_4 */
	},
	{ /* STATE_JEDI_CODE */
		0,				/* EVENT_START */
		STATE_JEDI_ORDER,		/* EVENT_1 */
		STATE_MAIN_MENU,		/* EVENT_2 */
		0,				/* EVENT_3 */
		0,				/* EVENT_4 */
	},
	{ /* STATE_JEDI_ORDER */
		0,				/* EVENT_START */
		STATE_JEDI_CODE,		/* EVENT_1 */
		STATE_MAIN_MENU,		/* EVENT_2 */
		0,				/* EVENT_3 */
		0,				/* EVENT_4 */
	},
	{ /* STATE_START_TESTING */
		0,				/* EVENT_START */
		0,				/* EVENT_1 */
		0,				/* EVENT_2 */
		0,				/* EVENT_3 */
		0,				/* EVENT_4 */
	},
	{ /* STATE_EXIT */
		0,				/* EVENT_START */
		0,				/* EVENT_1 */
		0,				/* EVENT_2 */
		0,				/* EVENT_3 */
		0,				/* EVENT_4 */
	},
};

static struct menu obj; /* singleton */

/* ---- private ------------------------------------------------------------- */

/**
 * Convert user input to event and validate it.
 *
 * @param in Input string from user
 * @param event Will contain parsed event
 * @return 0 on success or negative value on error
 */
static int menu_generate_event(const char *in, enum menu_event *event)
{
	char *s, *pos;
	int val, ret = 0;

	assert(in != NULL);
	assert(event != NULL);

	s = strdup(in);

	/* Remove trailing newline */
	if ((pos = strchr(s, '\n')) != NULL)
		*pos = '\0';

	ret = str2int(&val, s, 10);
	if (ret) {
		fprintf(stderr, "Error: Wrong input\n");
		ret = -1;
		goto exit;
	}

	if (val < 1 || val > 4) {
		fprintf(stderr, "Error: Wrong number\n");
		ret = -2;
		goto exit;
	}

	*event = val + EVENT_START;

exit:
	free(s);
	return ret;
}

static void menu_handle_state_main_menu(void)
{
	puts(menu_msg[STATE_MAIN_MENU]);
}

static void menu_handle_state_jedi_code(void)
{
	size_t i;

	for (i = 0; i < ARRAY_SIZE(jedi_code); ++i) {
		puts(jedi_code[i]);
		msleep(JEDI_CODE_DELAY);
	}
	puts("");
	clear_screen();
	puts(menu_msg[STATE_JEDI_CODE]);
}

static void menu_handle_state_jedi_order(void)
{
	puts(jedi_order);
	puts("");
	puts(menu_msg[STATE_JEDI_ORDER]);
}

static void menu_handle_state_start_testing(void)
{
	obj.cb(MENU_CB_START_TESTING);
}

static void menu_handle_state_exit(void)
{
	obj.cb(MENU_CB_EXIT);
}

static menu_handle_state_func_t menu_state_handler[STATE_NUM] = {
	NULL,					/* STATE_UNDEFINED */
	menu_handle_state_main_menu,		/* STATE_MAIN_MENU */
	menu_handle_state_jedi_code,		/* STATE_JEDI_CODE */
	menu_handle_state_jedi_order,		/* STATE_JEDI_ORDER */
	menu_handle_state_start_testing,	/* STATE_START_TESTING */
	menu_handle_state_exit,			/* STATE_EXIT */
};

/**
 * Process the transition for specified event.
 *
 * Run the function corresponding to current transition and set new state as
 * a current state.
 *
 * Complexity: O(1).
 *
 * @param event Event that user chose
 * @return 0 on success or negative value on error
 */
static int menu_handle_event(enum menu_event event)
{
	enum menu_state state = obj.state;	/* current state */
	enum menu_state new_state;
	menu_handle_state_func_t transition_func;

	if (event < EVENT_START || event > EVENT_4 ||
	    state < STATE_UNDEFINED || state > STATE_EXIT) {
		fprintf(stderr, "Error: Wrong transition: state = %d, "
				"event = %d\n", state, EVENT_START);
		return -1;
	}

	new_state = menu_transitions[state][event];
	if (new_state == STATE_UNDEFINED) {
		fprintf(stderr, "Error: Transition doesn't exist: state = %d, "
				"event = %d\n", state, EVENT_START);
		return -2;
	}

	transition_func = menu_state_handler[new_state];
	if (!transition_func) {
		fprintf(stderr, "Error: Transition function doesn't exist for "
				"state %d\n", new_state);
		return -3;
	}

	transition_func();
	obj.state = new_state;
	return 0;
}
void menu_init(menu_cb_t cb)
{
	obj.cb = cb;
	obj.state = STATE_UNDEFINED;
}

/**
 * De-initialize menu module.
 */
void menu_exit(void)
{
	memset(&obj, 0, sizeof(obj));
}

/**
 * Show main menu.
 *
 * @return 0 on success or negative value on failure
 */
int menu_start(void)
{
	return menu_handle_event(EVENT_START);
}

/**
 * Process new input data from user.
 *
 * @param data The data user has typed in
 * @return 0 on success or negative value on error
 */
int menu_inject(const char *data)
{
	int err;
	enum menu_event event;

	puts("");

	err = menu_generate_event(data, &event);
	if (err)
		return err;

	return menu_handle_event(event);
}
