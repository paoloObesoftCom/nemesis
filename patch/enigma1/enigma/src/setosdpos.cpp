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
#include "setosdpos.h"
#include <enigma.h>

eOsdPosition::eOsdPosition()
:eWindow(0)
{
	int sW = 720;
	int sH = 576;
	int winW = 320;
	int winH = 270;
	cresize(eSize(winW, winH));
    setText(_("Setting skin Position"));

	int v_offset = 0, h_offset = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/infobar/h_offset", h_offset );
	eConfig::getInstance()->getKey("/Nemesis-Project/infobar/v_offset", v_offset );

	cmove(ePoint(((sW-winW) / 2) + h_offset ,((sH - winH) / 2) + 15 + v_offset));

	int labelWidth = clientrect.width() - 45;
	int textWidth = clientrect.width() - labelWidth -15 ;

	label[0]=new eLabel(this);
	label[0]->move(ePoint(5, 20));
	label[0]->resize(eSize(labelWidth, 25));
	label[0]->setText(_("Horizontal offset"));
	label[0]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[0]->setAlign(eTextPara::dirCenter);

	sHor = new eSlider( this, label[0], 0, 50 );
	sHor->setIncrement(2);
	sHor->move( ePoint(5, 50) );
	sHor->resize(eSize( labelWidth , 25) );
	sHor->setHelpText(_("change the horizontal position"));
	sHor->setValue((h_offset+25));
	CONNECT( sHor->changed, eOsdPosition::horChanged );

	lblhor = new eLabel(this);
	lblhor->move(ePoint(labelWidth +10, 52 ));
	lblhor->resize(eSize(25, 25));
	lblhor->setText(eString().sprintf("%i",h_offset).c_str());
	lblhor->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	lblhor->setAlign(eTextPara::dirCenter);

	label[1]=new eLabel(this);
	label[1]->move(ePoint(5, 90));
	label[1]->resize(eSize(labelWidth, 25));
	label[1]->setText(_("Vertical offset"));
	label[1]->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	label[1]->setAlign(eTextPara::dirCenter);

	sVer = new eSlider( this, label[1], 0, 50 );
	sVer->setIncrement(2);
	sVer->move( ePoint(5, 120) );
	sVer->resize(eSize( labelWidth , 25) );
	sVer->setHelpText(_("change the vertical position"));
	sVer->setValue((v_offset+25));
	CONNECT( sVer->changed, eOsdPosition::verChanged );

	lblver = new eLabel(this);
	lblver->move(ePoint(labelWidth + 10, 122 ));
	lblver->resize(eSize(25, 25));
	lblver->setText(eString().sprintf("%i",v_offset).c_str());
	lblver->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	lblver->setAlign(eTextPara::dirCenter);

	osdLight = new eCheckbox(this,0,1);
	osdLight->setText(_("Test lite skin"));
	osdLight->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	osdLight->move(ePoint(5, 160));
	osdLight->resize(eSize( labelWidth, 30));
	osdLight->setHelpText(_("Test lite skin infobar"));

	int showBorder = 0;
	eConfig::getInstance()->getKey("/Nemesis-Project/tools/showborder", showBorder);

	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(5, clientrect.height()-75));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setAlign(0);
	if (showBorder) bt_exit->loadDeco();
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close"));
	CONNECT(bt_exit->selected, eWidget::reject);

	bt_test=new eButton(this);
	bt_test->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_test->move(ePoint(110, clientrect.height()-75));
	bt_test->resize(eSize(100, 30));
	bt_test->setShortcut("yellow");
	bt_test->setShortcutPixmap("yellow");
	bt_test->setAlign(0);
	if (showBorder) bt_test->loadDeco();
	bt_test->setText(_("Test"));
	bt_test->setHelpText(_("Test skin position"));
	CONNECT(bt_test->selected, eOsdPosition::testOsd);

	bt_ok=new eButton(this);
	bt_ok->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_ok->move(ePoint(clientrect.width()-105 , clientrect.height()-75));
	bt_ok->resize(eSize(100, 30));
	bt_ok->setShortcut("blue");
	bt_ok->setShortcutPixmap("blue");
	bt_ok->setAlign(0);
	if (showBorder) bt_ok->loadDeco();
	bt_ok->setText(_("Save"));
	bt_ok->setHelpText(_("Save skin position"));
	CONNECT(bt_ok->selected, eOsdPosition::saveParameter);

	status = new eStatusBar(this);
	status->move( ePoint(0, clientrect.height()- 40) );
	status->resize(eSize(clientrect.width(), 40));
	status->loadDeco();	
}

