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

//#define MAX_SECTION_BUFFER 65537
extern unsigned char buffer[MAX_SECTION_BUFFER+1];	/* General data buffer */

#define MAX_CHANNELS 180

extern char channels[MAX_CHANNELS][50];
extern char channel_ids[MAX_CHANNELS][50];
extern char themes[256][32];
int themes_first[100];
extern char channel_equiv[MAX_CHANNELS][2][50];
extern int nb_equiv;

#define MAX_PROGRAMS 25000
extern struct program * programs[MAX_PROGRAMS];
extern struct summary * summaries[MAX_PROGRAMS];

extern char device [30];
extern char source [50];

extern int nb_channels;
extern int nb_programs;
extern int nb_summaries;
extern int hour_index [24*7];

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

extern void strclean (char *s);

/*
 *	Compare country code and region
 */

extern int timezonecmp(item_local_time_offset_t *item_local_time_offset,
   			 const char * country, int region);

/*
 *	Set a filter for a DVB stream
 */

extern void SetFiltSection (int fd, unsigned short pid, unsigned char tid);

/*
 *	Get channel names (PID=561 , TID=200 )
 */

int GetChannels(int list_channels)
{
 channel_name_t *channel_name;
 int i;
 unsigned short sid;
   channel_name = (channel_name_t *) (buffer+121);
   nb_channels = buffer[120];
   int posname=121+6*nb_channels;
   if (nb_channels >= MAX_CHANNELS)
   {
      fprintf (stderr, "Channels Table overflow (%d names needed)\n", nb_channels);
      return -1;
   }

   for (i=0; i<nb_channels; i++)
   {
      sid = HILO (channel_name->channel_id);

      memcpy (channels[i], buffer+posname+1,buffer[posname]&0x0f);
      channels[i][buffer[posname]&0x0f] = 0;
      posname += (buffer[posname]&0x0f)+1;
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

int GetThemes2 (int list_channels)
{
 int i,j,lgr,m,theme,nb,nb2;
 unsigned short ind_theme;
 unsigned short ind_str;
 unsigned char *themes_index;
   
   themes_index = buffer+3;	/* Skip table ID and length */
   theme = 0;			
   m = 0;
   nb = buffer[4];	/* Number of themes */
   for (i=0; i<nb; i++)
   {
     themes_first[theme]=i;
     ind_theme = (themes_index[i*2+2]<<8)+ themes_index[i*2+3];
     nb2 = themes_index[ind_theme]&0x3f;
     for(j=0 ; j<=nb2 ; j++)
     {
      ind_str = (themes_index[ind_theme+1+j*2]<<8) + themes_index[ind_theme+2+j*2];
      lgr=themes_index[ind_str]&0x1f;
      memcpy (themes[theme], themes_index+ind_str+1,lgr);
      themes[theme][lgr] = 0;
#ifdef DEBUG
      fprintf (stderr,"%02X '%s'\n", m, themes[theme]);
#endif
      if(list_channels)
	      printf(" theme %d:%d = %s.\n",i,j,themes[theme]);
      theme++;
     }
   }
   return 0;
}

int GetChannelNames2(int list_channels)
{
   int fd,n;
   
#ifdef TEST
   if((fd = open("561.bin",O_RDONLY)) < 0)
#else
   if((fd = open(device,O_RDWR)) < 0)
#endif
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd, 561, 200 );

   n = read(fd,buffer,3);
   if(buffer[0] != 0xc8)
     fprintf(stderr,"type 0x%x inattendu\n",buffer[0]);
   unsigned short table_len=buffer[2]|((buffer[1]&0x0f)<<8);
   n = read(fd,buffer+3,table_len);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading channels names\n");
      perror(":");
      return -1;
   }

   if(buffer[3]==0) n=GetChannels(list_channels);
   if(buffer[3]==1) n=GetThemes2(list_channels);

   n = read(fd,buffer,3);
   if(buffer[0] != 0xc8)
     fprintf(stderr,"type  0x%x inattendu\n",buffer[0]);
   table_len=buffer[2]|((buffer[1]&0x0f)<<8);
   n = read(fd,buffer+3,table_len);
   if (n <= 0)
   {
      fprintf (stderr, "Error while reading channels names\n");
      perror(":");
      return -1;
   }

   if(buffer[3]==0) n=GetChannels(list_channels);
   if(buffer[3]==1) n=GetThemes2(list_channels);

   ioctl(fd, DMX_STOP);
   close (fd);

   return 0;
}


