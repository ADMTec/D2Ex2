#include "stdafx.h"
#include "ExOptions.h"
#include "Build.h"

//---Menu recon stuff
static BOOL MenuValueClicked;
static BOOL isMenuClicked;
static DWORD MenuKeyClicked;


//---Menu recon stuff

void ExOptions::ShowHide()
{
static bool wasAutoMap;
	if(D2Vars::D2CLIENT_UIModes[UI_MAINMENU]) {
		D2Vars::D2CLIENT_UIModes[UI_MAINMENU] = 0;
		sMsg* pMsgs = &D2Vars::D2CLIENT_MenuMsgs[0];
		ExInput::UnregisterMsgs(pMsgs,7);
		ExInput::DefineBindings();
		*D2Vars::D2CLIENT_UI_Unk2 = 0;
		if(wasAutoMap) D2Vars::D2CLIENT_UIModes[UI_AUTOMAP] = 1;
	}
	else
	{
		if(ExParty::isOpen()) { ExParty::ShowHide(); return;}
		if(!D2Funcs::D2CLIENT_ClearScreen3(0,1)) {
		if(D2Vars::D2CLIENT_UIModes[UI_AUTOMAP]) wasAutoMap = true; else wasAutoMap = false;
		if(D2Vars::D2CLIENT_UIModes[UI_NPCMENU]) return;
		D2Funcs::D2CLIENT_ClearScreen();
		if(D2Funcs::D2CLIENT_GetPlayer()->dwMode == PLAYER_MODE_DEAD || D2Funcs::D2CLIENT_GetPlayer()->dwMode == PLAYER_MODE_DEATH) return;
		D2Vars::D2CLIENT_UIModes[UI_MAINMENU] = 1;
		ExInput::UndefineBindings();
		sMsg* pMsgs = &D2Vars::D2CLIENT_MenuMsgs[0];
		ExInput::RegisterMsgs(pMsgs,7);
		*D2Vars::D2CLIENT_UI_Unk2 = 1;
		*D2Vars::D2CLIENT_UI_Unk1 = 0;

		ExOptions::MainMenu(0,0);
		}
	}
}

void ExOptions::OnClick(StormMsg * Msg)
{
int nMenu = *D2Vars::D2CLIENT_SelectedMenu;
D2MenuEntry * Entries = *D2Vars::D2CLIENT_D2MenuEntries;

	if(Entries[nMenu].EnableCheck)
		if(!Entries[nMenu].EnableCheck(&Entries[nMenu],nMenu)) return;
	switch(Entries[nMenu].dwMenuType) {
	case D2MENU_SWITCH: 
		{
		Entries[nMenu].dwCurrentSwitch++;
		if(Entries[nMenu].dwCurrentSwitch> Entries[nMenu].dwSwitchesNo - 1)
			Entries[nMenu].dwCurrentSwitch = 0;
		if(Entries[nMenu].OnPress)
			Entries[nMenu].OnPress(&Entries[nMenu],Msg);
		D2Funcs::D2CLIENT_PlaySound(1);
		}
		break;
	case D2MENU_NORMAL:
		{
		if(Entries[nMenu].OnPress)
		Entries[nMenu].OnPress(&Entries[nMenu],Msg);
		D2Funcs::D2CLIENT_PlaySound(2);
		}
		break;
	case D2MENU_KEY:
		{
			if(MenuKeyClicked) {
				D2Funcs::STORM_UnregisterMsg(D2Funcs::D2GFX_GetHwnd(),WM_KEYDOWN,&m_OnGetKey);
				MenuKeyClicked =  0;
			}
			else	{
				D2Funcs::STORM_RegisterMsg(D2Funcs::D2GFX_GetHwnd(),WM_KEYDOWN,&m_OnGetKey);
				MenuKeyClicked = nMenu;
			}
		}
		break;
	}
}

void __stdcall ExOptions::m_OnGetKey(StormMsg * Msg)
{
	if(MenuKeyClicked && Msg->wParam != VK_ESCAPE) {
		D2MenuEntry * Entries = *D2Vars::D2CLIENT_D2MenuEntries;
	//	ExInput::UndefineBindings();
		*Entries[MenuKeyClicked].Bind = Msg->wParam;
//		ExInput::DefineBindings();
	}
	MenuKeyClicked = 0;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs::STORM_ResetMsgQuene(Msg);
	D2Funcs::STORM_UnregisterMsg(D2Funcs::D2GFX_GetHwnd(),WM_KEYDOWN,&m_OnGetKey);
}

void __stdcall ExOptions::m_LBUTTONDOWN(StormMsg * Msg)
{
	WORD mX = LOWORD(Msg->lParam);
	WORD mY = HIWORD(Msg->lParam);
	int nMenu = D2Funcs::D2CLIENT_GetSelectedMenu(mY);
	if ( nMenu != -1 )
		*D2Vars::D2CLIENT_SelectedMenu = nMenu;
	MenuUpdate(mX,mY);
	isMenuClicked = 1;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs::STORM_ResetMsgQuene(Msg);
}

