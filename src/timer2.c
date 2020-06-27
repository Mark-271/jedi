/**
 * @file
 *
 * Timer class: Linux implementation.
 *
 * Implement timer routines for Linux using timer_t. As timer_t is POSIX
 * extension, it might be not available for other POSIX compliant OSes, like
 * MacOS or FreeBSD. We will implement timer routines in other way there. Linux
 * implements timer_t extension, so let's use it for Linux and Android.
 */

#include <timer2.h>
#include <assert.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct timer2 {
	timer_t timer_id;
	struct sigevent se;
	timer2_cb_t cb;
	void *cb_priv;
};

static void timer2_thread_func(union sigval sv)
{
	struct timer2 *obj;

	obj = sv.sival_ptr;
	obj->cb(obj->cb_priv);
}

/**
 * Start the timer.
 *
 * @param obj Timer object
 * @param period Timer period, msec
 * @param once One-shot if true, or periodic if false
 * @return true on success or false on failure
 */
bool timer2_start(struct timer2 *obj, int period, bool once)
{
	struct itimerspec its;
	int res;

	/* Initial expiration */
	its.it_value.tv_sec = period / 1000;
	its.it_value.tv_nsec = (period % 1000) * 1e6;

	/* Timer interval */
	if (once) {
		its.it_interval.tv_sec = 0;
		its.it_interval.tv_nsec = 0;
	} else {
		its.it_interval.tv_sec = its.it_value.tv_sec;
		its.it_interval.tv_nsec = its.it_value.tv_nsec;
	}

	/* Arm the timer */
	res = timer_settime(obj->timer_id, 0, &its, NULL);
	return res == 0;
}

/**
 * Stop the timer.
 *
 * @param obj Timer object
 * @return true on success or false on fail
 */
bool timer2_stop(struct timer2 *obj)
{
	struct itimerspec its;
	int res;

	memset(&its, 0, sizeof(its));
	res = timer_settime(obj->timer_id, 0, &its, NULL);
	return res == 0;
}

/**
 * Create timer object.
 *
 * @param cb Callback to call on timer tick
 * @param priv Private data to pass to callback
 * @return Timer object or NULL on failure
 */
struct timer2 *timer2_create(timer2_cb_t cb, void *priv)
{
	struct timer2 *obj;
	int res;

	assert(cb != NULL);

	obj = malloc(sizeof(*obj));
	memset(obj, 0, sizeof(*obj));

	obj->cb = cb;
	obj->cb_priv = priv;

	obj->se.sigev_notify		= SIGEV_THREAD;
	obj->se.sigev_value.sival_ptr	= obj;
	obj->se.sigev_notify_function	= timer2_thread_func;
	obj->se.sigev_notify_attributes	= NULL;

	res = timer_create(CLOCK_REALTIME, &obj->se, &obj->timer_id);
	if (res == -1)
		goto err;

	return obj;

err:
	free(obj);
	return NULL;
}

/**
 * Destroy timer object.
 *
 * @param obj Timer object
 */
void timer2_destroy(struct timer2 *obj)
{
	timer_delete(obj->timer_id);
	free(obj);
}
