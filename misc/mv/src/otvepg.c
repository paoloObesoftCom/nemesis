/*
 *  otvepg.c	EPG for TPS bouquet
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

/*
 *
I structure du flux DVB :

	24 octets = entete
	  1 octet =  table_id : toujours 0x87
	  2 octets : 4 bits = flags
	  	12 bits = longueur de la section apres ces 3 premiers octets
	  2 octets = N° module
	  1 octet = flags
	  1 octet = n° section (inutilisé?)
	  1 octet = n° dernière section (inutilisé?)
	  7 octets = ???
	  octet 15 = n° sous-module
	  octets 16-19 = offset
	  octets 20-23 = longueur totale du sous-module

	suivent les données puis 4 octets de CRC.

II organisation des modules

	le module 0 = module "directory"
	en 0x1a : 2 octets = nb de modules (en plus du 0)
	en 0x1c : description des modules : 16 octets par module
		16 octets = description du module 1
			2 octets = pointeur sur le nom du module
			2 octets = numéro du module
			4 octets = longueur compressée
			4 octets = longueur après décompression
			4 octets = type de module ?


	composition d'un sous-module :
	si ne commence ni par INFO ni par COMP : données brutes.
	si ne commence pas par COMP : données organisées comme suit :
	INFO + 4 octets = longueur de INFO
	CODE + 4 octets = longueur de CODE
	DATA + 4 octets = ?
	RSRC + 4 octets + 4 octets = longueur de RSRC
	SWAP + 4 octets = longueur de SWAP
	LAST + 4 octets = longueur de LAST ?
	
	si commence par COMP :
	4 octets = longueur non compressée
	4 octets = taille apres decompression
	4 octets = index des données non compressees
	suivent les données compressees

III-1 Le module DIRECTORY (= numero 0)

 Contient la description des modules.
 format du bloc RSRC :
  en 0x1a : 2 octets = nb de modules (en plus du 0)
  en 0x1c : description des modules : 16 octets par module
   un module :
    2 octets = pointeur sur le nom du module
    2 octets = numéro du module
    4 octets = longueur compressée
    4 octets = longueur après décompression
    4 octets = type de module ?

 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

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

#include "otvepg.h"
#include "si_tables.h"

struct chaineNoos {
 doub NumChaine;
 doub x1;
 quad ptrNom;
 u_char x2[12];
};

// structure decrivant l'entete du flux DVB :
typedef struct {
   // octet 0 : table
   u_char table_id                               :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char section_syntax_indicator               :1;
   u_char dummy                                  :1;        // has to be 0
   u_char dummy2                                 :2;
   u_char section_length_hi                      :4;
#else
   u_char section_length_hi                      :4;
   u_char dummy2                                 :2;
   u_char dummy                                  :1;        // has to be 0
   u_char section_syntax_indicator               :1;
#endif
   u_char section_length_lo                      :8;
   //octets 3-4 : 
   u_char mod_id_hi                 :8;
   u_char mod_id_lo                 :8;
#if BYTE_ORDER == BIG_ENDIAN
   u_char                                        :2;
   u_char version_number                         :5;
   u_char current_next_indicator                 :1;
#else
   u_char current_next_indicator                 :1;
   u_char version_number                         :5;
   u_char                                        :2;
#endif
   //octets 6-7 : sections
   u_char section_number                         :8;
   u_char last_section_number                    :8;
   u_char x1                    :8;
   u_char x2                    :8;
   u_char x3                    :8;
   u_char x4                    :8;
   u_char x5                    :8;
   u_char x6                    :8;
   u_char x7                    :8;
   u_char x8                    :8; // sous-module
   u_char offset_hi_hi          :8;
   u_char offset_hi_lo          :8;
   u_char offset_lo_hi          :8;
   u_char offset_lo_lo          :8;
   u_char len_hi_hi          :8;
   u_char len_hi_lo          :8;
   u_char len_lo_hi          :8;
   u_char len_lo_lo          :8;
} epg_t;

// entete d'un module compresse :
struct ent_comp {
  char sig[4];           // 'COMP'
  unsigned char len0[4]; // longueur donnees compressees
  unsigned char len1[4]; // longueur donnees decompressees
  unsigned char len2[4]; // index donnees non compressees
};


unsigned char *PModule;   // buffer pour recevoir les donnees du module
unsigned long Pos=0;   // pointeur d'ecriture dans PModule

unsigned char *PDecode;    // buffer pour recevoir les donnees decompresses
unsigned long Decode_len; // taille allouee au buffer PDecode
unsigned long LengthOut;  // longueur des donnees decmpressees
char foundDir=0;   // flag pour savoir si on a chargé le module directory
int NbLoadeds=0;

typedef struct SMOD{
	int pos;
	int len;
	u_char * data;
} smod;

typedef struct MOD {
	smod *smods;
	u_char nb_smod;
	u_char loadeds;
} mod;

mod *PModules=NULL;
int NbMaxModules=0;
time_t heure0=0;

extern char RepNL;// caractere a mettre a la place des \n  

void Decode(u_char *PModule)
{
 struct ent_comp *pEntete;
 unsigned long lengthin;
 unsigned long lengthNC;
 unsigned long posIn;
 unsigned long posIn0;
 int  i, j, k, c0,c2=0;
 unsigned int  flags1,flags2=0;
 unsigned long pos=0;
 char a_cheval=0;
    
    pEntete=(struct ent_comp *)(PModule);
    lengthin=(pEntete->len0[0]<<24)|(pEntete->len0[1]<<16)|(pEntete->len0[2]<<8)|pEntete->len0[3];
    LengthOut=(pEntete->len1[0]<<24)|(pEntete->len1[1]<<16)|(pEntete->len1[2]<<8)|pEntete->len1[3];
    if(LengthOut > Decode_len)
    {
      Decode_len = LengthOut+100;
      PDecode = realloc(PDecode,Decode_len);
    }
    memset(PDecode,0,LengthOut);
    memcpy(PDecode+LengthOut-lengthin+0x11, PModule+17,lengthin);
    posIn0 = posIn = LengthOut-lengthin+0x11;
    lengthNC=(pEntete->len2[0]<<24)|(pEntete->len2[1]<<16)|(pEntete->len2[2]<<8)|pEntete->len2[3];

  flags1 = 0;
  for ( ; ; )
  {
    if (   (pos >LengthOut)
	|| (posIn >= lengthNC)
	|| (pos >= lengthNC)
	)
	    break;
    if (((flags1 >>= 1) & 256) == 0)
    {
      c0 = PDecode[posIn++];
      if(a_cheval)
      {
        flags2 = (c0 &0xf0) | c2;
        c2= (c0 &0x0f);
      }
      else
        flags2 = c0;
      flags1 =  0xff00;     
    }  
    if ((flags2<<= 1) & 256)
    {
      c0 = PDecode[posIn++];
      if(a_cheval)
      {
        PDecode[pos++]=(c0 & 0xf0) | c2;
        c2= (c0 &0x0f);
      }
      else
        PDecode[pos++]=c0;
    }
    else
    {
      c0 = PDecode[posIn++];
      if(a_cheval)
      {
        i=(c0 & 0xf0) | c2;
        c2= (c0 & 0x0f) ;
      }
      else
	i=c0;
      switch (i &0xf0)
      {
        case 0x80:// un seul caractère, copie de deux
        case 0x90:// un seul caractère, copie de deux
          i = i&0x01f;//offset
	  j = 2;
	  break;;
        case 0xa0:// 3 quartets, copie de deux octets
        case 0xb0:// 3 quartets, copie de deux octets
	  // 101ooooo oooo
	  i = i&0x01f;//offset, partie basse
	  j = 2;
	  if(a_cheval)
	  {
	    i |= (c2 <<5) ;
	    a_cheval=0;
	  }
	  else 
	  {
            c0 = PDecode[posIn++];
	    i |= ((c0 & 0xf0)<<1) ;
	    c2= (c0 &0x0f) ;
	    a_cheval=1;
	  }
	  break;;
	case 0xc0:// un seul caractère, copie de trois
	  i = (i&0x0f);//offset
	  j = 3;
	  break;;
	case 0xd0:// 3 quartets, copie de trois octets
	  // 1101oooo oooo
	  i = i&0x0f;//offset, partie basse
	  j = 3;
	  if(a_cheval)
	  {
	    i |= (c2<<4) ;
	    a_cheval=0;
	  }
	  else 
	  {
            c0 = PDecode[posIn++];
            i |= ((c0 & 0xf0)) ;
            c2= (c0 &0x0f);
	    a_cheval=1;
	  }
	  break;;
	case 0xe0:// 3 quartets, 4 octets à copier
	case 0xf0:// 3 quartets, 5 octets à copier
	  // 1111oooo oooo
	  if( i &0x10) j = 5;
	  else j = 4;
	  i = i&0x0f;//offset, partie basse
	  if(a_cheval)
	  {
	    i |= (c2<<4) ;
	    a_cheval=0;
	  }
	  else 
	  {
            c0 = PDecode[posIn++];
            i |= ((c0 & 0xf0)) ;
	    c2= (c0 &0x0f) ;
	    a_cheval=1;
	  }
	  break;;
	case 0x00: // deux octets, 0llloooo oooooooo
	case 0x10:
	case 0x20:
	case 0x30:
	case 0x40:
	case 0x50:
	case 0x60:
	  // 0llloooo oooooooo
	  j = ((i & 0x70) >>4) +3; //longueur à copier
	  i = (i & 0x0f)<<8;
          c0 = PDecode[posIn++];
	  if(a_cheval)
	  {
	    i |=  ((c0 &0xf0) | c2);
	    c2= (c0 &0xf) ;
	  }
	  else
	    i |= c0;
	  break;;
        //trois, quatre, 6 octets
	//    0111llll looooooo oooooooo
	// ou 01111111 1ooooooo oooooooo llllllll
	// ou 01111111 1ooooooo oooooooo 11111111 llllllll llllllll
	case 0x70:
	  j = ((i & 0x0f) <<1) +3; //longueur à copier
          c0 = PDecode[posIn++];
	  if(a_cheval)
	  {
	    i= ((c0 &0xf0) | c2);
	    c2= (c0 &0xf) ;
	  }
	  else
	    i = c0;
	  if(i & 0x80)
	  {
	    j++;
	    i &= 0x7f;
	  }
	  i = i<<8;
          c0 = PDecode[posIn++];
	  if(a_cheval)
	  {
	    i |=  ((c0 & 0xf0) | c2);
	    c2= (c0 & 0xf) ;
	  }
	  else
	    i = i | c0;
	  if( j == (0x1f+3) ) // cas autres que  0111llll looooooo oooooooo
	  {
	    //
            c0 = PDecode[posIn++];
	    if(a_cheval)
	    {
	      j = ((c0 & 0xf0) | c2);
	      c2= (c0 & 0xf) ;
	    }
	    else
	      j = c0;
	    if(j == 0xff)
	    { // cas 01111111 1ooooooo oooooooo 11111111 llllllll llllllll
              c0 = PDecode[posIn++];
	      if(a_cheval)
	      {
	        j= ((c0 & 0xf0) | c2)<<8;
	        c2= (c0 & 0xf) ;
	      }
	      else
	        j = c0<<8;
              c0 = PDecode[posIn++];
	      if(a_cheval)
	      {
	        j |= ((c0 & 0xf0) | c2);
	        c2= (c0 &0xf) ;
	      }
	      else
	        j |= c0;
	      j+= 289;
	    }
	    else j += 34;
	  }
	  break;;
	default:
          j = PDecode[posIn++];
	  i = 0;
	  printf("??? doublet x%2.2x:%2.2x en 0x%lx --> 0x%lx\n",i,j,posIn-posIn0+16,pos);
	  break;;
      }
      if(i>pos)
      {
	printf("??? offset trop eleve\n");
	i = -1;
      }
      for (k = 0; k < j; k++) 
        PDecode[pos+k] = PDecode[pos-1-i+k];
      pos += j;
    }
  }
}

int NbModules=0;
unsigned char *pMod0;
unsigned char *pModData;
unsigned char *pModInfoevt;
int numData=5;
int numInfoevt=32;
int NbInfoevt[300];
int MaxInfoevt=0;
time_t heure0;// 6h du matin pour le premier evenement
time_t heure_debut;// 6h du matin pour le premier evenement

void * getRsrc(void *ptr)
{
  //FIXME : utiliser une meilleure methode de calculer le debut de RSRC
  if(!memcmp(ptr+32,"RSRC",4))
    return ptr+32;
  else return ptr;
}

unsigned long getQuad(void *ptr)
{
 u_char * pData=ptr;
      return (pData[0]<<24) | (pData[1]<<16) | (pData[2]<<8) | (pData[3]);
}

unsigned short getDoub(void *ptr)
{
 u_char * pData=ptr;
      return (pData[0]<<8) | pData[1];
}

char * getChaine(int numChaine)
{
 u_char *pRsrc=getRsrc(pModData);
 int iPtr=getQuad(pRsrc+8);
  if((iPtr+32)>PModules[numData].smods[0].len) return NULL;
 int iChaines=getQuad(pRsrc+iPtr+0x20);
 int iNbChaines=getDoub(pRsrc+iPtr+0x24);
 if(numChaine < iNbChaines && numChaine>=0)
 {
   return ((char *)pRsrc+getQuad(pRsrc+iChaines+20*numChaine));
 }
 else
   return NULL;
}

int NbChaine=140;

short memEvt[4096];
char filename [100];

/*
 *	Set a filter for a DVB stream
 */

