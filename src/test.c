/**
 * @file
 *
 * Implementation of Jedi Testing System test module.
 *
 */

#include <test.h>
#include <timer2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define BUF_SIZE 80

enum testing_state {
	QUESTION_1,
	QUESTION_2,
	QUESTION_3,
	QUESTION_4,
	QUESTION_5,
	/* --- */
	QUESTION_NUM
};

static const char * const questions[QUESTION_NUM] = {
	"There is no emotion, there is ...\n",
	"There is no ignorance, there is ...\n",
	"There is no passion, there is ...\n",
	"There is no chaos, there is ...\n",
	"There is no death, there is ...\n",
};

static const char * const answers[QUESTION_NUM+1] = {
	"peace",
	"knowledge",
	"serenity",
	"harmony",
	"the Force",
	"Force",
};

/* ---- private ------------------------------------------------------------- */
/**
 * Read user input to handle test questions.
 *
 * @param buf Array to store string from user
 */
static void read_answer(char *buf)
{
	char *s;
	s = fgets(buf, sizeof(buf) * BUF_SIZE, stdin);
	if (!s) {
		fprintf(stderr, "Error: wromg input\n");
		return;
	}

	/* Remove trailing newline */
	size_t len = strlen(buf);
	if (len > 0 && buf[len-1] == '\n')
		buf[--len] = '\0';
	return;
}

/**
 * Functions to handle answers by comparing user's input
 * with corresponding pattern.
 *
 * @param buf contains modified string from user
 * @return true on success or false on failure
 */
static bool test_handle_answer1(char *buf)
{
	int ret;
	ret = strcmp(buf, answers[QUESTION_1]);
	if(ret == 0)
		return true;
	else
		return false;
}

static bool test_handle_answer2(char *buf)
{
	int ret;
	ret = strcmp(buf, answers[QUESTION_2]);
	if(!ret)
		return true;
	else
		return false;
}

static bool test_handle_answer3(char *buf)
{
	int ret;
	ret = strcmp(buf, answers[QUESTION_3]);
	if(!ret)
		return true;
	else
		return false;
}

static bool test_handle_answer4(char *buf)
{
	int ret;
	ret = strcmp(buf, answers[QUESTION_4]);
	if(!ret)
		return true;
	else
		return false;
}

static bool test_handle_answer5(char *buf)
{
	int ret;
	ret = strcmp(buf, answers[QUESTION_5]);
	if(!ret)
		return true;
	else {
		ret = strcmp(buf, answers[QUESTION_NUM]);
		if(!ret)
			return true;
		return false;
	}
}

/**
 * Function that provides procedure for handling answers from user.
 *
 * @return 0 on success or 1 on failure.
 */
static int test_handle_answers(void)
{
	char buf[BUF_SIZE];
	size_t num = 0;

	while (num < QUESTION_NUM) {
		switch (num) {
			case QUESTION_1:
				puts(questions[QUESTION_1]);
				read_answer(buf);
				if (!test_handle_answer1(buf))
					return 1;
				++num;
				break;
			case QUESTION_2:
				puts(questions[QUESTION_2]);
				read_answer(buf);
				if (!test_handle_answer2(buf))
					return 1;
				++num;
				break;
			case QUESTION_3:
				puts(questions[QUESTION_3]);
				read_answer(buf);
				if (!test_handle_answer3(buf))
					return 1;
				++num;
				break;
			case QUESTION_4:
				puts(questions[QUESTION_4]);
				read_answer(buf);
				if (!test_handle_answer4(buf))
					return 1;
				++num;
				break;
			case QUESTION_5:
				puts(questions[num]);
				read_answer(buf);
				if (!test_handle_answer5(buf))
					return 1;
				++num;
				break;
		}
	}
	return 0;
}

/* Function that defines behaviour of the program after timer expires */
static void timeout(void *priv)
{
	struct timer2 *obj = (struct timer2 *)priv;
	char *msg = "Sorry, you out of time... Try again later.";

	printf("%s\n", msg);

	timer2_destroy(obj);
	exit(1);
}

/* --- public ----------------------------------------------------------------*/

/**
 * Function that provides testing procedure.
 *
 * @return 0 on success or 1 on failure
 * when timer expires, function returns 1
 */
int test_exercise(void)
{
	int ret;
	struct timer2 *tm = NULL;

	tm = timer2_create(timeout, tm);

	timer2_start(tm, 30000, true);

	ret = test_handle_answers();
	timer2_destroy(tm);

	if(!ret) {
		puts("Congratulations, young Padawan! You know Jedi Code.\n"
		     "May the Force be with you!");
		exit(0);
	}
	else {
		puts("Alas, young Padawan, you are not ready "
		     "for the path yet!");
		exit(1);
	}
}
