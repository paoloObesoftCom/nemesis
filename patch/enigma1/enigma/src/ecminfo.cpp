 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: apps/tuxbox/plugins/enigma/script/script.cpp
 ****************************************************************************************************
 */
#include "ecminfo.h"

void ecmInfo::readInfo()
{
	setText(_("ECM Information"));
	eString ecmTextInfo;
	ecmTextInfo = "";
	char buf[256];
	int serviceFlags = eServiceInterface::getInstance()->getService()->getFlags();
	if( serviceFlags & eServiceHandler::flagIsScrambled )
	{
		FILE *f = fopen("/tmp/ecm.info", "r");
		if (f)
		{	int len = 0;
			while (fgets(buf, 256, f))
				{
				len += strlen(buf);
				if (len <= 65536)
				ecmTextInfo += eString().sprintf("%s", buf);
				}
			fclose(f);
		}
	}else
		ecmTextInfo = _("No ECM info");
	label->setText(ecmTextInfo);
	updateScrollbar();
	label->show();
}

void ecmInfo::readPidInfo()
{
	setText(_("PID Information"));
	eString ecmTextInfo;
	ecmTextInfo = "";
	char buf[256];
	FILE *f = fopen("/tmp/pid.info", "r");
	if (f)
	{	int len = 0;
		while (fgets(buf, 256, f))
			{
			len += strlen(buf);
			if (len <= 65536)
			ecmTextInfo += eString().sprintf("%s", buf);
			}
		fclose(f);
	}else
		ecmTextInfo = _("No PID info.");
	label->setText(ecmTextInfo);
	updateScrollbar();
	label->show();
}

void ecmInfo::readEmmInfo()
{
	setText(_("EMM Information"));
	eString ecmTextInfo;
	ecmTextInfo = "";
	char buf[256];
	FILE *f = fopen("/tmp/emm.info", "r");
	if (f)
	{	int len = 0;
		while (fgets(buf, 256, f))
			{
			len += strlen(buf);
			if (len <= 65536)
			ecmTextInfo += eString().sprintf("%s", buf);
			}
		fclose(f);
	}else
		ecmTextInfo = _("No EMM info.");
	label->setText(ecmTextInfo);
	updateScrollbar();
	label->show();
}

ecmInfo::ecmInfo(): eWindow(1),refreshTimer(eApp)
{
	int winW = 550;
	int winH = 280;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));

	scrollbar = new eProgress(this);
	scrollbar->setName("scrollbar");
	scrollbar->setStart(0);
	scrollbar->setPerc(100);
	scrollbar->move(ePoint(clientrect.width() - 25, 5));
	scrollbar->resize(eSize(20, clientrect.height() - 45));
	scrollbar->setProperty("direction", "1");
	
	visible = new eWidget(this);
	visible->setName("visible");
	visible->move(ePoint(5, 5));
	visible->resize(eSize(clientrect.width() - 30, clientrect.height() - 40));

	label = new eLabel(visible);
	label->setFont(eSkin::getActive()->queryFont("eChannelInfo"));
	float lineheight = fontRenderClass::getInstance()->getLineHeight(label->getFont());
	int lines = (int) (visible->getSize().height() / lineheight);
	pageHeight = (int) (lines * lineheight);
	visible->resize(eSize(visible->getSize().width(), pageHeight + (int) (lineheight / 6)));
	label->resize(eSize(visible->getSize().width(), pageHeight * 16));
	label->setFlags(RS_WRAP);

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_abort=new eButton(this);
	bt_abort->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_abort->move(ePoint(5 , clientrect.height()-35));
	bt_abort->resize(eSize(100, 30));
	bt_abort->setShortcut("red");
	bt_abort->setShortcutPixmap("red");
	bt_abort->setText(_("Close"));
	bt_abort->setAlign(0);
	if (showBorder) bt_abort->loadDeco();
	CONNECT(bt_abort->selected, eWidget::reject);

	bt_emm=new eButton(this);
	bt_emm->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_emm->move(ePoint(clientrect.width()-315 , clientrect.height()-35));
	bt_emm->resize(eSize(100, 30));
	bt_emm->setShortcut("green");
	bt_emm->setShortcutPixmap("green");
	bt_emm->setText(_("EMM Info"));
	bt_emm->setAlign(0);
	if (showBorder) bt_emm->loadDeco();
	CONNECT(bt_emm->selected, ecmInfo::setEmm);

	bt_ecm=new eButton(this);
	bt_ecm->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_ecm->move(ePoint(clientrect.width()-210 , clientrect.height()-35));
	bt_ecm->resize(eSize(100, 30));
	bt_ecm->setShortcut("yellow");
	bt_ecm->setShortcutPixmap("yellow");
	bt_ecm->setText(_("ECM Info"));
	bt_ecm->setAlign(0);
	if (showBorder) bt_ecm->loadDeco();
	CONNECT(bt_ecm->selected, ecmInfo::setEcm);

	bt_pid=new eButton(this);
	bt_pid->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_pid->move(ePoint(clientrect.width()-105 , clientrect.height()-35));
	bt_pid->resize(eSize(100, 30));
	bt_pid->setShortcut("blue");
	bt_pid->setShortcutPixmap("blue");
	bt_pid->setText(_("PID Info"));
	bt_pid->setAlign(0);
	if (showBorder) bt_pid->loadDeco();
	CONNECT(bt_pid->selected, ecmInfo::setPid);

	label->hide();
	label->move(ePoint(0, 0));
	readInfo();

	refreshTimer.start(1000,false);
	refreshTimerConnection = CONNECT( refreshTimer.timeout, ecmInfo::readInfo );

}

