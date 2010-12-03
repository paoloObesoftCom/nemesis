#include <channelinfo.h>
#include <lib/base/i18n.h>
#include <lib/system/init.h>
#include <lib/system/init_num.h>
#include <lib/gui/eskin.h>
#include <lib/gdi/font.h>
#include <lib/dvb/epgcache.h>
#include <lib/dvb/dvbservice.h>
#include <lib/dvb/frontend.h>
#include <lib/system/info.h>
#include <sys/stat.h>
#include <time.h>
#include <epgwindow.h>
#include <sselect.h>

eChannelInfo::eChannelInfo( eWidget* parent, const char *deco)
	:eDecoWidget(parent, 0, deco),
	ctime(this), cname(this), copos(this), cdescr(this),
	cdolby(this), cstereo(this), ptime(this),
	cformat(this), cscrambled(this), eit(0)
{
	init_eChannelInfo();
}
void eChannelInfo::init_eChannelInfo()
{
	foregroundColor=eSkin::getActive()->queryColor("eStatusBar.foreground");
	backgroundColor=eSkin::getActive()->queryColor("eStatusBar.background");
	gFont fn = eSkin::getActive()->queryFont("eChannelInfo");

	cdescr.setFont( fn );
	cdescr.setForegroundColor( foregroundColor );
	cdescr.setBackgroundColor( backgroundColor );
	cdescr.setFlags(RS_WRAP);
// 	cdescr.hide();

	copos.setFont( fn );
	copos.setAlign( eTextPara::dirRight );
	copos.setForegroundColor( foregroundColor );
	copos.setBackgroundColor( backgroundColor );
	copos.setFlags( RS_FADE );

	cname.setFont( fn );
	cname.setForegroundColor( foregroundColor );
	cname.setBackgroundColor( backgroundColor );
	cname.setFlags( RS_FADE );
	cname.setAlign(eTextPara::dirCenter);

	ctime.setFont( fn );
	ctime.setForegroundColor( foregroundColor );
	ctime.setBackgroundColor( backgroundColor );
	ctime.setFlags( RS_FADE );
	ctime.setAlign(eTextPara::dirCenter);

	gPixmap *pm=eSkin::getActive()->queryImage("sselect_dolby");
	cdolby.setPixmap(pm);
	cdolby.pixmap_position = ePoint(0,0);
	cdolby.hide();

	pm = eSkin::getActive()->queryImage("sselect_stereo");
	cstereo.setPixmap(pm);
	cstereo.pixmap_position = ePoint(0,0);
	cstereo.hide();

	pm = eSkin::getActive()->queryImage("sselect_format");
	cformat.setPixmap(pm);
	cformat.pixmap_position = ePoint(0,0);
	cformat.hide();

	pm = eSkin::getActive()->queryImage("sselect_crypt");
	cscrambled.setPixmap(pm);
	cscrambled.pixmap_position = ePoint(0,0);
	cscrambled.hide();
}