void __stdcall  ExOptions::m_LBUTTONUP(StormMsg * Msg)
{
	WORD mY = HIWORD(Msg->lParam);
	if(isMenuClicked && D2Funcs::D2CLIENT_GetSelectedMenu(mY) == *D2Vars::D2CLIENT_SelectedMenu) OnClick(Msg);
	//CBA to check what exacly those vars are, but they were in the original code...
	*D2Vars::D2CLIENT_UI_Unk1 = 0;
	*D2Vars::D2CLIENT_UI_Unk2 = 0;
	*D2Vars::D2CLIENT_UI_Unk3 = 0;
	*D2Vars::D2CLIENT_UI_Unk4 = 0;
	*D2Vars::D2CLIENT_UI_Unk5 = 0;
	*D2Vars::D2CLIENT_UI_Unk6 = 0;
	*D2Vars::D2CLIENT_UI_Unk7 = 16;
	*D2Vars::D2CLIENT_UI_Unk8 = 16;

	isMenuClicked = 0;
	MenuValueClicked = 0;
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs::STORM_ResetMsgQuene(Msg);
}

void __stdcall  ExOptions::m_OnEnter(StormMsg * Msg) // 28.12 -> resolved
{
	OnClick(Msg);
	Msg->_2 = 1;
	Msg->_3 = 0;
	D2Funcs::STORM_ResetMsgQuene(Msg);
}

void ExOptions::MenuUpdate(int mX, int mY)
{
	int nMenu = *D2Vars::D2CLIENT_SelectedMenu;
	if(nMenu != D2Funcs::D2CLIENT_GetSelectedMenu(mY) && !MenuValueClicked) return;

	D2MenuEntry * Entries = *D2Vars::D2CLIENT_D2MenuEntries;

	int nValue = Entries[nMenu].dwCurrentValue;

	if(Entries[nMenu].EnableCheck)
		if(!Entries[nMenu].EnableCheck(&Entries[nMenu],nMenu)) return;

	switch(Entries[nMenu].dwMenuType) {
	case D2MENU_BAR: 
		{
			int CheckX, CheckX2;
			if(!Entries[nMenu].ptCellFile && !Entries[nMenu].wItemName[0]) {
				CheckX = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 144; 
				CheckX2 = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 145; 
			}
			else if(Entries[nMenu].ptCellFile)
			{
				CheckX = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 59; 
				CheckX2 = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 60; 
			}
			else
			{
				CheckX = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 59; 
				CheckX2 = (*D2Vars::D2CLIENT_ScreenWidth / 2) - 60; 
			}
			if(MenuValueClicked || mX> CheckX && mX < CheckX + 289) {
				if(mX >= CheckX2 + 12)	{
					if(mX <= CheckX2 + 277) {
						Entries[nMenu].dwCurrentValue = int(((mX - (CheckX +12)) / (132.5 / (Entries[nMenu].dwMaxValue - 1)) + 1) /2);
					}
					else
						Entries[nMenu].dwCurrentValue = Entries[nMenu].dwMaxValue - 1;
				}
				else
					Entries[nMenu].dwCurrentValue = 0;
				MenuValueClicked = 1;
			}
		}
	}
	if(nValue != Entries[nMenu].dwCurrentValue) {
		if(Entries[nMenu].OnPress)
			Entries[nMenu].OnPress(&Entries[nMenu],0);
		D2Funcs::D2CLIENT_PlaySound(1);
	}
}

void  ExOptions::DrawMenuBar(int BarPosX, int BarPosY, D2Menu * Menu, D2MenuEntry *Entry, int nTransLvl)
{
	D2Funcs::D2GFX_DrawRectangle(BarPosX,BarPosY,BarPosX+144,BarPosY+30,0,(Entry->dwBarType == 0) + 1);
	CellFile * cfBar = Entry->dwBarType ? *D2Vars::D2CLIENT_MenuBar2 : *D2Vars::D2CLIENT_MenuBar2;
	D2Funcs::D2WIN_DrawCellFile(cfBar,400+230,BarPosY,2,nTransLvl,-1);
	D2Funcs::D2WIN_DrawCellFile(*D2Vars::D2CLIENT_MenuBarSlider,400+250,BarPosY,2,nTransLvl,-1);
}



