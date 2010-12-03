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


#ifndef __escriptexecute_h
#define __escriptexecute_h

#include <lib/base/console.h>
#include <lib/gui/ewindow.h>
#include <lib/gui/guiactions.h>
#include <lib/gui/elabel.h>
#include <lib/gui/eskin.h>
#include <lib/gdi/font.h>
#include <lib/gui/eprogress.h>
#include <lib/gui/ebutton.h>

class eScriptExecute: public eWindow
{
private:
	eButton *bCancel, *bClose;
	eLabel *label;
	eWidget *visible;
	eProgress *scrollbar;
	int pageHeight;
	int total;
	int lines;
	int eventHandler(const eWidgetEvent &event);
	void updateScrollbar();
	eConsoleAppContainer *script;
	void getData( eString );
	void scriptClosed(int);
	void onCancel();
public:
	eScriptExecute(eString scriptToRun,eString Title,int wsize, int hsize);
	~eScriptExecute();
};

#endif
