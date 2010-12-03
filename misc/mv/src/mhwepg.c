//#define DEBUG
/*
 *  mhwepg.c	EPG for Canal+ bouquets
 *
 *  (C) 2002, 2003 Jean-Claude Repetto <mhwepg@repetto.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 * Or, point your browser to http://www.gnu.org/copyleft/gpl.html
 * 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#if defined(HAVE_LINUX_DVB_DMX_H)

#define DVB_API_VERSION 3
#include <linux/dvb/dmx.h>
#define DEMUX_DEVICE_MASK    "/dev/dvb/adapter%d/demux%d"

#elif defined(HAVE_OST_DMX_H)
// dreambox
#define DVB_API_VERSION 1
#include <ost/dmx.h>
#define DEMUX_DEVICE_MASK    "/dev/dvb/card%d/demux%d"
#define dmx_pes_filter_params dmxPesFilterParams
#define dmx_sct_filter_params dmxSctFilterParams

#endif

#include "mhwepg.h"

#ifdef DEBUG
const char *day_names[] = {
   "Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
#endif

extern unsigned char buffer[MAX_SECTION_BUFFER];	/* General data buffer */

#define MAX_CHANNELS 180

char channels[MAX_CHANNELS][50];
char channel_ids[MAX_CHANNELS][50];
char themes[256][16];
extern char channel_equiv[MAX_CHANNELS][2][50];
extern int nb_equiv;

#define MAX_PROGRAMS 25000
struct program * programs[MAX_PROGRAMS];
struct summary * summaries[MAX_PROGRAMS];

char device [30];
char source [50];

int nb_channels;
int nb_programs;
int nb_summaries;
int hour_index [24*7];

/* Local Time offsets */
extern int current_offset, next_offset;
extern time_t time_of_change;
extern int time_offset_set ;
extern int yesterday;	/* Day of the week of yesterday */
extern time_t yesterday_epoch;	/* Epoch of yesterday 00:00 in provider local time */

extern char RepNL;/* character to replace nl */
/*
 *	Removes trailing spaces from a string
 */

void strclean (char *s)
{
   char * end=s;
   while (*s)
   {
      if (!isspace (*s))
         end = s+1;
      s++;
   }
   *end = 0;
}

/*
 *	Compare country code and region
 */

int timezonecmp(item_local_time_offset_t *item_local_time_offset,
   			 const char * country, int region)
{
   return item_local_time_offset->country_code1 == country[0] &&
          item_local_time_offset->country_code2 == country[1] &&
          item_local_time_offset->country_code3 == country[2] &&
          item_local_time_offset->region_id == region ? 0 : 1;
}


/*
 *	Get channel names (PID=0xD3, TID=0x91)
 */

int GetChannelNames(int list_channels)
{
   int fd,i,n;
   unsigned short sid;
   channel_name_t *channel_name;
   
   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd, 211, 0x91);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading channels names\n");
      perror(":");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   channel_name = (channel_name_t *) (buffer+4);
   nb_channels = (n-4)/sizeof(channel_name_t);
   if (nb_channels >= MAX_CHANNELS)
   {
      fprintf (stderr, "Channels Table overflow (%d names needed)\n", nb_channels);
      return -1;
   }

   for (i=0; i<nb_channels; i++)
   {
      sid = HILO (channel_name->channel_id);

      memcpy (channels[i], channel_name -> name, 16);
      channels[i][16] = 0;
      /* Remove trailing blanks */
      strclean (channels[i]);
      if (source[0] != 0)
	      sprintf (channel_ids[i], "%s-%d-%d-%d",source,HILO (channel_name->network_id),HILO (channel_name->transponder_id), HILO (channel_name->channel_id));
      else
	      sprintf (channel_ids[i], "%d", HILO (channel_name->channel_id));
#ifdef DEBUG
      fprintf (stderr,"SID = %s   CHANNEL = '%s'\n",channel_ids[i], channels[i]);
#endif
	if (list_channels)
	      fprintf (stderr,"%s %s\n",channel_ids[i], channels[i]);
      channel_name++;
   }
   return 0;
}

/*
 *	Get themes names (PID=0xD3, TID=0x92)
 */