eOsdPosition::~eOsdPosition()
{
}

void eOsdPosition::horChanged(int i)
{
	int winW = clientrect.width();	int winH = clientrect.height();
	i = i - 25;
	lblhor->setText(eString().sprintf("%i",i).c_str());
	int v_offset = sVer->getValue() - 25;
	hide();
		cmove(ePoint(((720-winW) / 2) + i ,((576 - winH) / 2) + 15 + v_offset));
	show();
	setFocus(sHor);
}

void eOsdPosition::verChanged(int i)
{
	int winW = clientrect.width();	int winH = clientrect.height();
	i = i - 25;
	lblver->setText(eString().sprintf("%i",i).c_str());
	int h_offset = sHor->getValue() - 25;
	hide();
		cmove(ePoint(((720-winW) / 2) + h_offset ,((576 - winH) / 2) + 15 + i));
	show();
	setFocus(sVer);
}

void eOsdPosition::saveParameter()
{
	int h_offset = sHor->getValue() - 25;
	int v_offset = sVer->getValue() - 25;
	eConfig::getInstance()->setKey("/Nemesis-Project/infobar/h_offset", h_offset );
	eConfig::getInstance()->setKey("/Nemesis-Project/infobar/v_offset", v_offset );
	eConfig::getInstance()->flush();
	eConfig::getInstance()->setKey("/Nemesis-Project/infobar/changed_offset", 1 );
	close(0);
}

void eOsdPosition::testOsd()
{
	
	hide();	
		eOsdPositionTest dlg(osdLight->isChecked(), sHor->getValue() - 25, sVer->getValue() - 25); 
		dlg.show();sleep(3); dlg.hide();
// 		dlg.show();dlg.exec(); dlg.hide();
	show();
}