void ExOptions::DrawMenuRecon()
{
	ASSERT(*D2Vars::D2CLIENT_D2Menu);
	ASSERT(*D2Vars::D2CLIENT_D2MenuEntries);
	 
	D2Menu * Menu = *D2Vars::D2CLIENT_D2Menu;
	D2MenuEntry * Entries = *D2Vars::D2CLIENT_D2MenuEntries;
 
	static CellContext Pent = {0};
	Pent.pCellFile = *D2Vars::D2CLIENT_MenuPent;
	Pent.nCellNo = 0;
 
	static DWORD LastTick = 0; 
	static DWORD PentFrame = 0;
	D2Funcs::D2WIN_SetTextSize(6);
	ExScreen::DrawTextEx(2,10,5,0,5,"D2Ex2/Build %s.%d.%s.%s.%s,",
#ifdef D2EX_CLOSED_BNET
"PVP.BNET"
#else
"GENERIC"
#endif
,__BUILDNO__,__DATE__,__TIME__,
#ifdef _DEBUG
"Nightly"
#else
"Release"
#endif
);
	
	int MenuStartY = (*D2Vars::D2CLIENT_ScreenHeight - 80) /2 - (Menu->dwEntriesNo * Menu->dwInterline) / 2;

	if(*D2Vars::D2CLIENT_PrevMouseY != *D2Vars::D2CLIENT_MouseY || *D2Vars::D2CLIENT_PrevMouseX != *D2Vars::D2CLIENT_MouseX)
	{
		*D2Vars::D2CLIENT_PrevMouseY = *D2Vars::D2CLIENT_MouseY;
		*D2Vars::D2CLIENT_PrevMouseX = *D2Vars::D2CLIENT_MouseX;
		if(isMenuClicked) 
			MenuUpdate(*D2Vars::D2CLIENT_MouseX,*D2Vars::D2CLIENT_MouseY);
		//	D2Funcs::D2CLIENT_D2MenuChange(*D2Vars::D2CLIENT_MouseY,0,*D2Vars::D2CLIENT_MouseX);
		else
		{
			int SelMenu =	D2Funcs::D2CLIENT_GetSelectedMenu(*D2Vars::D2CLIENT_MouseY);
			if(SelMenu != -1) *D2Vars::D2CLIENT_SelectedMenu=SelMenu;
		}
	}

	for(int eNo = 0;(int)Menu->dwEntriesNo>eNo;eNo++)
	{
		Entries[eNo].dwYOffset = 0;
		int dwTrans =  0; 
		if( !Entries[eNo].dwExpansion || D2Funcs::FOG_isExpansion())
		{
			if(!Entries[eNo].EnableCheck || (dwTrans = Entries[eNo].EnableCheck(&Entries[eNo],eNo)) != 0 ) dwTrans = 1;
			dwTrans = (4* dwTrans) + 1;
			if(Entries[eNo].ChangeHandle) Entries[eNo].ChangeHandle(&Entries[eNo]);
			int MenuPosY  = MenuStartY + eNo * Menu->dwInterline;
			int MenuTextY  = MenuPosY + Menu->dwTextHeight;
			Entries[eNo].dwYOffset =  MenuPosY;
			switch(Entries[eNo].dwMenuType)
			{
			case D2MENU_STATIC: // Static entry
			case D2MENU_NORMAL: // Normal entry
				if(Entries[eNo].ptCellFile)
					D2Funcs::D2WIN_DrawCellFile(Entries[eNo].ptCellFile,*D2Vars::D2CLIENT_ScreenWidth / 2, MenuTextY, 1, dwTrans, -1);
				else if(Entries[eNo].wItemName[0])
				{
					D2Funcs::D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 3);
					int Tw = D2Funcs::D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs::D2WIN_DrawTextEx(Entries[eNo].wItemName,(*D2Vars::D2CLIENT_ScreenWidth - Tw) /2, MenuTextY,Entries[eNo].dwColor,0,dwTrans);
				}
				break;
			case D2MENU_SWITCH: // Switch Entry
				if(Entries[eNo].ptCellFile)
					D2Funcs::D2WIN_DrawCellFile(Entries[eNo].ptCellFile,*D2Vars::D2CLIENT_ScreenWidth / 2 - 230, MenuTextY, 0, dwTrans, -1);
				else if(Entries[eNo].wItemName[0])
				{
					D2Funcs::D2WIN_SetTextSize(2);
					int Tw = D2Funcs::D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs::D2WIN_DrawTextEx(Entries[eNo].wItemName,(*D2Vars::D2CLIENT_ScreenWidth) /2 - 250, MenuTextY,0,0,dwTrans);
				}
				if(Entries[eNo].ptSwitchCellFile[Entries[eNo].dwCurrentValue])
					D2Funcs::D2WIN_DrawCellFile(Entries[eNo].ptSwitchCellFile[Entries[eNo].dwCurrentValue],*D2Vars::D2CLIENT_ScreenWidth / 2 + 230, MenuTextY, 2, dwTrans, -1);
				else if(Entries[eNo].wItemName[0])
				{
					D2Funcs::D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 2);
					int Tw = D2Funcs::D2WIN_GetTextWidth(Entries[eNo].wSwitchItemName[Entries[eNo].dwCurrentValue]);
					D2Funcs::D2WIN_DrawTextEx(Entries[eNo].wSwitchItemName[Entries[eNo].dwCurrentValue],(*D2Vars::D2CLIENT_ScreenWidth - Tw) /2 + 230, MenuTextY,0,0,dwTrans);
				}
				break;
			case D2MENU_BAR: // Bar Entry
				if(Entries[eNo].ptCellFile)
				{
					D2Funcs::D2WIN_DrawCellFile(Entries[eNo].ptCellFile,*D2Vars::D2CLIENT_ScreenWidth / 2 - 230, MenuTextY, 0, dwTrans, -1);
					D2Funcs::D2CLIENT_D2DrawBar(MenuPosY,0,&Entries[eNo],dwTrans,Entries[eNo].ptCellFile == 0);
				}
				else if(Entries[eNo].wItemName[0])
				{
					D2Funcs::D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 7);
					int Tw = D2Funcs::D2WIN_GetTextWidth(Entries[eNo].wItemName);
					D2Funcs::D2WIN_DrawTextEx(Entries[eNo].wItemName,(*D2Vars::D2CLIENT_ScreenWidth - Tw) /2 - 230, MenuTextY,7,0,dwTrans);
					D2Funcs::D2CLIENT_D2DrawBar(MenuPosY+ 7,0,&Entries[eNo],dwTrans,0);
				}
				break;
			case D2MENU_KEY: // Key Config
				if(Entries[eNo].wItemName[0])
				{
					D2Funcs::D2WIN_SetTextSize(Entries[eNo].dwFontType ? Entries[eNo].dwFontType : 8);
					D2Funcs::D2WIN_DrawTextEx(Entries[eNo].wItemName, 130, MenuTextY,COL_ORANGE,0,dwTrans);
					if(Entries[eNo].Bind) {
					wchar_t* szKey = ExInput::GetNameOfKey((WORD)*Entries[eNo].Bind);
					int Tw2 = D2Funcs::D2WIN_GetTextWidth(szKey);
					D2Funcs::D2WIN_DrawTextEx(szKey,(*D2Vars::D2CLIENT_ScreenWidth - Tw2) /2 + 230, MenuTextY,MenuKeyClicked == eNo ? COL_RED : COL_WHITE ,0,dwTrans);
					}
				}
				break;
			}
		}
	}
	if(PentFrame == 0) Pent.nCellNo = 0; //*D2Vars::D2CLIENT_WidestMenu
	else Pent.nCellNo = 8 - PentFrame; 
	int PentY = Menu->dwMenuOffset + Entries[*D2Vars::D2CLIENT_SelectedMenu].dwYOffset;
	D2Funcs::D2GFX_DrawCellContext(&Pent,(int)(*D2Vars::D2CLIENT_ScreenWidth * 0.9), PentY, -1 , 5 , 0);
	Pent.nCellNo = PentFrame;
	D2Funcs::D2GFX_DrawCellContext(&Pent,(int)(*D2Vars::D2CLIENT_ScreenWidth * 0.02), PentY, -1 , 5 , 0);

	int Tick = GetTickCount();
	if(Tick - LastTick > 50)
	{
	LastTick = Tick;
	PentFrame  = (PentFrame + 1) % 8;
	}

}

