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
#ifndef __setosdpos_h
#define __setosdpos_h

#include <lib/gui/ewindow.h>
#include <lib/gui/emessage.h>
#include <lib/gui/statusbar.h>
#include <lib/gui/elabel.h>
#include <lib/gui/slider.h>
#include <lib/gui/ebutton.h>
#include <lib/gui/echeckbox.h>
#include <lib/gui/textinput.h>
#include <lib/gdi/font.h>


class eOsdPosition: public eWindow
{
	eCheckbox *osdLight;
	eTextInputField *input[2];
	eLabel *label[2], *lblhor, *lblver;
	eStatusBar *status;
	eSlider *sHor, *sVer;
	eButton *bt_exit,*bt_ok,*bt_test;
	void saveParameter();
	void testOsd();
	void horChanged(int i);
	void verChanged(int i);
public:
	eOsdPosition();
	~eOsdPosition();
};

class eOsdPositionTest: public eWidget
{
	eWidget *dvrInfoBar, *dvbInfoBar, *fileInfoBar;
// 	eButton *bt_exit;
	eLabel *ChannelNumber, *ChannelName, *Clock,
		*lsnr_num, *lsync_num, *lber_num, 
		*lsnr_text, *lagc_text, *lber_text,
		*EINow, *EINext, 
		*EINowDuration, *EINextDuration,
		*EINowTime, *EINextTime, 
		*Description, *fileinfos,
		*ButtonRedEn, *ButtonRedDis,
		*ButtonGreenEn, *ButtonGreenDis,
		*ButtonYellowEn, *ButtonYellowDis,
		*ButtonBlueEn, *ButtonBlueDis,
		*DolbyOn, *DolbyOff, *CryptOn, *CryptOff,
		*WideOn, *WideOff, *recstatus,
		*DVRSpaceLeft,*AudioOrPause,
		*dc_emm,*nds_emm, *via_emm, *cw_emm, *beta_emm, *conax_emm, *seca_emm, *irdeto_emm, *nagra_emm,
		*dc_ecm,*nds_ecm, *via_ecm, *cw_ecm, *beta_ecm, *conax_ecm, *seca_ecm, *irdeto_ecm, *nagra_ecm,
		*dc_no,*nds_no, *via_no, *cw_no, *beta_no, *conax_no, *seca_no, *irdeto_no, *nagra_no,
		*cam_info, *feq, *sr_snr, *fc_fec, *pro_name,*pol,*ecm_info, *ca_id, *prov_id, *pid_id, *ecm_time, *hops,
		*button_card, *button_emu, *button_spider, *netcard_info, *videoformat, *DDate, *picon;

public:
	eOsdPositionTest(bool light=0, int h_offset=0, int v_offset=0);
	~eOsdPositionTest();
};
#endif
