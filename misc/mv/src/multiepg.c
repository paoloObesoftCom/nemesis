
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

char device [30];
char source [50];

#define MAX_SECTION_BUFFER 4096
unsigned char buffer[MAX_SECTION_BUFFER+1];	/* General data buffer */
unsigned char buffer3[3];	/* Temporary data buffer */

#define MAX_CHANNELS 180

char channels[MAX_CHANNELS][50];
char channel_ids[MAX_CHANNELS][50];
char themes[256][16];
char channel_equiv[MAX_CHANNELS][2][50];
int nb_equiv=0;


/* Local Time offsets */
int current_offset, next_offset;
time_t time_of_change;
int time_offset_set = 0;
int yesterday;	/* Day of the week of yesterday */
time_t yesterday_epoch;	/* Epoch of yesterday 00:00 in provider local time */

char RepNL=' ';/* character to replace nl */

int fd211,fd566,fd4150;

/* Read channels equivalents */
int ReadEquiv (char *filename)
{
   FILE *file;
   char buffer [128];
   char * ptr;

   file = fopen (filename,"r");
   if (file == NULL)
   {
	fprintf (stderr, "Can't open equivalent channels file : %s\n", filename);
	return -1;
   }
   while (fgets (buffer, sizeof(buffer), file) > 0 && nb_equiv < 100)
   {
	ptr = strtok (buffer, " \t\n\r");
	strncpy (channel_equiv[nb_equiv][0], ptr, 50);
	ptr = strtok (NULL, " \t\n\r");
	strncpy (channel_equiv[nb_equiv][1], ptr, 50);
	nb_equiv++;	
   }
   fclose (file);
   fprintf (stderr, "Nb equivalents : %d\n",nb_equiv);
   return 0;
}

/*
 *	Set a filter for a DVB stream
 */

void SetFiltSection (int fd, unsigned short pid, unsigned char tid)
{
#ifndef TEST
	struct dmx_sct_filter_params sctFilterParams;

	if (ioctl(fd, DMX_SET_BUFFER_SIZE, 188*1024)<0) {
          perror("DMX SET BUFFER SIZE");
          exit(0);
        } 

	memset(&sctFilterParams, 0, sizeof(sctFilterParams));
	sctFilterParams.pid     = pid;
	sctFilterParams.timeout = 60000;
	sctFilterParams.flags   = DMX_IMMEDIATE_START;

    sctFilterParams.filter.filter[0] = tid;
    sctFilterParams.filter.mask[0] = 0xFF;
        
	if (ioctl(fd, DMX_SET_FILTER, &sctFilterParams) < 0)  
    {
		perror("DMX SET FILTER:");
        exit (0);
    }
#endif
}

static char *usage_str =
    "\nusage: multiepg [options]\n"
    "         get EPG data\n"
    "     -a number : use given adapter (default 0)\n"
    "     -d number : use given demux (default 0)\n"
    "     -s string : source (such as S19.2E)\n"
    "     -e string : list of equivalent channels\n"
    "     -t number : local time offset (in seconds)\n"
    "     -l        : send the channels list to stderr\n"
    "     -o string : output to file <string> (default /hdd/epg.dat)\n"
    "     -p string : path to epg.dat\n\n";

static void usage(void)
{
   fprintf(stderr, usage_str);
   exit(1);
}

char filename [100];
extern void do_mhwepg(int);
extern void do_mhwepg2(int);
extern void do_otvepg(int);

int main(int argc, char *argv[])
{
   unsigned int adapter = 0, demux = 0;
   int opt;
   int list_channels = 0;

   source [0] = 0;
   strcpy(filename,"epg.dat");
   while ((opt = getopt(argc, argv, "?hls:o:a:d:e:t:p:n:")) != -1)
   {
      switch (opt)
      {
	 case '?':
	 case 'h':
	 default:
	    usage();
	 case 's':
	    strncpy(source, optarg, sizeof(source));
	    break;
	 case 'o':
	    strncpy(filename, optarg, sizeof(filename));
	    break;
	 case 'e':
	   if (ReadEquiv(optarg))
	       exit (1);
           break;
	 case 'a':
	    adapter = strtoul(optarg, NULL, 0);
	    break;
	 case 'd':
	    demux = strtoul(optarg, NULL, 0);
            break;
	 case 't':
	    current_offset = strtoul(optarg, NULL, 0);
            next_offset = current_offset;
            time_of_change = 0;
            time_offset_set = 1;
            break;
	 case 'l':
	    list_channels = 1;
	    break;
	 case 'n':
	    RepNL=optarg[0];
	    break;
	 case 'p':
	    strncpy(filename, optarg, sizeof(filename)-8);
	    strcat(filename,"/epg.dat");
	    break;
      }
   }
   sprintf (device, DEMUX_DEVICE_MASK, adapter, demux);

   if((fd211 = open(device,O_RDWR|O_NONBLOCK)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd211, 211, 144 );
   if((fd566 = open(device,O_RDWR|O_NONBLOCK)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd566, 566, 150 );
   if((fd4150 = open(device,O_RDWR|O_NONBLOCK)) < 0)
   {
      perror("DEMUX DEVICE : ");
      return -1;
   }
   SetFiltSection (fd4150, 4150, 135 );
   sleep(1);
 int n;
  n = read(fd211,buffer3,3);
  if(n==3)
  {
    printf("mhwepg found!\n");
    ioctl(fd211, DMX_STOP);
    close (fd211);
    ioctl(fd566, DMX_STOP);
    close (fd566);
    ioctl(fd4150, DMX_STOP);
    close (fd4150);
    do_mhwepg(list_channels);
  }
  else 
  {
    n = read(fd566,buffer3,3);
    if(n ==3)
    {
      ioctl(fd211, DMX_STOP);
      close (fd211);
      ioctl(fd566, DMX_STOP);
      close (fd566);
      ioctl(fd4150, DMX_STOP);
      close (fd4150);
      printf("mhwepg2 found!\n");
      do_mhwepg2(list_channels);
    }
    else
    {
      n = read(fd4150,buffer3,3);
      if(n==3)
      {
        printf("otvepg found!\n");
        ioctl(fd211, DMX_STOP);
        close (fd211);
        ioctl(fd566, DMX_STOP);
        close (fd566);
        ioctl(fd4150, DMX_STOP);
        close (fd4150);
        do_otvepg(list_channels);
      }
      else
        printf("ERROR : no epg found ???\n");
    }
  }

  return 0;
}