BOOL __fastcall ExOptions::GiveUpCheck(D2MenuEntry* ptEntry, DWORD ItemNo)
{
Room1* aRoom =D2Funcs::D2COMMON_GetUnitRoom(D2Funcs::D2CLIENT_GetPlayer());
int aLvl = D2Funcs::D2COMMON_GetLevelNoByRoom(aRoom);
int aAct = D2Funcs::D2COMMON_GetActNoByLevelNo(aLvl);
if(aLvl!=D2Funcs::D2COMMON_GetTownLevel(aAct))
return true;
return false;
}

BOOL __fastcall ExOptions::GiveUpCB(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	const char szBuffer[] = "#gu";
	BYTE * aPacket = new BYTE[strlen(szBuffer) + 6];
	memset(aPacket, 0, strlen(szBuffer) + 6);
	aPacket[0] = 0x15;
	*(WORD*)&aPacket[1] = 1;
	memcpy(aPacket + 3, szBuffer, strlen(szBuffer));
	D2Funcs::D2NET_SendPacket(strlen(szBuffer) + 6,1,aPacket);
	delete[] aPacket;
D2Funcs::D2CLIENT_ClearScreen();
return true;
}

//BOOL __fastcall ExOptions::ChangeHandle(D2MenuEntry* ptEntry)
//{
////wstring tmp = L"You set "+ boost::lexical_cast<wstring>(ptEntry->dwCurrentValue);
////D2Funcs::D2CLIENT_PrintGameString(tmp.c_str(),1);
//wcscpy_s((wchar_t*)ptEntry->szCellFile,130,boost::lexical_cast<wstring>(ptEntry->dwCurrentValue).c_str());
//switch(*D2Vars::D2CLIENT_SelectedMenu)
//{
//case 0:
//BOLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","BOLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 1:
//BCLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","BCLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 2:
//ShoutLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","ShoutLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 3:
//EnchLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","EnchLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 4:
//AmpLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","AmpLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 5:
//LRLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","LRLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//case 6:
//SMLvl=ptEntry->dwCurrentValue;
//WritePrivateProfileString("D2Ex","SMLvl",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),FileName.c_str());
//break;
//}
//return false;
//}
//
//
//BOOL __fastcall ExOptions::BuffECheck(D2MenuEntry* ptEntry, DWORD ItemNo)
//{
//int SkillId = 1;
//	switch(ItemNo)
//	{
//	case 0:
//	SkillId=0x95;
//	break;
//	case 1:
//	SkillId=0x9B;
//	break;
//	case 2:
//	SkillId=0x8A;
//	break;
//	case 3:
//	SkillId=0x34;
//	break;
//	case 4:
//	SkillId=0x42;
//	break;
//	case 5:
//	SkillId=0x5B;
//	break;
//	case 6:
//	SkillId=0x11;
//	break;
//	}
//	if(!*D2Vars::D2CLIENT_isMenuClick && *D2Vars::D2CLIENT_SelectedMenu==ItemNo) {
//	wcscpy_s((wchar_t*)ptEntry->szCellFile,130,ExBuffs::GetSkillName(SkillId));
//	}
//
//return true;
//}
//
//BOOL __fastcall ExOptions::Buffs(D2MenuEntry* ptEntry)
//{
//static D2Menu NewMenu;
//static D2MenuEntry NewEntries[8];
//
//if(!NewMenu.dwEntriesNo)
//{
//NewMenu.dwEntriesNo=8;
//NewMenu.dwInterline=45;
//NewMenu.dwTextHeight=35;
//NewMenu.dwMenuOffset=51;
//NewMenu.dwBarHeight=36;
//}
//
//	wcscpy_s((wchar_t*)&NewEntries[0].szCellFile,130,ExBuffs::GetSkillName(0x95));
//	wcscpy_s((wchar_t*)&NewEntries[1].szCellFile,130,ExBuffs::GetSkillName(0x9B));
//	wcscpy_s((wchar_t*)&NewEntries[2].szCellFile,130,ExBuffs::GetSkillName(0x8A));
//	wcscpy_s((wchar_t*)&NewEntries[3].szCellFile,130,ExBuffs::GetSkillName(0x34));
//	wcscpy_s((wchar_t*)&NewEntries[4].szCellFile,130,ExBuffs::GetSkillName(0x42));
//	wcscpy_s((wchar_t*)&NewEntries[5].szCellFile,130,ExBuffs::GetSkillName(0x5B));
//	wcscpy_s((wchar_t*)&NewEntries[6].szCellFile,130,ExBuffs::GetSkillName(0x11));
//	wcscpy_s((wchar_t*)&NewEntries[7].szCellFile,130,D2Funcs::D2LANG_GetLocaleText(3409));
//
//	NewEntries[7].OnPress=&ExOptions::Options;
//	NewEntries[0].EnableCheck=NewEntries[1].EnableCheck=NewEntries[2].EnableCheck=NewEntries[3].EnableCheck=NewEntries[4].EnableCheck=NewEntries[5].EnableCheck=NewEntries[6].EnableCheck=&ExOptions::BuffECheck;
//	NewEntries[0].dwMenuType=NewEntries[1].dwMenuType=NewEntries[2].dwMenuType=NewEntries[3].dwMenuType=NewEntries[4].dwMenuType=NewEntries[5].dwMenuType=NewEntries[6].dwMenuType=2;
//	NewEntries[0].OnPress=NewEntries[1].OnPress=NewEntries[2].OnPress=NewEntries[3].OnPress=NewEntries[4].OnPress=NewEntries[5].OnPress=NewEntries[6].OnPress=&ExOptions::ChangeHandle;
//	NewEntries[0].dwCurrentValue = BOLvl;
//	NewEntries[0].dwMaxValue=55;
//	NewEntries[1].dwCurrentValue = BCLvl;
//	NewEntries[1].dwMaxValue=55;
//	NewEntries[2].dwCurrentValue = ShoutLvl;
//	NewEntries[2].dwMaxValue=55;
//	NewEntries[3].dwCurrentValue = EnchLvl;
//	NewEntries[3].dwMaxValue=55;
//	NewEntries[4].dwCurrentValue = AmpLvl;
//	NewEntries[4].dwMaxValue=55;
//	NewEntries[5].dwCurrentValue = LRLvl;
//	NewEntries[5].dwMaxValue=55;
//	NewEntries[6].dwCurrentValue = SMLvl;
//	NewEntries[6].dwMaxValue=55;
//
//
//*D2Vars::D2CLIENT_SelectedMenu=0;
//*D2Vars::D2CLIENT_D2Menu=&NewMenu;
//*D2Vars::D2CLIENT_D2MenuEntries=&NewEntries[0];
//return true;
//}

