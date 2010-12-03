 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs
 ****************************************************************************************************
 */

#include "escriptexecute.h"

eScriptExecute::eScriptExecute(eString scriptToRun, eString Title,int wsize, int hsize):
eWindow(1)
{
	int sW = 720;
	int sH = 576;
	int winW = wsize;
	int winH = hsize;
	cmove(ePoint((sW-winW) / 2,((sH - winH) / 2) + 15));
	cresize(eSize(winW, winH));
	setText(Title);

	scrollbar = new eProgress(this);
	scrollbar->setStart(0);
	scrollbar->setPerc(100);
	scrollbar->move(ePoint(winW - 25, 5));
	scrollbar->resize(eSize(20, winH - 40));
	scrollbar->setProperty("direction", "1");

	visible = new eWidget(this);
	visible->move(ePoint(5, 5));
	visible->resize(eSize(winW - 30, winH - 40));

	label = new eLabel(visible);
	label->setFlags(RS_WRAP);
	float lineheight = fontRenderClass::getInstance()->getLineHeight(label->getFont());
	int lines = (int) (visible->getSize().height() / lineheight);
	pageHeight = (int) (lines * lineheight);
	visible->resize(eSize(visible->getSize().width(), pageHeight + (int) (lineheight / 6)));
	label->resize(eSize(visible->getSize().width(), pageHeight * 64));

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bClose=new eButton(this,0,0);
	bClose->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bClose->move(ePoint(winW / 2 - 50, winH - 35));
	bClose->resize(eSize(100, 30));
	bClose->setShortcut("red");
	bClose->setShortcutPixmap("red");
	bClose->setText(_("Close"));
	bClose->setAlign(0);
	if (showBorder) bClose->loadDeco();
	CONNECT( bClose->selected, eScriptExecute::accept );
	bClose->hide();

	bCancel=new eButton(this,0,0);
	bCancel->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bCancel->move(ePoint(winW / 2 - 50, winH - 35));
	bCancel->resize(eSize(100, 30));
	bCancel->setShortcut("yellow");
	bCancel->setShortcutPixmap("yellow");
	bCancel->setText(_("Cancel"));
	bCancel->setAlign(0);
	if (showBorder) bCancel->loadDeco();
	CONNECT( bCancel->selected, eScriptExecute::onCancel );

	valign();

	label->hide();
	label->move(ePoint(0, 0));
	label->setText(eString().sprintf(_("Executing script.\nPlease wait...\n"), scriptToRun.c_str()));
	eWindow::globalCancel(eWindow::OFF);
	script = new eConsoleAppContainer(scriptToRun);
	if (!script->running())
		label->setText(eString().sprintf(_("Could not execute %s"), scriptToRun.c_str()));
	else
	{
		eDebug("%s started", scriptToRun.c_str());
		CONNECT(script->dataAvail, eScriptExecute::getData);
		CONNECT(script->appClosed, eScriptExecute::scriptClosed);
	}
	updateScrollbar();
	label->show();
}

eScriptExecute::~eScriptExecute()
{
}

int eScriptExecute::eventHandler(const eWidgetEvent & event)
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
		else
			break;
		return eWindow::eventHandler( event );
	default:
		break;
	}
	return eWindow::eventHandler(event);
}

void eScriptExecute::updateScrollbar()
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

void eScriptExecute::onCancel()
{
	if (script)
	{
		if (script->running())
			script->kill();
		delete script;
		script = 0;
	}
	label->hide();
	eString tmp = label->getText();
	tmp+="\nScript stopped by user!";
	label->setText(tmp);
	updateScrollbar();
	label->show();
	eWindow::globalCancel(eWindow::ON);
	bCancel->hide();
	bClose->show();
}

void eScriptExecute::getData(eString str)
{
	label->hide();
	eString tmp = label->getText();
	tmp+=str;
	label->setText(tmp);
	updateScrollbar();
	label->show();
}

void eScriptExecute::scriptClosed(int state)
{
	if (script)
	{
		delete script;
		script = 0;
	}
	label->hide();
	eString tmp = label->getText();
	tmp+="\nScript finished!";
	label->setText(tmp);
	updateScrollbar();
	label->show();
	eWindow::globalCancel(eWindow::ON);
	bCancel->hide();
	bClose->show();
}