const char *eChannelInfo::genresTableShort[256] =
{
	/* 0x0 undefined */    	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL, 

	/* 0x1 Movie */        	_("Movie"),("Thriller"),_("Adventure"),_("SciFi"),_("Comedy"),
							_("Soap"),_("Romance"),_("Serious"),_("Adult"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0x2 News */         	_("News"),_("Weather"),_("Magazine"),_("Documentary"),_("Discussion"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,

	/* 0x3 Show */         	_("Show"),_("Game Show"),_("Variety"),_("Talk"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0x4 Sports */       	_("Sports"),_("Special Event"),_("Sports Mag."),_("Football"),_("Tennis"),_("Team Sports"),
							_("Athletics"),_("Motor Sports"),_("Water Sports"),_("Winter Sports"),_("Equestrian"),
							_("Martial Sports"),
							NULL,NULL,NULL,NULL,

	/* 0x5 Children */     	_("Children"),_("Pre-School"),_("Age 6-14"),_("Age 10-16"),_("School"),
							_("Cartoons"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,

	/* 0x6 Music */        	_("Music"),_("Rock/Pop"),_("Classical"),_("Folk"),_("Jazz"),_("Musical"),_("Ballet"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,

	/* 0x7 Culture */      	_("Culture"),_("Perf. Arts"),_("Fine Arts"),_("Religion"),_("Pop. Arts"),_("Literature"),
							_("Film"),_("Experimental"),_("Press"),_("New Media"),_("Art Mag."),_("Fashion"),
							NULL,NULL,NULL,NULL,

	/* 0x8 Social */       	_("Social"),_("Soc. Mag."),_("Economics"),_("Remark. People"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0x9 Education */    	_("Education"),_("Nature"),_("Technology"),_("Medicine"),_("Expeditions"),_("Spiritual"),
							_("Further Ed."),_("Languages"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xa Leisure */      	_("Hobbies"),_("Travel"),_("Handicraft"),_("Motoring"),_("Fitness"),_("Cooking"),
							_("Shopping"),_("Gardening"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xb Special */      	_("Orig. Lang."),_("B&W"),_("Unpublished"),_("Live"),
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,

	/* 0xc reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xd reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xe reserved */     	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,

	/* 0xf user defined */ 	NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
							NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};

void eChannelInfo::ParseEITInfo(EITEvent *e)
{
		name=descr=genre=starttime=timenow=stop_time="";
		cflags=0;
		eString t;
			
		if(e->start_time!=0)
		{
			time_t now = time(0) + eDVB::getInstance()->time_difference;
			tm *ti=localtime(&now);
			timenow.sprintf("%02d:%02d", ti->tm_hour, ti->tm_min);
			tm *start = localtime(&e->start_time);
			starttime.sprintf("%02d:%02d", start->tm_hour, start->tm_min);

			long int cur_start_time = e->start_time;
			long int cur_duration = e->duration;
			long int stoptime = cur_start_time + cur_duration;
 			tm *st = localtime(&stoptime);
			stop_time.sprintf(" - %02d:%02d",st->tm_hour, st->tm_min);

			ptime.setPerc((now - e->start_time)*100/e->duration);
			ptime.show();
		}

		if (e->free_CA_mode )
			cflags |= cflagScrambled;

		LocalEventData led;
		led.getLocalData(e, &name, 0 , &descr);
		DescriptionForEPGSearch = name;
		for (ePtrList<Descriptor>::iterator d(e->descriptor); d != e->descriptor.end(); ++d)
		{
			Descriptor *descriptor=*d;
			if (descriptor->Tag()==DESCR_COMPONENT)
			{
				ComponentDescriptor *cd=(ComponentDescriptor*)descriptor;
				
				if( cd->stream_content == 2 && cd->component_type == 5)
					cflags |= cflagDolby;
				else if( cd->stream_content == 2 && cd->component_type == 3)
					cflags |= cflagStereo;
				else if( cd->stream_content == 1 && (cd->component_type == 2 || cd->component_type == 3) )
					cflags |= cflagWide;
			}
			else if(descriptor->Tag()==DESCR_CONTENT)
			{
				ContentDescriptor *cod=(ContentDescriptor*)descriptor;

				for(ePtrList<descr_content_entry_struct>::iterator ce(cod->contentList.begin()); ce != cod->contentList.end(); ++ce)
				{
					if(genresTableShort[ce->content_nibble_level_1*16+ce->content_nibble_level_2])
					{
/*						if ( !genre.length() )
							genre+=_("GENRE: ");*/
						genre += gettext( genresTableShort[ce->content_nibble_level_1*16+ce->content_nibble_level_2] );
						genre += " ";
					}
				}
			}
		}
//  		if(!t.isNull()) name += t;

 		cname.setText(timenow);
/*		if ( genre.size() )
		{
			if ( name.size() )
				descr+=" / ";
			descr+=genre;
		}*/
		cdescr.setText( descr );
		ctime.setText( starttime + stop_time );

		int n = 0;
		n = LayoutIcon(&cdolby, (cflags & cflagDolby), n);
		n = LayoutIcon(&cstereo, (cflags & cflagStereo), n);
		n = LayoutIcon(&cformat, (cflags & cflagWide), n );
		n = LayoutIcon(&cscrambled, (cflags & cflagScrambled), n );
}

void eChannelInfo::closeEIT()
{
	if (eit)
		eit->abort();
}

void eChannelInfo::getServiceInfo( const eServiceReferenceDVB& service )
{
	closeEIT();
	delete eit;
	eit=0;
	
	// eService *service=eServiceInterface::getInstance()->addRef(service);
	
	if (!service.path.size())
	{
		DescriptionForEPGSearch = "";
		cdescr.show();
// 		cname.setFlags(RS_FADE);
		int opos=service.getDVBNamespace().get()>>16;
		if ( eSystemInfo::getInstance()->getFEType() == eSystemInfo::feSatellite )
			copos.setText(eString().sprintf("%d.%d\xC2\xB0%c", abs(opos / 10), abs(opos % 10), opos>0?'E':'W') );
		EITEvent *e = 0;
		e = eEPGCache::getInstance()->lookupEvent(service);
		if (e && eListBoxEntryService::nownextEPG)
		{
			time_t t = e->start_time+e->duration+61;
			delete e;
			e = eEPGCache::getInstance()->lookupEvent((const eServiceReferenceDVB&)service,t);
		}
			
		if (e)  // data is in cache...
		{
/*			cname.move( ePoint( clientrect.width()/8 + 109, 0 ) );
			cname.resize( eSize( clientrect.width()/8*7-119, clientrect.height()/4) );*/
			ParseEITInfo(e);
			delete e;
		}
		else  // we parse the eit...
		{
/*			cname.move( ePoint( clientrect.width()/8 + 4, 0 ) );
			cname.resize( eSize( clientrect.width()/8*7-14, clientrect.height()/4) );*/
			cdescr.setText(_("no data for this service avail"));
			eDVBServiceController *sapi=eDVB::getInstance()->getServiceAPI();
			if (!sapi)
				return;
			eServiceReferenceDVB &ref = sapi->service;

			int type = ((service.getTransportStreamID()==ref.getTransportStreamID())
				&&	(service.getOriginalNetworkID()==ref.getOriginalNetworkID())) ? EIT::tsActual:EIT::tsOther;

			eit = new EIT( EIT::typeNowNext, service.getServiceID().get(), type );
			CONNECT( eit->tableReady, eChannelInfo::EITready );
			eit->start();
		}
	} else
	{
		copos.setText("");
// 		cdescr.hide();
/*		cname.setFlags(RS_WRAP);
		cname.move( ePoint( clientrect.width()/8 + 4, 0 ) );
		cname.resize( eSize( clientrect.width()/8*7-14, clientrect.height() ));*/
		// should be moved to eService
		int filelength=service.getFileLength();
		if (filelength <= 0)
		{
			eString filename=service.path;
			int slice=0;
			struct stat64 s;
			while (!stat64((filename + (slice ? eString().sprintf(".%03d", slice) : eString(""))).c_str(), &s))
			{
				filelength+=s.st_size/1024;
				slice++;
			}
		}
		int i = service.path.rfind("/");
		i++;
		eString size;
		size.sprintf(_("\nFilesize: %d MB"), filelength/1024);
		cdescr.setText(eString(_("Filename: "))+service.path.mid( i, service.path.length()-i)+size );
	}
}
	
void eChannelInfo::EITready( int err )
{
//	eDebug("Channelinfo eit ready: %d", err);
	DescriptionForEPGSearch = "";
	if (eit->ready && !eit->error)
	{
		if ( eit->events.size() )
			ParseEITInfo(eit->events.begin());
	}
	else if ( err == -ETIMEDOUT )
		closeEIT();
}

void eChannelInfo::update( const eServiceReferenceDVB& service )
{
	if (service)
	{
		current = service;
		DescriptionForEPGSearch = "";
		getServiceInfo(current);
	}
}

void eChannelInfo::clear()
{
	cname.setText("");
	cdescr.setText("");
	ctime.setText("");
	copos.setText("");
	cdolby.hide();
	cstereo.hide();
	cformat.hide();
	cscrambled.hide();
	ptime.hide();
	ptime.setPerc(0);
}

int eChannelInfo::LayoutIcon(eLabel *icon, int doit, int num )
{
	if( doit )
	{
		int x,y;
		int dx=clientrect.width()/4;

		switch(num)
		{
			case 0:
				x=dx*2 + 65;
				y=5;
				break;
			case 1:
				x=dx*2 + 65 + 30;
				y=5;
				break;
			case 2:
				x=dx*2 + 65 + 60;
				y=5;
				break;
			case 3:
				x=dx*2 + 65 + 90;
				y=5;
				break;
			default:
				x=0;
				y=0;
				break;
		}
		icon->move(ePoint(x,y));
		icon->show();
		num++;
	}

	return num;

}

void eChannelInfo::redrawWidget(gPainter *target, const eRect& where)
{
	if ( deco )
		deco.drawDecoration(target, ePoint(width(), height()));

// 	target->line( ePoint(clientrect.left() + clientrect.width()/8 + 1, clientrect.top()),ePoint(clientrect.left() + clientrect.width()/8 + 1,clientrect.bottom()-1));
}

int eChannelInfo::eventHandler(const eWidgetEvent &event)
{
	switch (event.type)
	{
		case eWidgetEvent::changedSize:
		{
			if (deco)
				clientrect=crect;

			int dx=clientrect.width()/4;
			int dy=clientrect.height()/3;
			ctime.move( ePoint(0,0) );
			ctime.resize( eSize(dx, 30 ));

			cname.move( ePoint(dx*2 + 10,0) );
			cname.resize( eSize(50,30));

			ptime.move( ePoint(dx ,4) );
			ptime.resize( eSize(dx, 18 ));

			cdescr.move( ePoint( 0, dy - 5) );
			cdescr.resize( eSize( clientrect.width(), dy*2 + 5) );

			copos.move( ePoint( clientrect.width() - 50 , 0) );
			copos.resize( eSize(50, 30) );

			cdolby.resize( eSize(25,15) );
			cstereo.resize( eSize(25,15) );
			cformat.resize( eSize(25,15) );
			cscrambled.resize( eSize(25,15) );

			invalidate();
			break;
    }
		default:
		break;
	}
	return eDecoWidget::eventHandler(event);
}

static eWidget *create_eChannelInfo(eWidget *parent)
{
	return new eChannelInfo(parent);
}

class eChannelInfoSkinInit
{
public:
	eChannelInfoSkinInit()
	{
		eSkin::addWidgetCreator("eChannelInfo", create_eChannelInfo);
	}
	~eChannelInfoSkinInit()
	{
		eSkin::removeWidgetCreator("eChannelInfo", create_eChannelInfo);
	}
};

eAutoInitP0<eChannelInfoSkinInit> init_eChannelInfoSkinInit(eAutoInitNumbers::guiobject, "eChannelInfo");