BOOL __fastcall ExOptions::AutoMapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
BlobType=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","BlobType",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}


BOOL __fastcall ExOptions::PermLifeOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
PermShowLife=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","PermShowLife",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::PermManaOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
PermShowMana=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","PermShowMana",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::VisOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
FullVisibility=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","FullVisibility",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::AutoOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
AutoShowMap=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","AutoShowMap",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::MapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
Maphack=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","Maphack",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::LagOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
bLagometer=ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","Lagometer",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::GoldOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
HideGold = ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","HideGold",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::CrapOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
HideCrap =ptEntry->dwCurrentSwitch;
WritePrivateProfileString("D2Ex","HideCrap",boost::lexical_cast<string>(ptEntry->dwCurrentSwitch).c_str(),ConfigIni.c_str());

return TRUE;
}

BOOL __fastcall ExOptions::EnteringOpt(D2MenuEntry* ptEntry, StormMsg* pMsg)
{

ptEntry->dwBarType=ptEntry->dwCurrentValue;
EnteringFont = ptEntry->dwCurrentValue;
WritePrivateProfileString("D2Ex","EnteringFont",boost::lexical_cast<string>(ptEntry->dwCurrentValue).c_str(),ConfigIni.c_str());
return false;
}

BOOL __fastcall ExOptions::EntECheck(D2MenuEntry* ptEntry, DWORD ItemNo)
{
	if(!*D2Vars::D2CLIENT_isMenuClick && *D2Vars::D2CLIENT_SelectedMenu==ItemNo) 
	ptEntry->dwBarType=0;

return true;
}