int GetThemes (void)
{
   int fd,i,n,m,theme,nb;
   unsigned char * themes_index;
   typedef unsigned char theme_name[15];
   theme_name * themes_names;
   
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }

   SetFiltSection (fd, 211, 0x92);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr,"Error while reading themes\n");
      perror(":");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   themes_index = buffer+3;	/* Skip table ID and length */
   themes_names = (theme_name *) (buffer+19);	/* Start of names table */
   theme = 0;			
   m = 0;
   nb = (n-19)/15;	/* Number of themes */
   for (i=0; i<nb; i++)
   {
      if (themes_index[theme] == i)	/* New theme */
      {
         m = (m + 15) & 0xF0;
         theme++;
      }
      memcpy (themes[m], themes_names, 15);
      themes[m][15] = 0;
      strclean(themes[m]);
#ifdef DEBUG
      fprintf (stderr,"%02X '%s'\n", m, themes[m]);
#endif
      m++;
      themes_names++;
   }
   return 0;
}

/*
 *	Convert provider local time to UTC
 */
time_t ProviderLocalTime2UTC (time_t t)
{
   if (t < time_of_change)	/* ---- FIX ME ---- */
      return t - current_offset;
   else
      return t - next_offset;
}

/*
 *	Convert UTC to provider local time
 */
time_t UTC2ProviderLocalTime (time_t t)
{
   if (t < time_of_change)
      return t + current_offset;
   else
      return t + next_offset;
}

/*
 *	Convert local time to UTC
 */
time_t LocalTime2UTC (time_t t)
{
	struct tm *temp;

	temp = gmtime(&t);
	temp->tm_isdst = -1;
	return mktime (temp);
}

/*
 *	Convert UTC to local time
 */
time_t UTC2LocalTime (time_t t)
{
	return 2 * t - LocalTime2UTC(t);
}

/*
 *	Parse TOT
 */

int ParseTOT(void)
{
   int fd,n,i;
   u_char *ptr;
   tot_t *tot;	/* Time offset table */
   descr_local_time_offset_t *descriptor;
   item_local_time_offset_t *item_local_time_offset;
   time_t diff_time, utc_offset, local_time_offset, next_time_offset,current_time;
//#ifdef DEBUG
   struct tm *change_time;
//#endif

   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd, 0x14, 0x73);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading Time Offset Table\n");
      return -1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);

   /* Some providers does not send a correct UTC time, vg Cyfra + */
   tot = (tot_t *) buffer;
   current_time = time(NULL);
   diff_time = MjdToEpochTime(tot->utc_mjd) + BcdTimeToSeconds(tot->utc_time)
      		- current_time;

   /* Round */
   utc_offset = ((diff_time + 1800) / 3600) * 3600;
   fprintf (stderr,"UTC Offset = %d\n",(int)utc_offset);

   /* Parse the descriptors */
   ptr = (u_char *) (tot+1);
   n -= TOT_LEN + 4;	/* Descriptors length */
   while (n > 0)
   {
      descriptor = CastLocalTimeOffsetDescriptor (ptr);
      if (descriptor->descriptor_tag == DESCR_LOCAL_TIME_OFF)
      {
         item_local_time_offset = CastItemLocalTimeOffset(descriptor+1);
         for (i=0; i < descriptor->descriptor_length/ITEM_LOCAL_TIME_OFFSET_LEN; i++)
         {			 /* Get the local time offset */
            fprintf (stderr,"Country = %c%c%c, Region = %d\n", item_local_time_offset->country_code1,
               		item_local_time_offset->country_code2, item_local_time_offset->country_code3,
                        item_local_time_offset->region_id);
            if (!timezonecmp(item_local_time_offset, "FRA", 0) ||
                !timezonecmp(item_local_time_offset, "POL", 0) ||
                !timezonecmp(item_local_time_offset, "NLD", 0) ||
                !timezonecmp(item_local_time_offset, "NED", 0) ||
                !timezonecmp(item_local_time_offset, "GER", 0) ||
                !timezonecmp(item_local_time_offset, "DEU", 0) ||
                !timezonecmp(item_local_time_offset, "ITA", 0) ||
                !timezonecmp(item_local_time_offset, "DNK", 0) ||
                !timezonecmp(item_local_time_offset, "SWE", 0) ||
                !timezonecmp(item_local_time_offset, "FIN", 0) ||
                !timezonecmp(item_local_time_offset, "NOR", 0) ||
                !timezonecmp(item_local_time_offset, "ESP", 1))
 
            {
               local_time_offset = 60 * BcdTimeToMinutes(item_local_time_offset->local_time_offset);
               next_time_offset = 60 * BcdTimeToMinutes(item_local_time_offset->next_time_offset);
               if (item_local_time_offset->local_time_offset_polarity)
               {
                  local_time_offset = -local_time_offset;
                  next_time_offset = -next_time_offset;
               }
               time_of_change = MjdToEpochTime(item_local_time_offset->toc_mjd)
                  		 + BcdTimeToSeconds(item_local_time_offset->toc_time);
            
               current_offset = local_time_offset + utc_offset;
               next_offset = next_time_offset + utc_offset;
//#ifdef DEBUG
               fprintf (stderr,"Local Time Offset = %d\n", current_offset);
               fprintf (stderr,"Next Local Time Offset = %d\n", next_offset);
               change_time = gmtime(&time_of_change);
               fprintf (stderr,"Date of change = %02d/%02d/%4d\n",change_time->tm_mday,
                  	change_time->tm_mon+1,change_time->tm_year+1900);
//#endif
               return 0;
            }
            item_local_time_offset++;
         }    
      }
      ptr += descriptor->descriptor_length+DESCR_LOCAL_TIME_OFFSET_LEN;
      n -= descriptor->descriptor_length+DESCR_LOCAL_TIME_OFFSET_LEN;
   }
   return -1;
}