/*
 *	Convert provider local time to UTC
 */
extern time_t ProviderLocalTime2UTC (time_t t);

/*
 *	Convert UTC to provider local time
 */
extern time_t UTC2ProviderLocalTime (time_t t);

/*
 *	Convert local time to UTC
 */
extern time_t LocalTime2UTC (time_t t);

/*
 *	Convert UTC to local time
 */
extern time_t UTC2LocalTime (time_t t);

/*
 *	Parse TOT
 */

extern int ParseTOT(void);

extern unsigned long getQuad(void *ptr);

extern unsigned short getDoub(void *ptr);

/*
 *	Get Program Titles (PID=564 , TID=230 )
 */
int GetTitles2(void)
{
   int fd,n,i,pos,lgr;
   struct program * program;
   time_t broadcast_time; 

   nb_programs=0;
   for (i=0; i<24*7; i++)
      hour_index[i] = -1;

#ifdef TEST
   if((fd = open("564.bin",O_RDONLY)) < 0)
#else
   if((fd = open(device,O_RDWR)) < 0)
#endif
   {
      perror("DEMUX DEVICE ");
      return -1;
   }
   SetFiltSection(fd,564,230 );

 unsigned short table_len;
 char tmpStr[65];
 unsigned long Quad0=0,Quad;
  while (1)
  {
    n = read(fd,buffer,3);
    if(buffer[0] != 0xe6)
      fprintf(stderr,"type 0x%x inattendu\n",buffer[0]);
    table_len=buffer[2]|((buffer[1]&0x0f)<<8);
    n = read(fd,buffer+3,table_len);
    Quad=getQuad(buffer+3);
    if(Quad0==0) Quad0=Quad;
    else if(Quad==Quad0) return 0;
    if(n<0) return 1;
    pos=18;
    while (pos<table_len)
    {
      program = malloc (sizeof (struct program));
      programs[nb_programs++] = program;


//    printf("chaine= %d:%s\n",buffer[pos], channels[buffer[pos]]);
      program -> channel_id = buffer[pos];
//    printf("pos1= 0x%x:%d\n",getDoub(buffer+pos+1),getDoub(buffer+pos+1));
//    printf("date=0x%x,%d\n",getDoub(buffer+pos+3),getDoub(buffer+pos+3));
//    printf("heure=%x\n",getDoub(buffer+pos+5));
      broadcast_time = (getDoub(buffer+pos+3)-40587) * 86400
			 + (((buffer[pos+5]&0xf0)>>4)*10+(buffer[pos+5]&0x0f)) * 3600
			 + (((buffer[pos+6]&0xf0)>>4)*10+(buffer[pos+6]&0x0f)) * 60;
//      broadcast_time = ProviderLocalTime2UTC (broadcast_time);
//      broadcast_time = LocalTime2UTC (broadcast_time);
      program -> time = broadcast_time;
//    printf("heure=%x\n",broadcast_time);
//    printf("duree=%d\n",getDoub(buffer+pos+8)>>4);
      program -> duration = (getDoub(buffer+pos+8)>>4) * 60;
      lgr=buffer[pos+10]&0x3f;
//    printf("lgr titre=%d\n",lgr);
      memcpy(tmpStr,buffer+pos+11,lgr);
      tmpStr[lgr]=0;
//    printf("titre=%s\n",tmpStr);
      strcpy (program -> title, tmpStr);
      pos+= lgr+11;
//    printf("st=%x\n",buffer[pos]&0x3f);
      program -> theme_id = (buffer[8]&0x3f)+(buffer[pos]&0x3f);
      program -> id = getDoub(buffer+pos+1);
      pos+= 4;
    }

  }

   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}
/*
 * fait le dump hexa d'une zone de memoire
 */
void dumphex(unsigned char*buf,int lgr,FILE *ficout)
{
 unsigned long pos2=0;
 int i;
  while(pos2 <lgr)
  {
    fprintf(ficout,"%4.4lx  ",pos2);
    for (i=0;i<16 && (i< lgr-pos2) ; i++)
      fprintf(ficout,"%2.2x ",buf[pos2+i]);
    for ( ;i<16 ; i++)
      fprintf(ficout,"   ");
    for (i=0;i<16 && (i< lgr-pos2) ; i++)
      if(isprint(buf[pos2+i]))
        fprintf(ficout,"%c",buf[pos2+i]);
      else
        fprintf(ficout,".");
    fprintf(ficout,"\n");
    pos2+=16;
  }
}