extern void SetFiltSection (int fd, unsigned short pid, unsigned char tid);

FILE *ficepg;
 char device [30];

int current_offset, next_offset;
time_t time_of_change;
extern int timezonecmp(item_local_time_offset_t *item_local_time_offset,
   			 const char * country, int region);

extern int ParseTOT(void);

int affEmi(int jour0,int heure,int mod,int id)
  {
   u_long iPtr,iPtr1,iPtr2,iPtr3,iPStr;
   long jour;
   if(mod+numInfoevt >= NbModules)
   {
#ifdef TEST
     fprintf(ficepg,"module %d trop grand\n",mod+numInfoevt);
#endif
     return 0;
   }
   u_char *pData=PModules[mod+numInfoevt].smods[0].data;
   u_char *pRsrc=getRsrc(pData);


    iPtr1=getQuad(pRsrc+8);
    iPtr=getQuad(pRsrc+iPtr1);
    iPStr=getQuad(pRsrc+iPtr+8);
    for( ;iPtr < iPtr1; iPtr+=36)
    {
      if(id==getDoub(pRsrc+iPtr)) break;
    }
    if(id!=getDoub(pRsrc+iPtr)) return 0;
      iPtr2=getQuad(pRsrc+iPtr+28);
      fprintf(ficepg,"E\n");
      if(heure <6*60) jour=jour0+1;
      else jour=jour0;
      heure_debut=heure0+jour*24*60*60-6*3600+heure*60;//heure de debut
	if( heure0 <time_of_change && heure_debut >time_of_change)
	{
		heure_debut -= next_offset-current_offset;
	}
      //else
      //  heure_debut=heure0+(jour+1)*24*60*60+heure*60;//heure de debut
      fprintf(ficepg,"X %ld\n",(long)heure_debut);
#ifdef TEST
      fprintf(ficepg,"x. jour %ld %ldh%2.2ld\n",jour,heure/60,heure-(heure/60)*60);
#endif
      //rediffusions :
      
      //titre
      iPtr2=getDoub(pRsrc+iPtr+2);
      iPtr2=getQuad(pRsrc+iPtr2*4+16);
      fprintf(ficepg,"T %s\n",pRsrc+iPtr2);
      //acteurs
      iPtr2=getDoub(pRsrc+iPtr+10);
      int DescEnCours=0;
      if(iPtr2 != 0xffff)
      {
        iPtr2=getQuad(pRsrc+iPtr2*4+16);
        fprintf(ficepg,"D %s",pRsrc+iPtr2);
        DescEnCours=1;
      }
      // de ...
      iPtr2=getDoub(pRsrc+iPtr+4);
      if(iPtr2 != 0xffff)
      {
        iPtr2=getQuad(pRsrc+iPtr2*4+16);
        if(DescEnCours)
          fprintf(ficepg,"| %s",pRsrc+iPtr2);
	else
          fprintf(ficepg,"D %s",pRsrc+iPtr2);
        DescEnCours=1;
      }
      // année
      iPtr2=getDoub(pRsrc+iPtr+8);
      if(iPtr2 != 0xffff)
      {
        iPtr2=getQuad(pRsrc+iPtr2*4+16);
        if(DescEnCours)
          fprintf(ficepg,"| %s",pRsrc+iPtr2);
	else
          fprintf(ficepg,"D %s",pRsrc+iPtr2);
        DescEnCours=1;
      }
      //description
      iPtr2=getDoub(pRsrc+iPtr+12);
      if(iPtr2 == 0xffff)
        iPtr2=getDoub(pRsrc+iPtr+14);
      if(iPtr2 != 0xffff)
      {
      iPtr2=getQuad(pRsrc+iPtr2*4+16);
      char *desc=(char *)pRsrc+iPtr2;
      char *ptr;
	 while ( (ptr=strchr(desc,'\n')) ) *ptr='|';;
        if(DescEnCours)
          fprintf(ficepg,"|%s",pRsrc+iPtr2);
	else
          fprintf(ficepg,"D %s",pRsrc+iPtr2);
      DescEnCours=1;
      }
      //rediffusions :
      iPtr2=getQuad(pRsrc+iPtr+28);
      if(iPtr2 != 0xffff)
      {
       int j;
        for(j=1; j <pRsrc[iPtr+32] ; j++)
        {
	  if(j==1)
	  {
            if(DescEnCours)
              fprintf(ficepg,"|Rediff. : ");
            else
              fprintf(ficepg,"D Rediff. : ");
	  }
	  else
              fprintf(ficepg,", ");
	  int chaine = pRsrc[iPtr2+1+j*8];
          jour= pRsrc[iPtr2+2+j*8];
          heure=getDoub(pRsrc+iPtr2+4+j*8);
	 char *pChaine=getChaine(chaine);
	  if(pChaine)
            fprintf(ficepg,"%s" ,pChaine);
        heure_debut=heure0+jour*24*60*60-6*3600+heure*60;//heure de debut
	if( heure0 <time_of_change && heure_debut >time_of_change)
	{
		printf("correction heure\n");
		heure_debut -= next_offset-current_offset;
	}
   struct tm *debut_time;
               debut_time = gmtime(&heure_debut);
          fprintf(ficepg," %02d/%02d a %dh%2.2d",debut_time->tm_mday,debut_time->tm_mon+1
			  ,heure/60,heure-(heure/60)*60);
        }
      }
      if(DescEnCours)
        fprintf(ficepg,"\n");
#ifdef TEST
      fprintf(ficepg,"x0 id programme=0x%2.2x mod %d dif 0x%lx\n",getDoub(pRsrc+iPtr),mod,getQuad(pRsrc+iPtr+28));
      //???
      // pays ?
      iPtr2=getDoub(pRsrc+iPtr+6);
      if(iPtr2 != 0xffff)
        fprintf(ficepg,"x6: pays %4.4lx \n",iPtr2);
      //???
      iPtr2=getDoub(pRsrc+iPtr+16);
      fprintf(ficepg,"x16: %2.2x \n",pRsrc[iPtr+16]);
      fprintf(ficepg,"x17: %2.2x \n",pRsrc[iPtr+17]);
      fprintf(ficepg,"x21: %2.2x \n",pRsrc[iPtr+21]);
      //???
      fprintf(ficepg,"x22: %2.2x \n",pRsrc[iPtr+22]);
      fprintf(ficepg,"x23: %2.2x \n",pRsrc[iPtr+23]);
      //???
      iPtr2=getDoub(pRsrc+iPtr+24);
      if(iPtr2 != 0xffff)
        fprintf(ficepg,"x24: %4.4lx \n",iPtr2);
      fprintf(ficepg,"x31: %2.2x \n",pRsrc[iPtr+31]);
      fprintf(ficepg,"x32: %2.2x \n",pRsrc[iPtr+32]);
      fprintf(ficepg,"x33: %2.2x \n",pRsrc[iPtr+33]);
      fprintf(ficepg,"x34: %2.2x \n",pRsrc[iPtr+34]);
      fprintf(ficepg,"x35: %2.2x \n",pRsrc[iPtr+35]);
#endif
      // duree
      iPtr2=getDoub(pRsrc+iPtr+18);
      if(iPtr2 != 0xffff)
      {
          fprintf(ficepg,"L %ld\n",iPtr2);
      }
      else
        // trouver la duree de l'emission
        fprintf(ficepg,"L 5\n");
/* TODO broken in v3
      // sous-categorie de programme
      iPtr2=getDoub(pRsrc+iPtr+20);
      if(iPtr2 != 0xffff)
      {
       u_char *pRsrcData=getRsrc(pModData);
       iPtr3=getQuad(pRsrcData+8);
       int iPtr4=getQuad(pRsrcData+iPtr3+0x60);
       int iPtr5=getQuad(pRsrcData+iPtr4+4*pRsrc[iPtr+20]);
        //fprintf(ficepg,"x20: %04.4lx %04.4lx %x %s\n",iPtr2,iPtr3,iPtr4,pRsrcData+iPtr5);
        fprintf(ficepg,"S %s\n",pRsrcData+iPtr5);
      }
*/
      fprintf(ficepg,"e\n");fflush(ficepg);
      return 1;
  }