/*
 *	Get Local Time Offset
 */

int GetTimeOffset(void)
{
   struct tm *cur_time;
   time_t yesterday_time;

   if (!time_offset_set)
   {
      if (ParseTOT() != 0)
      {
         printf ("No local time in the TOT table. Please use the -t option.\n");
         return -1;
      }
   }

   /* Get day of the week of yesterday (provider local time) */
// yesterday_time = UTC2ProviderLocalTime (time(NULL) - 86400);
   yesterday_time = UTC2LocalTime (time(NULL) - 86400);
   cur_time = gmtime(&yesterday_time);
   yesterday = cur_time->tm_wday;

   /* Get epoch of yesterday 00:00 (provider local time) */
   cur_time->tm_hour = 0;
   cur_time->tm_min = 0;
   cur_time->tm_sec = 0;
   cur_time->tm_isdst = -1;
// yesterday_epoch = UTC2ProviderLocalTime(mktime (cur_time));
   yesterday_epoch = UTC2LocalTime(mktime (cur_time));
   return 0;
}

/*
 *	Get Program Titles (PID=0xD2, TID=0x90)
 */

int GetTitles(void)
{
   int fd,n,d,h,i;
   char first_buffer[EPG_TITLE_LEN];
   short end_flag;
   struct program * program;
   epg_title_t *epgtitle = (epg_title_t *) buffer;
   time_t broadcast_time; 

   nb_programs=0;
   for (i=0; i<24*7; i++)
      hour_index[i] = -1;

   if((fd = open(device,O_RDWR)) < 0)
   {
      perror("DEMUX DEVICE ");
      return -1;
   }
   SetFiltSection(fd,210,0x90);

   /* Skip also the titles until the next separator */
   do
   {
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         fprintf (stderr,"Error while reading titles\n");
         perror(":");
         return -1;
      }
      if (n != EPG_TITLE_LEN)
         fprintf (stderr,"Warning, wrong size table : %d\n",n);
   } while (n != EPG_TITLE_LEN || epgtitle->channel_id != 0xFF);

   fprintf (stderr,"Reading titles ...\n");

   /* Store the separator */
   memcpy (first_buffer, buffer, EPG_TITLE_LEN);
   end_flag =0;
   while (!end_flag)
   {
	if (n == EPG_TITLE_LEN)
	{
	   /* Get Day and Hour of the program */
	      d = epgtitle->day;
	      if (d == 7)
	         d = 0;
	      h = epgtitle->hours;
	      if (h>15)
        	 h = h-4;
	      else if (h>7)
        	 h = h-2;
	      else
        	 d = (d==6) ? 0 : d+1;
     
	      if (epgtitle->channel_id == 0xFF)		/* Separator */
	      {
#ifdef DEBUG
	      	  fprintf (stderr,"-------------------------------\n");
#endif
	         hour_index[d*24+h] = nb_programs;
	      }
	      else
	      {
	         program = malloc (sizeof (struct program));
	         programs[nb_programs++] = program;

        	 broadcast_time = (d-yesterday) * 86400 + h * 3600 + epgtitle->minutes * 60;
	         if (broadcast_time < 6 * 3600)		/* After yesterday 06:00 ? */
        	    broadcast_time += 7 * 86400;	/* Next week */
	         broadcast_time += yesterday_epoch;
		 broadcast_time = ProviderLocalTime2UTC (broadcast_time);
//		 broadcast_time = LocalTime2UTC (broadcast_time);

	         program -> channel_id = epgtitle->channel_id-1;
        	 program -> theme_id = epgtitle->theme_id;
	         program -> time = broadcast_time;
        	 program -> summary_available = epgtitle->summary_available;
	         program -> duration = HILO(epgtitle->duration) * 60;
        	 memcpy (program -> title, &epgtitle->title, 23);
	         program -> title[23] = 0;
        	 program -> ppv = HILO32 (epgtitle->ppv_id);
	         program -> id = HILO32 (epgtitle->program_id);

#ifdef DEBUG
	         fprintf (stderr,"\n%s %s -  ",channel_ids[program -> channel_id], channels[program -> channel_id]);
        	 fprintf (stderr,"%s ", day_names[d]);
	         fprintf (stderr,"%2d:%02d ", h, epgtitle->minutes);

	         /* Duration */
        	 fprintf (stderr,"%4d' ", program -> duration / 60);
	         fprintf (stderr,"%10d ", program -> ppv);
        	 fprintf (stderr,"%10d ", program -> id);
	         fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");
        	 fprintf (stderr,"%s", ctime(&program -> time));
         
	         fprintf (stderr,"%s - ", themes[program -> theme_id]);
        	 fprintf (stderr,"%s\n",program -> title);
#endif
	      }
	}
      /* Read the next program title */
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         fprintf (stderr,"Error while reading titles\n");
         perror(":");
         return -1;
      }
      if (n != EPG_TITLE_LEN)
         fprintf (stderr,"Warning, wrong size table : %d\n",n);
      /* Check if we've got all the titles */
      if (!memcmp (first_buffer,buffer,n))
         end_flag=1;
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

