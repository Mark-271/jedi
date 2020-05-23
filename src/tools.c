#include <tools.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

/* TODO: Maybe use existing TEMP_FAILURE_RETRY() instead? */
#define EINTR_LOOP(var, cmd)			\
	do {					\
		var = cmd;			\
	} while (var == -1 && errno == EINTR)

/**
 * Convert string to int.
 *
 * @param[out] out The converted int; cannot be NULL
 * @param[in] s Input string to be converted; cannot be NULL
 *              The format is the same as in strtol(), except that the
 *              following are inconvertible:
 *                - empty string
 *                - leading whitespace
 *                - any trailing characters that are not part of the number
 * @param[in] base Base to interpret string in. Same range as strtol (2 to 36)
 * @return 0 on success or negative value on error
 */
int str2int(int *out, char *s, int base)
{
	char *end;
	long l;

	assert(out);
	assert(s);
	assert(base >= 2 && base <= 36);

	if (s[0] == '\0' || isspace(s[0]))
		return -EINVAL;

	errno = 0;
	l = strtol(s, &end, base);

	/* Both checks are needed because INT_MAX == LONG_MAX is possible */
	if (l > INT_MAX || (errno == ERANGE && l == LONG_MAX))
		return -ERANGE;
	if (l < INT_MIN || (errno == ERANGE && l == LONG_MIN))
		return -ERANGE;
	if (*end != '\0')
		return -EINVAL;

	*out = l;
	return 0;
}

/**
 * Parse string to double.
 *
 * @param[out] out Will contain conversion result; must be not NULL
 * @param[in] s String to convert
 * @return 0 on success or negative on error
 */
int str2double(double *out, char *s)
{
	char *end;
	double d;

	assert(out);
	assert(s);

	if (s[0] == '\0' || isspace(s[0]))
		return -EINVAL;

	errno = 0;
	d = strtod(s, &end);
	if (errno == ERANGE)
		return -ERANGE;
	if (*end != '\0')
		return -EINVAL;

	*out = d;
	return 0;
}

/**
 * Sleep for interval specified in milli-seconds.
 *
 * @param msec Milli-seconds to sleep; can be greater than a second
 */
void msleep(unsigned long msec)
{
	int r;
	struct timespec wait = {
		.tv_sec = msec / 1000,
		.tv_nsec = (msec % 1000) * 1e6,
	};

	EINTR_LOOP(r, nanosleep(&wait, NULL));
}

void clear_screen(void)
{
	const char *clear_screen_ansi = "\e[1;1H\e[2J";
	int r;

	EINTR_LOOP(r, write(STDOUT_FILENO, clear_screen_ansi, 12));
}
