// R6ExternalSource.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Globals.h"


#include <iostream>
#include <thread>
#include <stdlib.h> //sleep

using namespace std;
//Defining our globals
namespace GameGlobal {


	ULONG64 BaseAddress = 0;
	HANDLE GameHandle = 0x0;
	BYTE Old[] = { 0x48 ,0xff ,0x25 ,0x39 ,0x97 ,0x05 ,0x00 };
	BYTE Close[] = { 0xC3, 0x90, 0x90, 0x90,0x90,0x90,0x90 };
	ULONG64 GameRet=0x0;

	ULONG oldsize = sizeof(GameGlobal::Old);

	std::string LocalName = "";
	LH_Rd Rd;
	LH_Mem Mem;
}

namespace aa {
	
}

class cc {

};

struct bb {
	
};
void aimthread() {
	while (true)
	{
		Sleep(10);
		if(BestEntity != NULL ){
			//GameGlobal::Mem.set_viewAngle(LocalEntity, LocalEntity2
			//, GameGlobal::Mem.calculate_angle(GameGlobal::Mem.GetViewTranslation(), GameGlobal::Mem.GetEntityHeadPosition(BestEntity)), true);
			//cout << "work " << endl;
			
		}
		
	}
}

void t1()  //普通的函式，用來執行執行緒
{

	thread t(&aimthread);
	GameGlobal::Rd.Loop();
	while (true)
	{

		//cout << "a 0x:" << endl;
	//	Draw.Loop();
	}



}

int main()
{

	//You will need to set the actual handle to the game here from your bypass
	/*if (GameGlobal::GameHandle == INVALID_HANDLE_VALUE || GameGlobal::GameHandle == NULL || GameGlobal::GameHandle == (HANDLE)0x0) {
		std::cout << "Error 找不到遊戲!!" << std::endl;
		system("pause");
		return 1;
	}*/

	dr = new Lh_Dr();
	GameGlobal::Rd = LH_Rd();
	GameGlobal::Mem = LH_Mem();


	if (dr == NULL) {


		system("pause");
		return 1;
	}
	while (true) {
		if (GameGlobal::LocalName == "") {
			std::cout << VMProtectDecryptStringA("Entry Your Game Name") << std::endl;
			std::string EntryName = "";
			std::cin >> EntryName;
			 if(EntryName == "A"){
				GameGlobal::LocalName = "xx0911216";
			}
			else if (EntryName == "B") {
				GameGlobal::LocalName = "AerryBoB";
			}
			else if (EntryName == "C") {
				 GameGlobal::LocalName = "PafEgN";
			 }
			else {
				std::cout << VMProtectDecryptStringA("Error! User: ") << EntryName << std::endl;
				continue;
			}
				
			std::cout << VMProtectDecryptStringA("Welcome ") << GameGlobal::LocalName << std::endl;
			break;
		}
		else {
			continue;
		}
	}
	GameGlobal::BaseAddress = dr->GBA();
	std::cout << VMProtectDecryptStringA("入口: ") << std::hex << GameGlobal::BaseAddress << std::dec << std::endl;


	GameGlobal::Rd.CreateExEWindow();
	std::cout << VMProtectDecryptStringA("完成!") << std::endl;

	
	//ret
	GameGlobal::GameRet =
		(ULONG64)GetProcAddress(GetModuleHandleA("KERNEL32.dll")
			, VMProtectDecryptStringA("TerminateProcess"));//KERNEL32 kernelbase,   GetModuleHandleA LoadLibrary

	printf(VMProtectDecryptStringA("Hook遊戲保護機制地址: %X"),
		GameGlobal::GameRet);
	
    if(GameGlobal::GameRet){
		
	


		//BYTE Close[] = { 0xC2, 0x08, 0x00 };
		
		//BYTE Close[] = { 0xC3, 0x90, 0x90,0x90 };
	//	for (int idx = 0; idx < sizeof(Close); idx++)
			//dr->WPM< BYTE>(GameRet + idx * 0x1, Close[idx]);
		//for (int idx = 0; idx < sizeof(Old); idx++)
			//printf("A: %X ", Old[idx]);
	
		// 48 ff 25 39 97 05 00 jmp qword ptr [7FFDA98A8520]+

		// 48 ff 25 89 91 05 00 jmp qword ptr [7FFDA98A7F80] LocalName

		//dr->WBT(GameGlobal::GameRet, GameGlobal::Close, sizeof(GameGlobal::Close));
	    std::cout << VMProtectDecryptStringA("\nHook保護失敗  無後座無法啟用\n") << std::endl;
		//system("pause");
	}
	else {
		std::cout << VMProtectDecryptStringA("Hook保護失敗") << std::endl;
		system("pause");
		return EXIT_SUCCESS;
	}

	//Start the main loop
	t1();
	
	
	//bot

	system("pause");
	return EXIT_SUCCESS;

	return 0;
}