BOOL __fastcall ExOptions::Various(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
static D2Menu NewMenu;
static D2MenuEntry NewEntries[11];

if(!NewMenu.dwEntriesNo)
{
::memset(&NewEntries,0,sizeof(NewEntries));
NewMenu.dwEntriesNo=11;
NewMenu.dwInterline=40;
NewMenu.dwTextHeight=35;
NewMenu.dwMenuOffset=46;
NewMenu.dwBarHeight=32;

NewEntries[0].dwMenuType=1;
NewEntries[0].dwSwitchesNo=3;
NewEntries[0].OnPress=&ExOptions::AutoMapOpt;
NewEntries[0].dwCurrentSwitch=BlobType;

NewEntries[1].dwMenuType=2;
NewEntries[1].dwMaxValue=13;
NewEntries[1].dwCurrentValue=EnteringFont;
NewEntries[1].OnPress=&ExOptions::EnteringOpt;
NewEntries[1].EnableCheck=&ExOptions::EntECheck;

NewEntries[2].dwMenuType=NewEntries[3].dwMenuType=NewEntries[4].dwMenuType= NewEntries[5].dwMenuType= NewEntries[6].dwMenuType=NewEntries[7].dwMenuType=NewEntries[8].dwMenuType=NewEntries[9].dwMenuType=1;
NewEntries[2].dwSwitchesNo=NewEntries[3].dwSwitchesNo=NewEntries[4].dwSwitchesNo=NewEntries[5].dwSwitchesNo=NewEntries[6].dwSwitchesNo=NewEntries[7].dwSwitchesNo=NewEntries[8].dwSwitchesNo=NewEntries[9].dwSwitchesNo=2;

NewEntries[2].dwCurrentSwitch = PermShowLife;
NewEntries[2].OnPress=&ExOptions::PermLifeOpt;

NewEntries[3].dwCurrentSwitch = PermShowMana;
NewEntries[3].OnPress=&ExOptions::PermManaOpt;

NewEntries[4].dwCurrentSwitch = FullVisibility;
NewEntries[4].OnPress=&ExOptions::VisOpt;

NewEntries[5].dwCurrentSwitch = AutoShowMap;
NewEntries[5].OnPress=&ExOptions::AutoOpt;

NewEntries[6].dwCurrentSwitch = Maphack;
NewEntries[6].OnPress=&ExOptions::MapOpt;

NewEntries[7].dwCurrentSwitch = HideGold;
NewEntries[7].OnPress=&ExOptions::GoldOpt;

NewEntries[8].dwCurrentSwitch = HideCrap;
NewEntries[8].OnPress=&ExOptions::CrapOpt;

NewEntries[9].dwCurrentSwitch = bLagometer;
NewEntries[9].OnPress=&ExOptions::LagOpt;

NewEntries[10].OnPress=&ExOptions::Options;

int LocId = D2Funcs::D2LANG_GetLocaleId();

wcscpy_s((wchar_t*)&NewEntries[0].szCellFile,130,LocId == 10 ? L"WSKA�NIK AUTOMAPY": L"AUTOMAP BLOB");
wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[0],130,LocId == 10 ? L"KROPKA" : L"DOT");
wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[1],130,LocId == 10 ? L"KӣKO" : L"CIRCLE");
wcscpy_s((wchar_t*)&NewEntries[0].szSwitchCellFiles[2],130,LocId == 10 ? L"KRZY�YK" : L"CROSS");
	