void do_otvepg(int list_channels)
{
 unsigned int adapter = 0, demux = 0;
 char buf[4096];
 epg_t *table;
 int table_len;
 unsigned int sous_table;
 u_char        s_sous_table;
 unsigned long offset=0;
 unsigned long len=0;

#ifdef TEST
  strcpy(device,"4150.bin");
#else
  sprintf (device, DEMUX_DEVICE_MASK, adapter, demux);
#endif
  printf("  sortie vers : %s\n",filename);
  int fd;
  if((fd = open(device,O_RDWR)) <0)
  {
    perror("DEMUX DEVICE ");
    return ;
  }
#ifndef TEST
  SetFiltSection ( fd, 4150, 135);
#endif

  table= (epg_t *)&buf[0];
  PDecode = malloc(40000);
  Decode_len=40000;
  NbMaxModules=50;
  PModules = (mod *)malloc(NbMaxModules*sizeof(mod));
  memset(PModules,0,NbMaxModules*sizeof(mod));
  long X=0;
  while( read(fd,buf,3) )
  {
    if(table->table_id != 0x87) printf("type 0x%x inconnu\n",table->table_id);
    table_len=table->section_length_lo|(table->section_length_hi<<8);
    if(!read(fd,buf+3,table_len)) break;
    sous_table=table->mod_id_lo|(table->mod_id_hi<<8);
    s_sous_table=table->x8;
    offset =table->offset_lo_lo|(table->offset_lo_hi<<8)|(table->offset_hi_lo<<16)|(table->offset_hi_hi<<24);
    len =table->len_lo_lo|(table->len_lo_hi<<8)|(table->len_hi_lo<<16)|(table->len_hi_hi<<24);
    //printf("table type 0x%x,mod=%d,off=%ld\n",table->table_id,sous_table,offset);
    if(sous_table == 0xffff)
	    continue;
    if(!X)
    {
      X=getQuad(((char *)(table))+8);
      printf("version %8.8lx\n",X);
    }
    else if( X != getQuad(((char *)(table))+8))
    {
      // TODO : dans ce cas il faut recommencer depuis 0
      printf(" !!! pb de changement de version ??? %8.8lx %8.8lx\n"
         ,X,   getQuad(((char *)(table))+8)
			      );
      memset(PModules,0,NbMaxModules*sizeof(mod));
      X=0;
    }
    if(sous_table >=NbMaxModules)
    {
      PModules = (mod *)realloc(PModules,(sous_table+11)*sizeof(mod));
      memset(&PModules[NbMaxModules],0,(sous_table+11-NbMaxModules)*sizeof(mod));
      NbMaxModules = sous_table+11;
    }
    if(s_sous_table >= PModules[sous_table].nb_smod)
    {
      PModules[sous_table].smods = realloc(PModules[sous_table].smods,(s_sous_table+1)*sizeof(smod));
      memset(&PModules[sous_table].smods[PModules[sous_table].nb_smod],0,(s_sous_table+1-PModules[sous_table].nb_smod)*sizeof(smod));
      PModules[sous_table].nb_smod = s_sous_table+1;
    }
    if (offset == PModules[sous_table].smods[s_sous_table].pos)
    {
      if(!PModules[sous_table].smods[s_sous_table].data && offset==0)
      {
	PModules[sous_table].smods[s_sous_table].len =len;
	if(!memcmp(buf+24,"COMP",4))
	{
	  int LenDec=getQuad(buf+32);
	  PModules[sous_table].smods[s_sous_table].data = malloc(LenDec);
	}
	else
	  PModules[sous_table].smods[s_sous_table].data = malloc(len);
      }
      memcpy(PModules[sous_table].smods[s_sous_table].data+offset
		      ,buf+24,table_len-25);
      PModules[sous_table].smods[s_sous_table].pos += table_len-25;
      if(PModules[sous_table].smods[s_sous_table].pos == len)
      {
  putchar('.');fflush(stdout);
	if(sous_table==0) foundDir=1;
	if(!memcmp(PModules[sous_table].smods[s_sous_table].data,"COMP",4))
	{
	  int LenDec=getQuad(PModules[sous_table].smods[s_sous_table].data+8);
	  Decode(PModules[sous_table].smods[s_sous_table].data);
	  memcpy(PModules[sous_table].smods[s_sous_table].data,PDecode,LenDec);
	  PModules[sous_table].smods[s_sous_table].len=LenDec;
	}
	PModules[sous_table].loadeds++;
	//FIXME : trouver un meilleur test de fin
	if(foundDir)
	{
	  NbLoadeds=0;
	  int i;
	  for (i=0 ; i<= PModules[0].smods[0].data[27] ; i++)
	    if((PModules[i].loadeds>0))// && (PModules[i].loadeds  == PModules[i].nb_smod))
	      NbLoadeds++;
	  // on devrait charger PModules[0].smods[0].data[27]+1 modules
	  // mais TPS oublie d'envoyer le dernier module !!!!
	  if(NbLoadeds==PModules[0].smods[0].data[27])
	  {
	  printf ("%d modules chargés\n",NbLoadeds);
	    printf("FIN NORMALE!\n");
	    break;
	  }
	}
      }
    }
  }
  putchar('\n');fflush(stdout);
#ifndef TEST
  ioctl(fd, DMX_STOP);
#endif
  close(fd);
  {
    pMod0=PModules[0].smods[0].data;
    NbModules=pMod0[27];
    printf("dernier module = %d\n",NbModules);
    int i;
    for (i=1 ; i<= NbModules ; i++)
    {
      int pNom=getDoub(pMod0+12+i*16);
      printf("module %d : %s\n",i,pMod0+pNom);
      if(!strcmp((char *)pMod0+pNom,"DATA")) numData=i;
      if(!memcmp((char *)pMod0+pNom,"MOD",3))
      {
	numInfoevt=i;
	break;
      }
    }
  }
  pModData=PModules[numData].smods[0].data;
  u_char *pRsrc=getRsrc(pModData);
  int iPtr=getQuad(pRsrc+8);
  heure0 = getQuad(pRsrc+iPtr);
      if (ParseTOT() != 0)
      {
         printf ("No local time in the TOT table. Please use the -t option.\n");
      }
      else
      {
	      // etrange : la date donnee par TPS serait fausse de 3 jours.
	      time_of_change -= 86400*3;
	      printf(" time : current=%d,next=%d,time_of_change=%ld\n",current_offset,next_offset,time_of_change);
      }
  ficepg=fopen(filename,"w");
 int i;
 int chaine;
 u_char *pRsrcData=getRsrc(pModData);
 u_long iPtrData=getQuad(pRsrcData+8);
 int iChaines=getQuad(pRsrcData+iPtrData+0x20);
 int iNbChaines=getDoub(pRsrcData+iPtrData+0x24);
 u_long iPtrLst=getQuad(pRsrcData+iPtrData+0x44);
 u_long iInd=getQuad(pRsrcData+iPtrData+0x34);
 u_long iEmi=getQuad(pRsrcData+iPtrData+0x38);
 u_long iTit=getQuad(pRsrcData+iPtrData+0x58);
 int jour;
  for(chaine=0 ; chaine < iNbChaines ; chaine++)
  {
    fprintf(ficepg,"C %d %s\n",chaine,getChaine(chaine));
    for(jour=0 ; jour <7 ; jour++)
    {
     int nbrEmi = pRsrcData[iPtrLst+7*chaine*8+jour*8];
     u_long iPtrInd = getQuad(pRsrcData+iPtrLst+7*chaine*8+jour*8+4);
#ifdef TEST
      fprintf(ficepg,"jour %d chaine %d:nb=%d,ptrind=%x\n",jour,chaine,nbrEmi,iPtrInd);
#endif
      for(i=0 ; i <nbrEmi ; i++)
      {
       int ind=getDoub(pRsrcData+iPtrInd+i*2);
       int hdeb=getDoub(pRsrcData+iInd+ind*8+2);
       int id=getDoub(pRsrcData+iInd+ind*8+4);
       int xx=getDoub(pRsrcData+iEmi+id*4);
       int xy=pRsrcData[iEmi+id*4+2];
       int mod=pRsrcData[iEmi+id*4+3];
#ifdef TEST
        fprintf(ficepg," id %x xx=%x xy=%x mod=%d\n",id,xx,xy,mod);
        fprintf(ficepg,"x. jour %d %dh%2.2d\n",jour,hdeb/60,hdeb-(hdeb/60)*60);
#endif
        if (!affEmi(jour,hdeb,mod,id))
	{
          u_long   iPtr=getQuad(pRsrcData+iTit+4*xx);
	  fprintf(ficepg,"E\nT %s\n",pRsrcData+iPtr);
	  if(hdeb < 6*60)
            heure_debut=heure0+(jour+1)*24*60*60-6*3600+hdeb*60;
	  else
            heure_debut=heure0+jour*24*60*60-6*3600+hdeb*60;
	  if( heure0 <time_of_change && heure_debut >time_of_change)
 	  {
		heure_debut -= next_offset-current_offset;
	  }
          fprintf(ficepg,"X %ld\nL 5\ne\n",(long)heure_debut);
	}
      }
    }
    fprintf(ficepg,"c\n");
  }
  fclose(ficepg);

  return;
}
