#ifndef EPG_H
#define EPH_H

#include "si_tables.h"

/* Structures for EPG tables parsing */
typedef struct {
   u_char	network_id_hi;
   u_char	network_id_lo;
   u_char	transponder_id_hi;
   u_char	transponder_id_lo;
   u_char	channel_id_hi;
   u_char	channel_id_lo;
} channel_name_t;



struct program {
      u_char channel_id;
      u_char theme_id;
      time_t time;
      u_char summary_available;
      u_short duration;
      char title[65];
      u_int ppv;
      u_int id;
};

struct summary {
      u_int id;
      char * text;
      u_char nb_replays;
      time_t time [10];
      u_char channel_id [10];
      u_char subtitles [10];
      u_char vm [10];
      u_char vo [10];
      u_char last_replay [10];
};

/* Functions prototypes */

void SetFiltSection (int fd, unsigned short pid, unsigned char tid);

#endif

