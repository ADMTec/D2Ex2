#ifndef _EXINPUT_H__
#define _EXINPUT_H__

#include <string>
#include <sstream>

#include "Vars.h"
#include "Misc.h"
#include "ExEditBox.h"
#include "ExParty.h"
#include "ExOOG.h"
#include "ExFriends.h"

using namespace std;

namespace ExInput
{
void RegisterMsgs(sMsg* pMsgList, int nCount);
void UnregisterMsgs(sMsg* pMsgList, int nCount);

DWORD __fastcall GameInput(wchar_t* wMsg);
void __stdcall RealmInput(wchar_t* Text, int Color);
DWORD __fastcall PacketInput(BYTE* aPacket, int aLen);

void GameInput_STUB();
void PacketInput_STUB();
LONG WINAPI GameWindowEvent(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void Say(const char* Msg,...);
}




#endif