ecmInfo::~ecmInfo()
{
	if ( refreshTimerConnection.connected() )
		refreshTimerConnection.disconnect();
	refreshTimer.stop();
}

void ecmInfo::setPid()
{
	if ( refreshTimerConnection.connected() )
		refreshTimerConnection.disconnect();
	refreshTimer.stop();
	readPidInfo();
}

void ecmInfo::setEcm()
{
	if ( refreshTimerConnection.connected() )
		refreshTimerConnection.disconnect();
	refreshTimer.stop();
	readInfo();
	refreshTimer.start(1000,false);
	refreshTimerConnection = CONNECT( refreshTimer.timeout, ecmInfo::readInfo );
}

void ecmInfo::setEmm()
{
	if ( refreshTimerConnection.connected() )
		refreshTimerConnection.disconnect();
	refreshTimer.stop();
	readEmmInfo();
	refreshTimer.start(1000,false);
	refreshTimerConnection = CONNECT( refreshTimer.timeout, ecmInfo::readEmmInfo );
}

int ecmInfo::eventHandler(const eWidgetEvent & event)
{
   switch (event.type)
   {
   case eWidgetEvent::evtAction:
      if (total && event.action == &i_cursorActions->up)
      {
         ePoint curPos = label->getPosition();
         if (curPos.y() < 0)
         {
            label->move(ePoint(curPos.x(), curPos.y() + pageHeight));
            updateScrollbar();
         }
      }
      else if (total && event.action == &i_cursorActions->down)
      {
         ePoint curPos = label->getPosition();
         if ((total - pageHeight) >= abs(curPos.y() - pageHeight))
         {
            label->move(ePoint(curPos.x(), curPos.y() - pageHeight));
            updateScrollbar();
         }
      }
      else if (event.action == &i_cursorActions->cancel)
         close(0);
      else
         break;
      return 1;
   default:
      break;
   }
   return eWindow::eventHandler(event);
}

void ecmInfo::updateScrollbar()
{
   total = pageHeight;
   int pages = 1;
   while (total < label->getExtend().height())
   {
      total += pageHeight;
      pages++;
   }

   int start = -label->getPosition().y() * 100 / total;
   int vis = pageHeight * 100 / total;
   scrollbar->setParams(start, vis);
   scrollbar->show();
   if (pages == 1)
      total = 0;
}
