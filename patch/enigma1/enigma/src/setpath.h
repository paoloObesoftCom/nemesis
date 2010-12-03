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
#ifndef __setpath_h
#define __setpath_h

#include <lib/gui/ewindow.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/elabel.h>
#include <lib/gui/textinput.h>
#include <lib/gui/emessage.h>
#include <lib/gui/eskin.h>
#include <lib/gui/combobox.h>
#include <lib/gui/statusbar.h>

#include <enigma_main.h>

class eSetpath: public eWindow
{
	eLabel *label;
	eButton *bt_exit,*bt_def,*bt_save,*bt_rec;
	eTextInputField *recdir;
	eComboBox *picondir;
	eStatusBar *sbar;
	void saveDir();
	void setDef();
	void recoverMovieList();
	bool recover_movies();
public:
	eSetpath();
	~eSetpath();
};

#endif
