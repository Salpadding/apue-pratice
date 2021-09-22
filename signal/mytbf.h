#ifndef MYTBF_H__
#define MYTBF_H__

// create a job, return -1 if failed
// cps = amount of token per second, burst = the upper bound of token
int mytbf_init(int cps, int burst);

// return tokens if unused
int mytbf_returntoken(int id, int token);

// block until token fetched
int mytbf_fetchtoken(int id, int token);

// destroy job
int mytbf_destroy(int id);

#endif
