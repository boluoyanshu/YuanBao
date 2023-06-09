#include "includes.h"
#include  "app.h"

static const GUI_WIDGET_CREATE_INFO _aDialogCreateClock[] = {
  { FRAMEWIN_CreateIndirect, "Clock",   0,                0, 0, 240, 320,   0, 0x64, 0 },
//	{ DROPDOWN_CreateIndirect, "rate",       GUI_ID_DROPDOWN0, 75, 30, 60, 36, 0, 0x0, 0 },	
//  { DROPDOWN_CreateIndirect, "Agc",        GUI_ID_DROPDOWN1, 160, 30, 60, 86, 0, 0x0, 0 },
//	{ RADIO_CreateIndirect,    "input",      GUI_ID_RADIO0,    5, 30, 60, 44, 0, 0x1402, 0 },
//	{ RADIO_CreateIndirect,    "Channel",    GUI_ID_RADIO1,    5, 90, 60, 66, 0, 0x1402, 0 },
//	
//  //{ SLIDER_CreateIndirect,   "Volume",     GUI_ID_SLIDER0,   130, 160, 90, 20, 0, 0x0,  0 },	
//  { BUTTON_CreateIndirect,   "Record",     GUI_ID_BUTTON0,   40, 190, 58, 58,  0, 0x0,  0 },
//  { BUTTON_CreateIndirect,   "Replay",     GUI_ID_BUTTON1,   135, 190, 58, 58, 0, 0x0,  0 },
	{ TEXT_CreateIndirect,     "Hour:",       GUI_ID_TEXT0,     5, 70, 100, 40, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,     "Minute:",       GUI_ID_TEXT1,     85, 70, 100, 40, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,     "Second",       GUI_ID_TEXT2,     165, 70, 100, 40, 0, 0x64, 0 },
		{ TEXT_CreateIndirect,     "Temperature:",       GUI_ID_TEXT3,     5, 130, 100, 40, 0, 0x64, 0 },
	{ TEXT_CreateIndirect,     "Humidity",       GUI_ID_TEXT4,     165, 130, 100, 40, 0, 0x64, 0 }
  // USER START (Optionally insert additional widgets)
  // USER END
};

static void _cbDialogMusic(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  uint8_t i=0,j=0;
  switch (pMsg->MsgId) {
	case WM_DELETE:
		OS_INFO("Clockapp delete\n");	
		VS_HD_Reset();
		//VS_Soft_Reset();
//		PROGBAR_SetDefaultSkin(PROGBAR_SKIN_FLEX);
		UserApp_Flag = 0;
	  Flag_ICON002 = 0;		
		tpad_flag=0;
		break;
  case WM_INIT_DIALOG:
		    //
    // Initialization of 'Clock'
    //
   	hItem = pMsg->hWin;
		FRAMEWIN_SetTextColor(hItem,GUI_DARKGRAY);
		FRAMEWIN_SetFont(hItem, GUI_FONT_16B_ASCII);
		FRAMEWIN_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
		FRAMEWIN_AddCloseButton(hItem,FRAMEWIN_BUTTON_RIGHT,0);
		FRAMEWIN_SetTitleHeight(hItem, 20);
	//
	// Initialization of 'Music'
	//
		break;
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

void FUN_ICON002Clicked(void)
{
	WM_HWIN hWin;

	hWin = GUI_CreateDialogBox(_aDialogCreateClock, GUI_COUNTOF(_aDialogCreateClock), _cbDialogMusic, WM_HBKWIN, 0, 0);

}