eOsdPositionTest::eOsdPositionTest(bool light, int h_offset, int v_offset)
	:eWidget(0,1)
{
	dvrInfoBar=new eLabel(this);dvrInfoBar->setName("dvrInfoBar");dvrInfoBar->hide();
	dvbInfoBar=new eWidget(this);dvbInfoBar->setName("dvbInfoBar");dvbInfoBar->hide();
	fileInfoBar=new eWidget(this);fileInfoBar->setName("fileInfoBar");fileInfoBar->hide();

	recstatus=new eLabel(this);recstatus->setName("recStatus");recstatus->hide();
	DVRSpaceLeft=new eLabel(dvrInfoBar);DVRSpaceLeft->setName("TimeLeft");DVRSpaceLeft->hide();
	fileinfos=new eLabel(fileInfoBar);fileinfos->setName("fileinfos");fileinfos->hide();

	eSkin *skin=eSkin::getActive();
	if (light)
	{	
		if (skin->build(this, "ezap_lite"))
		{
			eString msg;
			msg.sprintf("Unable to load light skin, main will be loaded");
			eMessageBox mb(msg.c_str(), _("Lite skin not found"));
			mb.show(); sleep(1); mb.hide();
			if (skin->build(this, "ezap_main"))
				eFatal("skin load of \"ezap_lite\" failed");
		}
	}
	else
	{
		if (skin->build(this, "ezap_main"))
			eFatal("skin load of \"ezap_main\" failed");
	}

	ASSIGN(ChannelNumber, eLabel, "ch_number");ChannelNumber->hide();
	ASSIGN(ChannelName, eLabel, "ch_name");ChannelName->setText("TEST INFOBAR OFFSET");
	
	ASSIGN(lsnr_num, eLabel, "snr_num");lsnr_num->hide();
	ASSIGN(lsync_num, eLabel, "agc_num");lsync_num->hide();
	ASSIGN(lber_num, eLabel, "ber_num");lber_num->hide();
	
	ASSIGN(lsnr_text, eLabel, "snr_text");lsnr_text->setText("SNR:");
	ASSIGN(lagc_text, eLabel, "agc_text");lagc_text->setText("AGC:");
	ASSIGN(lber_text, eLabel, "ber_text");lber_text->setText("BER:");
	
	ASSIGN(EINow, eLabel, "e_now_title");EINow->hide();
	ASSIGN(EINext, eLabel, "e_next_title");EINext->hide();
	
	ASSIGN(EINowDuration, eLabel, "e_now_duration");EINowDuration->hide();
	ASSIGN(EINextDuration, eLabel, "e_next_duration");EINextDuration->hide();
	
	ASSIGN(EINowTime, eLabel, "e_now_time");EINowTime->hide();
	ASSIGN(EINextTime, eLabel, "e_next_time");EINextTime->hide();

	ASSIGN(Description, eLabel, "description");Description->hide();
	
	ASSIGN(nds_emm, eLabel ,"nds_emm");nds_emm->hide();
	ASSIGN(via_emm, eLabel ,"via_emm");via_emm->hide();
	ASSIGN(cw_emm, eLabel ,"cw_emm");cw_emm->hide();
	ASSIGN(beta_emm, eLabel ,"beta_emm");beta_emm->hide();
	ASSIGN(conax_emm, eLabel ,"conax_emm");conax_emm->hide();
	ASSIGN(seca_emm, eLabel ,"seca_emm");seca_emm->hide();
	ASSIGN(irdeto_emm, eLabel ,"irdeto_emm");irdeto_emm->hide();
	ASSIGN(nagra_emm, eLabel ,"nagra_emm");nagra_emm->hide();
	ASSIGN(dc_emm, eLabel ,"dc_emm");dc_emm->hide();
		
	ASSIGN(nds_ecm, eLabel ,"nds_ecm");nds_ecm->hide();
	ASSIGN(via_ecm, eLabel ,"via_ecm");via_ecm->hide();
	ASSIGN(cw_ecm, eLabel ,"cw_ecm");cw_ecm->hide();
	ASSIGN(beta_ecm, eLabel ,"beta_ecm");beta_ecm->hide();
	ASSIGN(conax_ecm, eLabel ,"conax_ecm");conax_ecm->hide();
	ASSIGN(seca_ecm, eLabel ,"seca_ecm");seca_ecm->hide();
	ASSIGN(irdeto_ecm, eLabel ,"irdeto_ecm");irdeto_ecm->hide();
	ASSIGN(nagra_ecm, eLabel ,"nagra_ecm");nagra_ecm->hide();
	ASSIGN(dc_ecm, eLabel ,"dc_ecm");dc_ecm->hide();
	
/*	ASSIGN(nds_no, eLabel ,"nds_no");nds_no->show();
	ASSIGN(via_no, eLabel ,"via_no");via_no->show();
	ASSIGN(cw_no, eLabel ,"cw_no");cw_no->show();
	ASSIGN(beta_no, eLabel ,"beta_no");beta_no->show();
	ASSIGN(conax_no, eLabel ,"conax_no");conax_no->show();
	ASSIGN(seca_no, eLabel ,"seca_no");seca_no->show();
	ASSIGN(irdeto_no, eLabel ,"irdeto_no");irdeto_no->show();
	ASSIGN(nagra_no, eLabel ,"nagra_no");nagra_no->show();
	ASSIGN(dc_no, eLabel ,"dc_no");dc_no->show();*/
	
	ASSIGN(picon, eLabel ,"picon");picon->hide();
	ASSIGN(cam_info, eLabel ,"cam_info");cam_info->hide();
	ASSIGN(ecm_info, eLabel ,"ecm_info");ecm_info->hide();

/*	ASSIGN(feq, eLabel ,"feq");feq->show();
	ASSIGN(pol, eLabel ,"pol");pol->show();
	ASSIGN(sr_snr, eLabel ,"sr_snr");sr_snr->show();
	ASSIGN(fc_fec, eLabel ,"fc_fec");fc_fec->show();*/
	ASSIGN(pro_name, eLabel ,"pro_name");pro_name->setText("EDG-Nemesis");

	ASSIGN(ca_id, eLabel ,"ca_id");ca_id->show();
	ASSIGN(prov_id, eLabel ,"prov_id");prov_id->show();
	ASSIGN(pid_id, eLabel ,"pid_id");pid_id->show();
	ASSIGN(ecm_time, eLabel ,"ecm_time");ecm_time->show();
	ASSIGN(hops, eLabel ,"hops");hops->show();
	
/*	ASSIGN(netcard_info, eLabel ,"netcard_info");netcard_info->show();
	ASSIGN(videoformat, eLabel ,"videoformat");videoformat->show();*/
	ASSIGN(button_card, eLabel ,"button_card");button_card->hide();
	ASSIGN(button_emu, eLabel ,"button_emu");button_emu->hide();
	ASSIGN(button_spider, eLabel ,"button_spider");button_spider->hide();
	
	ASSIGN(ButtonRedEn, eLabel, "button_red_enabled");ButtonRedEn->hide();
	ASSIGN(ButtonGreenEn, eLabel, "button_green_enabled");ButtonGreenEn->hide();
	ASSIGN(ButtonYellowEn, eLabel, "button_yellow_enabled");ButtonYellowEn->hide();
/*	ASSIGN(ButtonBlueEn, eLabel, "button_blue_enabled");ButtonBlueEn->show();
	ASSIGN(ButtonRedDis, eLabel, "button_red_disabled");ButtonRedDis->show();
	ASSIGN(ButtonGreenDis, eLabel, "button_green_disabled");ButtonGreenDis->show();
	ASSIGN(ButtonYellowDis, eLabel, "button_yellow_disabled");ButtonYellowDis->show();*/
	ASSIGN(ButtonBlueDis, eLabel, "button_blue_disabled");ButtonBlueDis->hide();
	ASSIGN(AudioOrPause,eLabel, "AudioOrPause");AudioOrPause->hide();
	
	ASSIGN(DolbyOn, eLabel, "osd_dolby_on");DolbyOn->hide();
// 	ASSIGN(CryptOn, eLabel, "osd_crypt_on");CryptOn->show();
	ASSIGN(WideOn, eLabel, "osd_format_on");WideOn->hide();
	
// 	ASSIGN(DolbyOff, eLabel, "osd_dolby_off");DolbyOff->show();
	ASSIGN(CryptOff, eLabel, "osd_crypt_off");CryptOff->hide();
// 	ASSIGN(WideOff, eLabel, "osd_format_off");WideOff->show();
	
	ASSIGN(Clock, eLabel, "time");Clock->setText("88:88:88");
	ASSIGN(DDate, eLabel, "date");DDate->hide();

	ePoint skinPos=getPosition();
 	cmove(ePoint(skinPos.x() + h_offset , skinPos.y() + v_offset));

/*	bt_exit=new eButton(this);
	bt_exit->setFont(eSkin::getActive()->queryFont("eStatusBar"));
	bt_exit->move(ePoint(clientrect.width()/2 - 50, clientrect.height()/2 - 15));
	bt_exit->resize(eSize(100, 30));
	bt_exit->setShortcut("red");
	bt_exit->setShortcutPixmap("red");
	bt_exit->setAlign(0);
 	bt_exit->loadDeco();
	bt_exit->setText(_("Close"));
	bt_exit->setHelpText(_("Close"));
	CONNECT(bt_exit->selected, eWidget::reject);
	setFocus(bt_exit);*/
}

eOsdPositionTest::~eOsdPositionTest()
{
}
