#ifndef _SRT_H_
#define _SRT_H_

struct rules_s {
  char *_name; // can be freed
  char *_rule;
  char *_pre_rule;
  char *_line; // can be freed
  struct rules_s *_next;
  struct rules_s *_prev;
};

typedef struct rules_s rules_t;

struct graph_slr_s {
  char *_symbol;
  struct graph_slr_s **_follow;
};

int		init(const char *);

#endif
