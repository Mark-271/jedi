#ifndef TOOLS_H
#define TOOLS_H

#define UNUSED(v)	((void)v)
#define BIT(n)		(1 << (n))
#define ARRAY_SIZE(a)	(sizeof(a) / sizeof(a[0]))

int str2int(int *out, char *s, int base);
int str2double(double *out, char *s);
void msleep(unsigned long msec);
void clear_screen(void);

#endif /* TOOLS_H */