int dumpPid(int pid,int tid)
{
 int fd;
 int n;
   if((fd = open(device,O_RDWR)) < 0)
   {
    perror("DEMUX DEVICE ");
    return -1;
   }
   SetFiltSection(fd,pid,tid );
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   printf(" pid %d,tid %d, lus %d octets:\n",pid,tid,n);
   dumphex(buffer,n,stdout);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   printf(" pid %d,tid %d, lus %d octets:\n",pid,tid,n);
   dumphex(buffer,n,stdout);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   printf(" pid %d,tid %d, lus %d octets:\n",pid,tid,n);
   dumphex(buffer,n,stdout);
   n = read(fd,buffer,MAX_SECTION_BUFFER);
   printf(" pid %d,tid %d, lus %d octets:\n",pid,tid,n);
   dumphex(buffer,n,stdout);
   return 0;
}
/*
 *	Get Program Summaries (PID=566 , TID=150 )
 */
int GetSummaries2(void)
{
   int fd,n;
   struct summary * summary;
   
   fprintf (stderr,"Reading summaries ...\n");
   nb_summaries=0;
#ifdef TEST
   if((fd = open("566.bin",O_RDONLY)) < 0)
#else
   if((fd = open(device,O_RDWR)) < 0)
#endif
   {
    perror("DEMUX DEVICE ");
    return -1;
   }
#ifndef TEST
   SetFiltSection(fd,566, 150);
#endif

 unsigned short table_len;
 unsigned long Quad0=0,Quad;
  while (1)
  {
   int len;
   int nb;
    n = read(fd,buffer,3);
    if(buffer[0] != 150 )
      fprintf(stderr,"type 0x%x inattendu\n",buffer[0]);
    table_len=buffer[2]|((buffer[1]&0x0f)<<8);
    n = read(fd,buffer+3,table_len);
    Quad=getQuad(buffer+3);
    if(Quad0==0) Quad0=Quad;
    else if(Quad==Quad0) break;
    if(n<0) return 1;

      summary = malloc (sizeof (struct summary));
      summaries[nb_summaries++] = summary;

      summary -> id = getDoub(buffer+3);
      nb = buffer[12];
      int pos=13+nb;
      nb=buffer[pos] & 0x0f;
      pos += 1;
      len=0;
      int len_line;
      for ( ; nb >0 ; nb--)
      {
	len_line=buffer[pos+len];
	buffer[pos+len]='|';
	len+= len_line+1;
      }
      if(len>0)
        buffer[pos+len]=0;
      else
        buffer[pos+1]=0;
      summary -> text = malloc (len+1);
      if (summary -> text == NULL)
      {
         fprintf (stderr,"Memory allocation error %d\n",len+1);
         exit(1);
      }
      memcpy(summary -> text ,buffer+pos+1,len+1);
   }
   ioctl(fd, DMX_STOP);
   close (fd);
   return 0;
}

int SaveEpgData2(char * filename)
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
               //TODO if (program->time + program->duration > current_time)
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
                  fprintf (stderr,"0x%4x ", program -> id);
                  fprintf (stderr,"Summary%s available\n", program -> summary_available ? "" : " NOT");

                  fprintf (stderr,"%s\n",program -> title);
                  fprintf (stderr,"%s ", themes[program -> theme_id]);
#endif
                  summary_found = 0;
                  //if (program -> summary_available)
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
                       fprintf (file, "X %d\n",  (int)(program->time));
                       fprintf (file, "S %s - %d'\n", themes[program -> theme_id], program -> duration / 60);
                       fprintf (file, "L %d\n", program->duration/60);
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

/* Read channels equivalents */
extern int ReadEquiv (char *filename);

extern char filename[];
extern int GetTimeOffset(void);

void do_mhwepg2(int list_channels)
{
   if (GetTimeOffset())
      exit(1);
   if (GetChannelNames2(list_channels))
      exit(1);
   if (GetTitles2())
      exit(1);
    
   if (GetSummaries2())
      exit(1);

   fprintf (stderr,"Nb of summaries : %d\n",nb_summaries);
   fprintf (stderr,"Nb of titles : %d\n",nb_programs);

   if (SaveEpgData2(filename))
      exit(0);
 
   return 0;
}
