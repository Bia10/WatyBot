#include "ChangeChannel.h"
#include "Packet.h"
#include <Windows.h>
#include "Memory.h"
#include <vcclr.h>
using namespace ChangeChannel;
using namespace System;
extern gcroot<Packets::CPackets^> CPacket;

typedef void (__stdcall* PFN_CField_SendTransferChannelRequest)(unsigned char nChannel);
auto CField_SendTransferChannelRequest = reinterpret_cast<PFN_CField_SendTransferChannelRequest>(CCAddy);

CChangeChannel::CChangeChannel()
{
	bw = gcnew System::ComponentModel::BackgroundWorker;
}

void CChangeChannel::CCSwitch(CCType type)
{
	if(Busy) return;
	String^ strError = String::Empty;
	switch(type)
	{
	case CCType::CC:
		bw->DoWork += gcnew ComponentModel::DoWorkEventHandler(this, &CChangeChannel::CC);
		bw->RunWorkerAsync();
		break;

	case CCType::CS:
		bw->DoWork += gcnew ComponentModel::DoWorkEventHandler(this, &CChangeChannel::CS);
		bw->RunWorkerAsync();
		break;

	case CCType::DC:
		bw->DoWork += gcnew ComponentModel::DoWorkEventHandler(this, &CChangeChannel::DC);
		bw->RunWorkerAsync();
		break;
	}
}

void CChangeChannel::CC(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	if(!InGame())
	{
		e->Cancel;
		return;
	}
	
	//Store the currrent Channel
	StartChannel = getChannel();

	//While loop checking if you actually did CC
	while(getChannel() == StartChannel)
	{
		//Generate a random int to CC to + check if it is a different channel
		do
		{
			srand (GetCurrentTime());
			TargetChannel = rand()%14;
		}		
		while(TargetChannel == StartChannel);

		//Sleep while breath > 0
		while(getBreathValue() > 0) Sleep(100);

		//Send the CC request
		CField_SendTransferChannelRequest(TargetChannel);
		
		//Sleep 3 seconds to make sure the client is ingame again
		Sleep(3000);
	}
}

void CChangeChannel::CS(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	if(!InGame()) e->Cancel;
	while(getBreathValue() != 0) Sleep(100);
	Sleep(500);
	String^ strError = String::Empty;
	CPacket->Send(EnterCashShop, strError);
	Sleep(3000);
}

void CChangeChannel::DC(System::Object^  sender, System::ComponentModel::DoWorkEventArgs^  e)
{
	while(InGame())
	{
		String^ strError = String::Empty;
		CPacket->Send(ChangeCharacter, strError);
		Sleep(1000);
	}
}

/*
enum class CCReturns {Succes, Breath, Death};
CCReturns CCReturn;
#define CCSuccesAddy 0x005688ED
#define CCBreathAddy 0x00568955
#define CCDeathAddy 0x005689CF

VOID WINAPI SetCCSucces(){ CCReturn = CCReturns::Succes; ShowInfo("Succes!"); };
CodeCave(CCHookSucces)
{
	call SetCCSucces
	add esp,0x20
	ret 0004
}
EndCodeCave
CMemory cmCCHookSucces(CCSuccesAddy - 3, CaveCCHookSucces, 1, true);

VOID WINAPI SetCCBreath(){ CCReturn = CCReturns::Breath; ShowInfo("No Breath!"); };
CodeCave(CCHookBreath)
{
	call SetCCBreath
	add esp,0x20
	ret 0004
}
EndCodeCave
CMemory cmCCHookBreath(CCBreathAddy - 3, CaveCCHookBreath, 1, true);

VOID WINAPI SetCCDeath(){ CCReturn = CCReturns::Death; ShowInfo("You're dead...."); };
CodeCave(CCHookDeath)
{
	call SetCCDeath
	add esp,0x20
	ret 0004
}
EndCodeCave
CMemory cmCCHookDeath(CCDeathAddy - 3, CaveCCHookDeath, 1, true);
*/