wcscpy_s((wchar_t*)&NewEntries[1].szCellFile,130,LocId == 10 ? L"CZCIONKA NAZWY POZIOMU" : L"ENTERING LVL TEXT FONT");
wcscpy_s((wchar_t*)&NewEntries[2].szCellFile,130,LocId == 10 ? L"ZAWSZE POKAZUJ �YCIE" : L"ALWAYS SHOW LIFE");
wcscpy_s((wchar_t*)&NewEntries[2].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[2].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[3].szCellFile,130,LocId == 10 ? L"ZAWSZE POKAZUJ MANE" : L"ALWAYS SHOW MANA");
wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[3].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[4].szCellFile,130,LocId == 10 ? L"PE�NA WIDOCZNO��" : L"FULL VISIBILITY");
wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[4].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[5].szCellFile,130,LocId == 10 ? L"AUTOM. W��CZ MAP�" : L"AUTO. TURN ON THE MAP");
wcscpy_s((wchar_t*)&NewEntries[5].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[5].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[6].szCellFile,130, L"MAPHACK");
wcscpy_s((wchar_t*)&NewEntries[6].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[6].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[7].szCellFile,130,LocId == 10 ? L"UKRYJ Z�OTO NA ZIEMI" : L"HIDE GOLD ON THE GROUND");
wcscpy_s((wchar_t*)&NewEntries[7].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[7].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[8].szCellFile,130,LocId == 10 ? L"UKRYJ �MIECI NA ZIEMI" : L"HIDE CRAP ON THE GROUND");
wcscpy_s((wchar_t*)&NewEntries[8].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[8].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");
wcscpy_s((wchar_t*)&NewEntries[9].szCellFile,130,LocId == 10 ? L"LAGOMETR" : L"LAGOMETER");
wcscpy_s((wchar_t*)&NewEntries[9].szSwitchCellFiles[0],130,LocId == 10 ? L"WY�." : L"OFF");
wcscpy_s((wchar_t*)&NewEntries[9].szSwitchCellFiles[1],130,LocId == 10 ? L"W�." : L"ON");


wcscpy_s((wchar_t*)&NewEntries[10].szCellFile,130,LocId == 10 ? L"POPRZEDNIE MENU" : L"PREVIOUS MENU");

}

*D2Vars::D2CLIENT_SelectedMenu=0;
*D2Vars::D2CLIENT_D2Menu=&NewMenu;
*D2Vars::D2CLIENT_D2MenuEntries=&NewEntries[0];

return TRUE;
}

// Import from Scrap
BOOL __fastcall ExOptions::KeyConfig(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static int LocId = D2Funcs::D2LANG_GetLocaleId();
	static D2Menu NewMenu = {6, 18, 20, 37, 0, 0};
	static D2MenuEntry NewEntries[6] = {0};

	if(!NewEntries[0].wItemName[0]) {
		NewEntries[0].dwMenuType = D2MENU_STATIC;
		NewEntries[1].dwMenuType = NewEntries[2].dwMenuType = NewEntries[3].dwMenuType = NewEntries[4].dwMenuType = NewEntries[5].dwMenuType = D2MENU_KEY;
	//	NewEntries[1].Bind = (DWORD*)&VK_Aim;
	//	NewEntries[2].Bind = (DWORD*)&VK_Blind;
	//	NewEntries[3].Bind = (DWORD*)&VK_Tease;
	//	NewEntries[4].Bind = (DWORD*)&VK_FastTP;
	//	NewEntries[5].Bind = (DWORD*)&VK_AntiJoder;

		wcscpy_s(NewEntries[0].wItemName,130,LocId == LOCALE_POL ? L"KONFIG. KLAWISZY" : L"CONFIGURE CONTROLS");
		wcscpy_s(NewEntries[1].wItemName,130,LocId == LOCALE_POL ? L"BEZPO�R. AA" : L"DIRECT AA");
		wcscpy_s(NewEntries[2].wItemName,130,LocId == LOCALE_POL ? L"�LEPY PKT. AA" : L"BLINDSPOT AA");
		wcscpy_s(NewEntries[3].wItemName,130,LocId == LOCALE_POL ? L"WKURZACZ" : L"TEASE ATTACK");
		wcscpy_s(NewEntries[4].wItemName,130,LocId == LOCALE_POL ? L"SZYBKIE TP" : L"FAST TP");
		wcscpy_s(NewEntries[5].wItemName,130,L"ANTI-JODER");

	}

	*D2Vars::D2CLIENT_SelectedMenu=0;
	*D2Vars::D2CLIENT_D2Menu=&NewMenu;
	*D2Vars::D2CLIENT_D2MenuEntries=&NewEntries[0];
	return true;

}