/*
 *	Get Program Summaries (PID=0xD3, TID=0x90)
 */
extern int fd211;
extern char buffer3[];

int GetSummaries(void)
{
   int fd,i,j,n;
   short end_flag;
   char first_buffer[EPG_SUMMARY_LEN];
   struct summary * summary;
   epg_summary_t *epgsummary = (epg_summary_t *) buffer;
   epg_replay_t *epg_replay;
   
   fprintf (stderr,"Reading summaries ...\n");
   nb_summaries=0;
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }
   SetFiltSection(fd,211,0x90);

   /* Read Summaries */
   do {
   	n = read(fd,buffer,MAX_SECTION_BUFFER);
        if (n <= 0)
        {
           fprintf (stderr,"Error while reading summaries\n");
           perror("read:");
           return -1;
        }
   }
   while (n < 12 || buffer[7] != 0xFF || buffer[8] != 0xFF || buffer[9] !=0xFF || buffer[10] >= 10);	/* Invalid Data */

   memcpy (first_buffer, buffer, EPG_SUMMARY_LEN);	/* Store the first summary */

   end_flag =0;
   while (!end_flag)
   {
      n = read(fd,buffer,MAX_SECTION_BUFFER);
      if (n <= 0)
      {
         perror("Error while reading summaries : ");
         exit(1);
      }
      if (n < 12 || buffer[7] != 0xFF || buffer[8] != 0xFF || buffer[9] !=0xFF || buffer[10] >= 10)	/* Invalid Data */
         continue;
         
      if (!memcmp (first_buffer,buffer,EPG_SUMMARY_LEN))  /* Got all summaries ? */
         end_flag=1;

      buffer[n] = 0;	/* String terminator */

      summary = malloc (sizeof (struct summary));
      summaries[nb_summaries++] = summary;

      summary -> id = HILO32 (epgsummary->program_id);
      /* Index of summary text beginning */
      j = EPG_SUMMARY_LEN + epgsummary->nb_replays*EPG_REPLAY_LEN;
      summary -> text = malloc (n+1 - j);

      if (summary -> text == NULL)
      {
         fprintf (stderr,"Memory allocation error %d\n",n+1-j);
#ifdef DEBUG
         fprintf (stderr,"n=%d j=%d nb_replays=%d\n",n,j,epgsummary->nb_replays);
         for (i=0; i<n; i++)
            fprintf (stderr,"%02X ",buffer[i]);
         fprintf (stderr,"\n%s\n", buffer+10);
#endif
         exit(1);
      }
      /* Copy the summary and replace line feeds by the "|" character */
      i=0;
      do
      {
         summary -> text[i] = buffer[j+i] == '\n' ? RepNL : buffer[j+i];
         i++;
      }
      while (buffer[j+i-1] != 0);
      summary -> nb_replays = epgsummary->nb_replays;

      /* Get the times of replays */
      epg_replay = (epg_replay_t *) (epgsummary+1);
      for (i=0; i< summary -> nb_replays; i++)
      {
         summary -> time[i] = MjdToEpochTime(epg_replay->replay_mjd) +
            			BcdTimeToSeconds(epg_replay->replay_time);
       summary -> time[i] = ProviderLocalTime2UTC (summary -> time[i]);
//         summary -> time[i] = LocalTime2UTC (summary -> time[i]);
         summary -> channel_id[i] = epg_replay->channel_id-1;
         summary -> subtitles[i] = epg_replay->subtitles;
         summary -> vm[i] = epg_replay->vm;
         summary -> vo[i] = epg_replay->vo;
         summary -> last_replay[i] = epg_replay->last;
         epg_replay++;
      }
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

int SaveEpgData(char * filename)
{
   FILE *file;
   struct summary * summary;
   int i,j,n,equiv_index;
   struct program * program;
   struct tm *next_date;
   int first_program;
   int index;
   u_char new_channel;
   time_t current_time;
   time_t first_time;
   char channel_id[50];
   char long_title[81];
   int summary_found;
   char *ptr;
   
   fprintf (stderr,"Writing EPG data to file ...\n");
   current_time = time(NULL);
   /* Start from yesterday 6:00 */
   //j = yesterday*24+6;
   //for (i=0; i<24*7; i++)
   //   if ((first_program = hour_index[(i+j) % 24*7]) != -1)
   //      break;
   first_time= programs[0]->time;
   first_program=0;
   for(i=1 ;  i<nb_programs ; i++)
       if(programs[i]->time < first_time)
       {
        first_time= programs[i]->time;
        first_program=i;
       }
#ifdef DEBUG
            fprintf (stderr,"First program = %d\n", first_program);
#endif
   if (first_program == -1)
      return -1;	/* No program */
   
   if (filename[0] != 0)
	file = fopen (filename,"w");
   else
        file = stdout;
   fprintf (stderr,"Nb of channels = %d\n", nb_channels);
   for (index = 0; index < nb_channels; index++)
   {
      strcpy (channel_id, channel_ids[index]);
      for (equiv_index=-1; equiv_index<nb_equiv; equiv_index++)
      {
         if (equiv_index >= 0)
         {
#ifdef DEBUG
            fprintf (stderr, "Equiv index = %d : '%s' '%s'\n",equiv_index,channel_ids[index], channel_equiv[equiv_index][0]);
#endif
            if (strcmp(channel_ids[index], channel_equiv[equiv_index][0]))
               continue;
            else
               strcpy (channel_id, channel_equiv[equiv_index][1]);
         }
         new_channel = 1;
         for (i=0; i<nb_programs; i++)
         {
            program = programs[(first_program+i)%nb_programs];

            if (program -> channel_id == index)
            {
               if (program->time + program->duration > current_time)
               {
                  if (new_channel)
                  {
                     fprintf (file, "C %s %s\n", channel_id, channels[index]);
                     new_channel = 0;
                  }
                  fprintf (file, "E %d %d %d 90\n", i, (int)(program->time), program->duration);

#ifdef DEBUG
                  fprintf (stderr,"%s %s\n",channel_id, channels[index]);
                  fprintf (stderr,"%s", ctime(&program -> time));

                  /* Duration */
                  fprintf (stderr,"%4d' ", program -> duration / 60);
                  fprintf (stderr,"%10d ", program -> ppv);
                  fprintf (stderr,"%10d ", program -> id);
                  fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");

                  fprintf (stderr,"%s\n",program -> title);
                  fprintf (stderr,"%s ", themes[program -> theme_id]);
#endif
                  summary_found = 0;
                  if (program -> summary_available)
                  {
                     for (j=0; j<nb_summaries; j++)
                        if (program -> id == summaries[j] -> id)
                        {
                           summary_found = 1;
                           summary = summaries[j];
                           strclean (summary -> text);

                           strncpy (long_title, program -> title, sizeof(long_title));
                           strclean (long_title);
                           if ((ptr = strstr (long_title, "...")) != NULL)
                             if (strlen(ptr) == 3)
                             {
                                *ptr = 0;    // Remove ...
                                if ((ptr=strstr (summary -> text, long_title)) != NULL)
                                {
                                   strncpy (long_title, ptr, sizeof(long_title));
                                   if ((ptr=strchr(long_title, '|')) != NULL)
                                      *ptr=0;
                                   else
                                      long_title [sizeof(long_title) - 1] = 0;
                                }
                                else
                                   strncpy (long_title, program -> title, sizeof(long_title));
                             }

                           fprintf (file, "T %s\n", long_title);
                  	   fprintf (file, "X %d\n",  (int)(program->time));
                           fprintf (file, "S %s - %d'\n", themes[program -> theme_id], program -> duration / 60);
                  	   fprintf (file, "L %d\n", program->duration/60);

                           fprintf (file, "D %s", summary -> text);
#ifdef DEBUG
                           fprintf (stderr,"%s\n", summary -> text);
#endif
                           for (n=0; n<summary -> nb_replays; n++)
                           {
                              next_date = localtime (&summary->time[n]);
                              if (n == 0)
                                 fprintf (file,"%cRediffusions:",RepNL);
                              fprintf (file,"%c%02d/%02d/%4d ",RepNL,next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (file,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (file," %s",channels[summary -> channel_id[n]]);
                              if (summary -> vm[n])
                                 fprintf (file," - VM");
                              if (summary -> vo[n])
                                 fprintf (file," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (file," - ST");
                              if (summary -> last_replay[n])
                                 fprintf (file," - Dernière diffusion");

#ifdef DEBUG
                              if (summary -> channel_id[n] < nb_channels)
	                              fprintf (stderr,"%s ",channels[summary -> channel_id[n]]);
                              fprintf (stderr,"%2d/%02d/%4d ",next_date->tm_mday,next_date->tm_mon+1,next_date->tm_year+1900);
                              fprintf (stderr,"%02d:%02d",next_date->tm_hour, next_date->tm_min);
                              if (summary -> vm[n])
                                 fprintf (stderr," - VM");
                              if (summary -> vo[n])
                                 fprintf (stderr," - VO");
                              if (summary -> subtitles[n])
                                 fprintf (stderr," - ST");
                                 if (summary -> last_replay[n])
                                    fprintf (stderr," - Last");
                                 fprintf (stderr,"\n");
#endif
                           }
                           fprintf (file, "\n");
                       }
                   }
                   if (!summary_found)
                   {
                       fprintf (file, "T %s\n",program -> title);
                       fprintf (file, "S %s - %d'\n", themes[program -> theme_id], program -> duration / 60);
                   }
                   fprintf (file, "e\n");
               }
#ifdef DEBUG
               fprintf (stderr,"\n");
#endif
            }
         }
         if (!new_channel)
            fprintf (file, "c\n");
      }
   }
   if (filename[0] != 0)
	fclose (file);
   return 0;
}
extern   char filename [100];

void do_mhwepg(int list_channels)
{
   if (GetTimeOffset())
      exit(1);

   if (GetThemes())
      exit(1);

   if (GetChannelNames(list_channels))
      exit(1);

   if (GetTitles())
      exit(1);

   if (GetSummaries())
      exit(1);

   fprintf (stderr,"Nb of summaries : %d\n",nb_summaries);
   fprintf (stderr,"Nb of titles : %d\n",nb_programs);

   if (SaveEpgData(filename))
      exit(0);
}
