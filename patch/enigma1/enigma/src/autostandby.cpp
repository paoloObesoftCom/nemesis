 /*
 ****************************************************************************************************
 N E M E S I S
 Public sources
 Author: Gianathem
 Copyright (C) 2007  Nemesis - Team
 http://www.genesi-project.it/forum
 Please if you use this source, refer to Nemesis -Team

 A part of this code is based from: enigma cvs
 Special thanks a tension for this code
 ****************************************************************************************************
 */
#include "autostandby.h"

eAutoStandbySelWindow::eAutoStandbySelWindow()
:eWindow(0)
{
    cmove(ePoint(180, 160));
    cresize(eSize(360, 300));
    setText(_("AutoStandby Manager"));

    int autostandby=0;
    eConfig::getInstance()->getKey("/extras/autostandby", autostandby);

    stand = new eCheckbox(this);
    stand->move(ePoint(20,40));
    stand->resize(eSize(200,40));
    if (autostandby==3)
        stand->setCheck(1);
    stand->loadDeco();
    stand->setText(_("Auto-Standby"));
    stand->setHelpText(_("Go to standby after (unwanted) reboot"));
    CONNECT(stand->checked, eAutoStandbySelWindow::standChanged);

    shut = new eCheckbox(this);
    shut->move(ePoint(20,100));
    shut->resize(eSize(200,40));
    if (autostandby==2)
        shut->setCheck(1);
    shut->loadDeco();
    shut->setText(_("Auto-Shutdown"));
    shut->setHelpText(_("Shutdown after (unwanted) reboot"));
    CONNECT(shut->checked, eAutoStandbySelWindow::shutChanged);

    sbar = new eStatusBar(this);
    sbar->move(ePoint(0, 250));
    sbar->resize(eSize(360, 50));
    sbar->loadDeco();
    sbar->setName("status");
}

eAutoStandbySelWindow::~eAutoStandbySelWindow()
{

}

void eAutoStandbySelWindow::standChanged( int checked )
{
    if ( checked )
       {
        shut->setCheck( 0 );
        eConfig::getInstance()->setKey("/extras/autostandby", 3);
       }
    else
        eConfig::getInstance()->setKey("/extras/autostandby", 0);
}

void eAutoStandbySelWindow::shutChanged( int checked )
{
    if ( checked )
       {
        stand->setCheck( 0 );
        eConfig::getInstance()->setKey("/extras/autostandby", 2);
       }
    else
        eConfig::getInstance()->setKey("/extras/autostandby", 0);
}