BOOL __fastcall ExOptions::Options(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
static D2Menu NewMenu;
static D2MenuEntry NewEntries[6];


if(!NewMenu.dwEntriesNo)
{
	::memset(&NewMenu,0,sizeof(NewMenu));
	::memset(&NewEntries,0,sizeof(NewEntries));

NewMenu.dwEntriesNo=6;
NewMenu.dwInterline=48;
NewMenu.dwTextHeight=45;
NewMenu.dwMenuOffset=51;

D2Vars::D2CLIENT_SndOptionsMenu[7].OnPress=&ExOptions::Options;
D2Vars::D2CLIENT_VidOptionsMenu[7].OnPress=&ExOptions::Options;
D2Vars::D2CLIENT_MapOptionsMenu[6].OnPress=&ExOptions::Options;


int LocId = D2Funcs::D2LANG_GetLocaleId();

memcpy(NewEntries,(const void*)*&D2Vars::D2CLIENT_OptionsMenu,sizeof(D2MenuEntry)*4);
memcpy(&NewEntries[5],(const void*)&D2Vars::D2CLIENT_OptionsMenu[4],sizeof(D2MenuEntry));

wcscpy_s((wchar_t*)&NewEntries[0].szCellFile,130,LocId == 10 ? L"OPCJE D�WI�KOWE" : L"SOUND OPTIONS");
wcscpy_s((wchar_t*)&NewEntries[1].szCellFile,130,LocId == 10 ? L"OPCJE GRAFICZNE" : L"VIDEO OPTIONS");
wcscpy_s((wchar_t*)&NewEntries[2].szCellFile,130,LocId == 10 ? L"OPCJE AUTOMAPY" : L"AUTOMAP OPTIONS");
wcscpy_s((wchar_t*)&NewEntries[3].szCellFile,130,LocId == 10 ? L"KONFIGURACJA STEROWANIA" : L"CONFIGURE CONTROLS");
wcscpy_s((wchar_t*)&NewEntries[4].szCellFile,130,LocId == 10 ? L"USTAWIENIA D2EX" : L"D2EX SETTINGS");
wcscpy_s((wchar_t*)&NewEntries[5].szCellFile,130,LocId == 10 ? L"POPRZEDNIE MENU" : L"PREVIOUS MENU");

NewEntries[0].ptCellFile=NewEntries[1].ptCellFile=NewEntries[2].ptCellFile=NewEntries[3].ptCellFile=NewEntries[4].ptCellFile=NewEntries[5].ptCellFile=0;
NewEntries[5].OnPress=&ExOptions::MainMenu;

NewEntries[4].dwCurrentValue=COL_ORANGE;
NewEntries[4].OnPress=&ExOptions::Various;
}

*D2Vars::D2CLIENT_SelectedMenu=0;
*D2Vars::D2CLIENT_D2Menu=&NewMenu;
*D2Vars::D2CLIENT_D2MenuEntries=&NewEntries[0];

return TRUE;
}

BOOL __fastcall ExOptions::MainMenu(D2MenuEntry* ptEntry, StormMsg* pMsg)
{
	static D2Menu NewMenu;
	static D2MenuEntry NewEntries[4];

if(!NewMenu.dwEntriesNo)
{
::memset(&NewMenu,0,sizeof(NewMenu));
::memset(&NewEntries,0,sizeof(NewEntries));
NewMenu.dwEntriesNo=4;
NewMenu.dwInterline=50;
NewMenu.dwTextHeight=45;
NewMenu.dwMenuOffset=51;


memcpy(NewEntries,(const void*)*&D2Vars::D2CLIENT_OldMenu,sizeof(D2MenuEntry));
memcpy(&NewEntries[2],(const void*)&D2Vars::D2CLIENT_OldMenu[1],sizeof(D2MenuEntry)*2);

NewEntries[0].OnPress=&ExOptions::Options;
NewEntries[1].OnPress=&ExOptions::GiveUpCB;
NewEntries[1].EnableCheck=&ExOptions::GiveUpCheck;
NewEntries[0].ptCellFile=NewEntries[2].ptCellFile=NewEntries[3].ptCellFile=0;

int LocId = D2Funcs::D2LANG_GetLocaleId();

wcscpy_s((wchar_t*)&NewEntries[0].szCellFile,130,LocId == 10 ? L"OPCJE" : L"OPTIONS");
wcscpy_s((wchar_t*)&NewEntries[1].szCellFile,130,LocId == 10 ? L"PODDAJ SI�" : L"GIVE UP");
wcscpy_s((wchar_t*)&NewEntries[2].szCellFile,130,LocId == 10 ? L"ZAPIS I WYJ�CIE Z GRY" : L"SAVE AND EXIT GAME");
wcscpy_s((wchar_t*)&NewEntries[3].szCellFile,130,LocId == 10 ? L"POWR�T DO GRY" : L"RETURN TO GAME");
}

*D2Vars::D2CLIENT_SelectedMenu=1;
*D2Vars::D2CLIENT_D2Menu=&NewMenu;
*D2Vars::D2CLIENT_D2MenuEntries=&NewEntries[0];

return true;
}
