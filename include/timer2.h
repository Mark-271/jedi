#ifndef TIMER2_H
#define TIMER2_H

#include <stdbool.h>

typedef void (*timer2_cb_t)(void *priv);

struct timer2;

struct timer2 *timer2_create(timer2_cb_t cb, void *priv);
void timer2_destroy(struct timer2 *obj);
bool timer2_start(struct timer2 *obj, int period, bool once);
bool timer2_stop(struct timer2 *obj);

#endif /* TIMER2_H */
