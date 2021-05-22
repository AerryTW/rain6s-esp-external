
#include <iostream>
#include "Globals.h"

#include <thread>
void little_sleep(std::chrono::microseconds us)
{
	auto start = std::chrono::high_resolution_clock::now();
	auto end = start + us;
	do {
		std::this_thread::yield();
	} while (std::chrono::high_resolution_clock::now() < end);
}

LH_Rd::LH_Rd() {};







WPARAM LH_Rd::Loop() {
	VMProtectBegin("Loop");
	MSG message;
	RECT rect;

	while (TRUE) {
		ZeroMemory(&message, sizeof(MSG));

		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (message.message == WM_QUIT) {
			exit(0);
		}

		GWindow = NULL;
		GWindow = FindWindow(NULL, VMProtectDecryptStringW(L"Rainbow Six"));

		HWND TopWindow = GetForegroundWindow();
		HWND RTopWindow = GetWindow(TopWindow, GW_HWNDPREV);


		if (!GWindow) {
			std::cout << VMProtectDecryptStringA("Error 找不到遊戲!") << std::endl;
			ExitProcess(0);
		}

		ZeroMemory(&rect, sizeof(RECT));

		GetWindowRect(GWindow, &rect);
		ScreenWidth = rect.right - rect.left;
		ScreenHeight = rect.bottom - rect.top;

		MoveWindow(ExEWindow, rect.left, rect.top, ScreenWidth, ScreenHeight, true);



		SetWindowPos(ExEWindow, RTopWindow, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		UpdateWindow(ExEWindow);

		Render();

		Sleep(1);

	}
	return message.wParam;

	VMProtectEnd();
}






//所有變態功能
/*
void read_data(WinProcess& proc, R6Data& data, bool init = true)
{
	if (init)
	{
		data.base = get_base(proc);
	}

	auto localplayer = dr->RPM<uint64_t>(data.base + PROFILE_MANAGER_OFFSET);
	localplayer = dr->RPM<uint64_t>(localplayer + 0x68);
	localplayer = dr->RPM<uint64_t>(localplayer + 0x0);
	localplayer = proc.Read<uint64_t>(localplayer + 0x28) + 0xebab0991057478ed;
	data.local_player = localplayer;

	auto fov_manager = proc.Read<uint64_t>(data.base + FOV_MANAGER_OFFSET);
	fov_manager = proc.Read<uint64_t>(fov_manager + 0xE8);
	fov_manager = proc.Read<uint64_t>(fov_manager + 0x88B932A0D99755B8);
	data.fov_manager = fov_manager;

	auto weapon = proc.Read<uint64_t>(localplayer + 0x90);
	weapon = proc.Read<uint64_t>(weapon + 0xc8);
	data.curr_weapon = weapon;

	auto weapon2 = proc.Read<uint64_t>(data.curr_weapon + 0x290) - 0x2b306cb952f73b96;
	data.weapon_info = weapon2;

	data.glow_manager = proc.Read<uint64_t>(data.base + GLOW_MANAGER_OFFSET);

	data.round_manager = proc.Read<uint64_t>(data.base + ROUND_MANAGER_OFFSET);

	data.game_manager = proc.Read<uint64_t>(data.base + GAME_MANAGER_OFFSET);
}

void enable_esp(WinProcess& proc, const R6Data& data)
{
	if (!USE_CAV_ESP) return;
	bool updated = false;

	if (data.game_manager == 0) return;
	auto entity_list = proc.Read<uint64_t>(data.game_manager + 0x98) + 0xE60F6CF8784B5E96;
	if (entity_list == 0) return;
	for (int i = 0; i < 11; ++i)
	{
		auto entity_address = proc.Read<uint64_t>(entity_list + (0x8 * i)); //entity_object
		auto buffer = proc.Read<uint64_t>(entity_address + 0x18);
		auto size = proc.Read<uint32_t>(buffer + 0xE0) & 0x3FFFFFFF;
		auto list_address = proc.Read<uint64_t>(buffer + 0xD8);
		for (uint32_t i = 0; i < size; ++i)
		{
			auto pbuffer = proc.Read<uint64_t>(list_address + i * sizeof(uint64_t));
			auto current_vtable_rel = proc.Read<uint64_t>(pbuffer) - data.base;
			if (current_vtable_rel == VTMARKER_OFFSET)
			{
				auto spotted = proc.Read<uint8_t>(pbuffer + 0x632);
				if (!spotted)
				{
					updated = true;
					proc.Write<uint8_t>(pbuffer + 0x632, 1);
				}
			}
		}
	}
	if (updated)
	{
		printf("Cav esp updated\n");
	}
}

void enable_no_recoil(WinProcess& proc, const R6Data& data)
{
	if (!USE_NO_RECOIL) return;

	if (data.fov_manager == 0) return;
	proc.Write<float>(data.fov_manager + 0xE34, 0.f);
	proc.Write<float>(data.weapon_info + 0x198, 0.f); //rec b
	proc.Write<float>(data.weapon_info + 0x18c, 0.f); //rec v
	proc.Write<float>(data.weapon_info + 0x17c, 0.f); //rec h
	printf("No recoil active\n");
}

void enable_no_spread(WinProcess& proc, const R6Data& data)
{
	if (!USE_NO_SPREAD) return;

	if (data.weapon_info == 0) return;
	proc.Write<float>(data.weapon_info + 0x80, 0.f); // no spread
	printf("No spread active\n");
}

void enable_run_and_shoot(WinProcess& proc, const R6Data& data)
{
	if (!USE_RUN_AND_SHOOT) return;

	if (data.base == 0) return;
	proc.Write<uint8_t>(data.base + 0x1E59401 + 0x6, 0x1); // run and shoot
	proc.Write<uint8_t>(data.base + 0x33AE195 + 0x6, 0x1); // run and shoot
	printf("Run and shoot active\n");
}

void enable_no_flash(WinProcess& proc, const R6Data& data)
{
	if (!USE_NO_FLASH) return;

	if (data.local_player == 0) return;
	auto player = proc.Read<uint64_t>(data.local_player + 0x30);
	player = proc.Read<uint64_t>(player + 0x31);
	auto noflash = proc.Read<uint64_t>(player + 0x28);
	proc.Write<uint8_t>(noflash + 0x40, 0); // noflash
	printf("Noflash active\n");
}

void enable_no_aim_animation(WinProcess& proc, const R6Data& data)
{
	if (!USE_NO_AIM_ANIM) return;

	if (data.local_player == 0) return;
	auto no_anim = proc.Read<uint64_t>(data.local_player + 0x90);
	no_anim = proc.Read<uint64_t>(no_anim + 0xc8);
	proc.Write<uint8_t>(no_anim + 0x384, 0);
	printf("No aim animation active\n");
}

void set_fov(WinProcess& proc, const R6Data& data, float fov_val)
{
	if (!CHANGE_FOV) return;

	auto fov = proc.Read<uint64_t>(data.base + FOV_MANAGER_OFFSET);
	if (fov == 0) return;
	fov = proc.Read<uint64_t>(fov + 0x60) + 0xe658f449242c196;
	if (fov == 0) return;
	auto playerfov = proc.Read<uint64_t>(fov + 0x0) + 0x38;
	if (playerfov == 0) return;
	proc.Write<float>(playerfov, fov_val); // player fov ~1.2f-2.3f
	printf("Player fov changed to %f\n", fov_val);
}

void set_firing_mode(WinProcess& proc, const R6Data& data, FiringMode mode)
{
	if (!CHANGE_FIRING_MODE) return;

	if (data.curr_weapon == 0) return;
	proc.Write<uint32_t>(data.curr_weapon + 0x118, (uint32_t)mode); //firing mode 0 - auto 3 - single 2 -  burst
	printf("Fire mode: %s\n", (mode == FiringMode::AUTO ? "auto" : mode == FiringMode::BURST ? "burst" : "single"));
}

void enable_glow(WinProcess& proc, const R6Data& data)
{
	if (!USE_GLOW) return;
	auto chain = proc.Read<uint64_t>(data.glow_manager + 0xb8);
	if (chain != 0)
	{
		proc.Write<float>(chain + 0xd0, 1.f); //r
		proc.Write<float>(chain + 0xd4, 1.f);	//g
		proc.Write<float>(chain + 0xd8, 0.f); //b
		proc.Write<float>(chain + 0x110, 100.f); //distance
		proc.Write<float>(chain + 0x118, 0.f); //a
		proc.Write<float>(chain + 0x11c, 1.f); //opacity
		printf("Glow active\n");
	}
	else
	{
		printf("[ERROR]: Can not activate glow\n");
	}
}

int32_t get_game_state(WinProcess& proc, const R6Data& data)
{
	if (data.round_manager == 0) return -1;
	return proc.Read<uint8_t>(data.round_manager + 0x2e8); // 3/2=in game 1=in operator selection menu 5=in main menu
}

//player is currently in operator selection menu
bool is_in_op_select_menu(WinProcess& proc, const R6Data& data)
{
	return get_game_state(proc, data) == 1;
}

bool is_in_main_menu(WinProcess& proc, const R6Data& data)
{
	return get_game_state(proc, data) == 5;
}

bool is_in_game(WinProcess& proc, const R6Data& data)
{
	return !is_in_op_select_menu(proc, data) && !is_in_main_menu(proc, data) && get_game_state(proc, data) != 0;
}

void unlock_all(WinProcess& proc, const R6Data& data)
{
	if (!UNLOCK_ALL) return;
	uint8_t shellcode[] = { 65, 198, 70, 81, 0, 144 };
	proc.WriteMem(data.base + 0x271470B, shellcode, sizeof(shellcode));
	printf("Unlock all executed\n");
}

void update_all(WinProcess& proc, R6Data& data, ValuesUpdates& update)
{
	enable_esp(proc, data);
	if (update.update_no_recoil)
	{
		enable_no_recoil(proc, data);
		update.update_no_recoil = false;
		printf("No recoil updated\n");
	}
	if (update.update_no_spread)
	{
		enable_no_spread(proc, data);
		update.update_no_spread = false;
		printf("No spread updated\n");
	}
	if (update.update_no_flash)
	{
		enable_no_flash(proc, data);
		update.update_no_flash = false;
		printf("No flash updated\n");
	}
	if (update.update_firing_mode)
	{
		set_firing_mode(proc, data, CURRENT_FIRE_MODE);
		update.update_firing_mode = false;
		printf("Firing mode updated\n");
	}
	if (update.update_fov)
	{
		set_fov(proc, data, NEW_FOV);
		update.update_fov = false;
		printf("Fov updated\n");
	}
	if (update.update_fast_aim)
	{
		enable_no_aim_animation(proc, data);
		update.update_fast_aim = false;
	}

	enable_glow(proc, data);
}

void check_update(WinProcess& proc, R6Data& data, ValuesUpdates& update)
{
	static bool esp_updated = false;

	if (USE_CAV_ESP)
	{
		if (esp_updated)
		{
			if (!is_in_game(proc, data))
			{
				esp_updated = false;
			}
		}
		if (!esp_updated && is_in_game(proc, data))
		{
			update.update_cav_esp = true;
			esp_updated = true;
		}
	}
	if (USE_NO_SPREAD)
	{
		float spread_val = proc.Read<float>(data.weapon_info + 0x80);
		if (spread_val)
		{
			update.update_no_spread = true;
		}
	}
	if (USE_NO_RECOIL)
	{
		float no_recoil_b = proc.Read<float>(data.weapon_info + 0x198);
		if (no_recoil_b)
		{
			update.update_no_recoil = true;
		}
	}
	if (USE_NO_FLASH)
	{
		auto player = proc.Read<uint64_t>(data.local_player + 0x30);
		player = proc.Read<uint64_t>(player + 0x31);
		auto noflash = proc.Read<uint64_t>(player + 0x28);
		auto noflash_value = proc.Read<uint8_t>(noflash + 0x40);
		if (noflash_value)
		{
			update.update_no_flash = true;
		}
	}
	if (CHANGE_FIRING_MODE)
	{
		uint32_t firing_mode = proc.Read<uint32_t>(data.curr_weapon + 0x118);
		if (firing_mode != (uint32_t)CURRENT_FIRE_MODE)
		{
			update.update_firing_mode = true;
		}
	}
	if (CHANGE_FOV)
	{
		auto fov = proc.Read<uint64_t>(data.base + FOV_MANAGER_OFFSET);
		fov = proc.Read<uint64_t>(fov + 0x60) + 0xe658f449242c196;
		auto playerfov = proc.Read<uint64_t>(fov + 0x0) + 0x38;
		auto fov_value = proc.Read<float>(playerfov);
		if (fov_value != NEW_FOV)
		{
			update.update_fov = true;
		}
	}
	if (USE_NO_AIM_ANIM)
	{
		auto no_anim = proc.Read<uint64_t>(data.local_player + 0x90);
		no_anim = proc.Read<uint64_t>(no_anim + 0xc8);
		auto val = proc.Read<uint8_t>(no_anim + 0x384);
		if (val)
		{
			update.update_fast_aim = true;
		}
	}
}

void write_loop(WinProcess& proc, R6Data& data)
{
	printf("Write thread started\n\n");
	ValuesUpdates update = { false,false,false,false,false,false,false,false };
	while (run_cheat)
	{
		read_data(proc, data,
			data.base == 0
			|| data.fov_manager == 0
			|| data.curr_weapon == 0
			|| data.game_manager == 0
			|| data.glow_manager == 0
			|| data.round_manager == 0
			|| data.weapon_info == 0);

		while ((is_in_main_menu(proc, data))
			|| get_game_state(proc, data) == 0) //waiting until the game is started
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		check_update(proc, data, update);

		if (is_in_game(proc, data))
		{
			update_all(proc, data, update);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	printf("Exiting...\n");
}*/


void Glow() {

	float value = 999;
	uint64_t fovManager = GameGlobal::Mem.pFovManager;

	uint32_t distance = *(uint32_t*)&value;
	distance = __ROR4__(__ROR4__(distance, 0x12) ^ 0x1B39D2AE, 0xA);

	//dr->WPMP<uint32_t>(fovManager + 0x40, distance);
	/*
	ULONG64 glowbase = dr->RPM<ULONG64>(GameGlobal::BaseAddress + Offset::GlowManager);// 0x605c388
	ULONG64 glowbase1 = dr->RPM<ULONG64>(glowbase + Offset::GlowChain);
	dr->WPM<float>(glowbase1 + Offset::GlowColor, 1);// red
	dr->WPM<float>(glowbase1 + Offset::GlowColor + 0x4, 1);// green
	dr->WPM<float>(glowbase1 + Offset::GlowColor + 0x8, 0);// blue
	//ULONG64 distance = __ROR4__(__ROR4__((glowbase1 + Offset::GlowIntensity), 0x10) - 0x204EAB81, 0xC);
	dr->WPM<float>(glowbase1 + Offset::GlowIntensity, 100);// distance
	dr->WPM<float>(glowbase1 + Offset::GlowIntensity + 0x8, 0);// alpha
	//DWORD32 encrypted_opcacity = (((((glowbase1 + Offset::GlowIntensity + 0xc)) ^ 0xE659C3E8) - 0xFF29AF1A) ^ 0x7F7C1AAA);
	dr->WPM<float>(glowbase1 + Offset::GlowIntensity + 0xc, 1);// opacity
	*/
}


void unlock_all()
{ 
	
	uint8_t shellcode[] = { 0x41, 0xC6, 0x46, 0x51, 0x00, 0x90 };
	//dr->WPMP(GameGlobal::BaseAddress + Offset::unlock_all, shellcode);
	//dr->WBT(GameGlobal::BaseAddress + Offset::unlock_all, shellcode, sizeof(shellcode));
	//printf("%d\n", dr->RPM<byte>(GameGlobal::BaseAddress + Offset::unlock_all));
	/*
	ULONG64 unlock_all = dr->RPM<ULONG64>(GameGlobal::BaseAddress + Offset::unlock_all);
	unlock_all = dr->RPM<ULONG64>(unlock_all + 0xBF8);
	 unlock_all = dr->RPM<ULONG64>(unlock_all + 0x0);

	 for (auto idx = 0u; idx < 56; ++idx) {
		 dr->WPMP<uint32_t>(unlock_all  + (0x30c + idx * 0x10), true);
	 }*/
	//+0x05CD8F40] + 0xBF8] + 0x 0] + 0x30C + i * 0x10]
	//printf("Unlock all executed\n");
}

void enable_no_recoil(ULONG64 fov_manager,ULONG64 weapon_info)
{


	//if (fov_manager == 0) return;
	dr->WPMP<BYTE>(weapon_info + 0x94, Encryption::EncryptEnableRecoil(0));
	uint32_t recoilX[3] = {};
	Encryption::EncryptRecoilX(0, recoilX);

	uint32_t recoilY[3] = {};
	Encryption::EncryptRecoilY(0, recoilY);

	Vector3 aa;
	aa.x = recoilX[0];
	aa.y = recoilX[1];
	aa.z = recoilX[2];
	Vector3 bb;
	bb.x = recoilY[0];
	bb.y = recoilY[1];
	bb.z = recoilY[2];
	dr->WPMP< Vector3>(weapon_info + 0X100, aa);

	dr->WPMP<Vector3>(weapon_info + 0X10C, bb);
}

void enable_no_spread(ULONG64 weapon_info)//1
{


	if (1)
	{
		dr->WPMP<uint32_t>(weapon_info + 0x158, Encryption::Spread(0));
		return;
	}


	dr->WPMP(weapon_info + 0x158, Encryption::Spread(0.750f));
}
	//printf("No spread active\n");


void enable_run_and_shoot()
{
	//if (!USE_RUN_AND_SHOOT) return;
	
	if (GameGlobal::BaseAddress == 0) return;
	dr->WPM<uint8_t>(GameGlobal::BaseAddress + 0x2D991C0 + 0x6, 0x1); // run and shoot
	dr->WPM<uint8_t>(GameGlobal::BaseAddress + 0x14667c1 + 0x6, 0x1);
	
}

void enable_no_flash(ULONG64 local_player)
{
	//if (!USE_NO_FLASH) return;

	if (local_player == 0) return;
	auto player =dr->RPM<uint64_t>(local_player + 0x30);
	player = dr->RPM<uint64_t>(player + 0x31);
	auto noflash = dr->RPM<uint64_t>(player + 0x28);
	dr->WPM<uint8_t>(noflash + 0x40, 0); // noflash
	printf("Noflash active\n");
}

void enable_no_aim_animation(ULONG64 local_player)
{
	//if (!USE_NO_AIM_ANIM) return;

	if (local_player == 0) return;
	auto no_anim = dr->RPM<uint64_t>(local_player + 0x90);
	no_anim = dr->RPM<uint64_t>(no_anim + 0xc8);
	dr->WPM<uint8_t>(no_anim + 0x384, 0);
	//printf("No aim animation active\n");
}

void set_fov( float fov_val)
{
	//if (!CHANGE_FOV) return;

	auto fov = dr->RPM<uint64_t>(GameGlobal::BaseAddress + Offset::FovManager);
	if (fov == 0) return;
	fov = dr->RPM<uint64_t>(fov + 0x60) + 0xe658f449242c196;
	if (fov == 0) return;
	auto playerfov = dr->RPM<uint64_t>(fov + 0x0) + 0x38;
	if (playerfov == 0) return;
	dr->WPM<float>(playerfov, fov_val); // player fov ~1.2f-2.3f
	printf("Player fov changed to %f\n", fov_val);
}

void set_firing_mode(ULONG64 curr_weapon,FiringMode mode)
{
	//if (!CHANGE_FIRING_MODE) return;

	if (curr_weapon == 0) return;
	dr->WPM<uint32_t>(curr_weapon + 0x118, (uint32_t)mode); //firing mode 0 - auto 3 - single 2 -  burst
	printf("Fire mode: %s\n", (mode == FiringMode::FullAuto ? "auto" : mode == FiringMode::Burst2 ? "burst" : "single"));
}

void autogun(ULONG64 LocalPlayer
) {

	auto unknown = dr->RPM<uintptr_t>(LocalPlayer + 0x1c80);
	

	auto pInstanceOne = dr->RPM<uintptr_t>(unknown + 0xd0);
	


	auto pInstanceTwo = dr->RPM<uintptr_t>(pInstanceOne + 0x0);
	

	auto pInstanceEncrypted = dr->RPM<uintptr_t>(pInstanceTwo + 0x2A8);

	

	/*
	ULONG64 WeaponInfo = dr->RPM<ULONG64>(LocalPlayer + Offset::ENTITY_WEAPON_INFO);
	if (!WeaponInfo)
		return;


	ULONG64 HoldingWeapon = dr->RPM<ULONG64>(WeaponInfo + Offset::WEAPON_INFO_HOLDING_WEAPON);
	if (!HoldingWeapon)
		return;
	
	ULONG64 WeaponData = dr->RPM<ULONG64>(HoldingWeapon + Offset::HOLDING_WEAPON_DATA_OFFSET);
	WeaponData = __ROL8__(WeaponData, 0x1D);
	WeaponData -= 0x49;
	WeaponData ^= 0xC07C346E918191F1;
	if (!WeaponData)
		return;*/
	
	//if (4 > dr->RPM<int>(HoldingWeapon + Offset::HOLDING_WEAPON_Auto_OFFSET) >= 0) //有數值代表手上有槍
	{
	//dr->WPMP<int>(HoldingWeapon + Offset::HOLDING_WEAPON_Auto_OFFSET, 0);
		ULONG64 WeaponClass = Decryption::WeaponClass(pInstanceEncrypted);
		enable_no_recoil(GameGlobal::Mem.pFovManager, WeaponClass);
		//enable_no_spread(WeaponClass);
	}
	



}


void speedRun(ULONG64 fLocalPlayer) {//0

	uintptr_t chain = NULL;
	if (!(chain = (dr->RPM<uintptr_t>(fLocalPlayer + 0x30), 0x1F) + 0xD1ECA41690E3EAC9)) return;
	if (!(chain = dr->RPM<uintptr_t>(chain + 0x30))) return;
	if (!(chain = dr->RPM<uintptr_t>(chain + 0x38))) return;
	if (bSpeedRun) {
		dr->WPMP<int>(chain + 0x58, 160);//OLD 105
	}
	else if (dr->RPM<int>(chain + 0x58) != 105 && !bSpeedRun) {
		dr->WPMP<int>(chain + 0x58, 115);//OLD 105

	}

	//printf("%d\n " ,dr->RPM<int>(chain + 0x58));
}

void long_knife(ULONG64 fLocalPlayer)//0
{

	uintptr_t chain = NULL;
	if (fLocalPlayer == 1) return;
	if (!(chain = (dr->RPM<uintptr_t>(fLocalPlayer + 0x30) - 0x60) ^ 0x2F) + 0x192EFC05F71B13D4) return;
	if (!(chain = dr->RPM<uintptr_t>(chain + 0x80))) return;
	if (!(chain = dr->RPM<uintptr_t>(chain + 0x30))) return;
	uint64_t  noclip = dr->RPM<uint64_t>(GameGlobal::BaseAddress + Offset::NoClip);
	noclip = dr->RPM<uint64_t>(noclip + 0x4E8);
	noclip = dr->RPM<uint64_t>(noclip + 0x8);

	//加速
	speedRun(fLocalPlayer);

	if (GetAsyncKeyState(VK_CAPITAL) & 0x8000) {
		//穿牆
		dr->WPMP<float>(noclip + 0x700, -1);
		dr->WPMP<float>(noclip + 0x704, -1);
		dr->WPMP<float>(noclip + 0x708, -1);
		dr->WPMP<float>(noclip + 0x70C, -1);
		//長刀
		dr->WPMP<float>(chain + 0x60, 750);
		dr->WPMP<float>(chain + 0x64, 750);
	}
	else {


		dr->WPMP<float>(noclip + 0x700, 0.000305);
		dr->WPMP<float>(noclip + 0x704, 0.000305);
		dr->WPMP<float>(noclip + 0x708, 0.000305);
		dr->WPMP<float>(noclip + 0x70C, 0.000305);

		dr->WPMP<float>(chain + 0x60, 2);
		dr->WPMP<float>(chain + 0x64, 2);

	}



}

void LH_Rd::insane(ULONG64 fLocalPlayer) {
	//hack memory 
	//long_knife(fLocalPlayer);
	
	autogun(fLocalPlayer);// LocalEntity2 or Getlocalplayer連發
	//enable_run_and_shoot();

	//dr->WBT(GameGlobal::BaseAddress + Offset::NoRecoil, Offset::NoRecoilOn, sizeof(Offset::NoRecoilOn));
}
//CAV ESP
/*
bool r6manager::enable_esp()
{
	printf("ESP");
	auto entity_list = read<uint64_t>(this->m_base + OFFSET_GAME_MANAGER + 0xB8) ^ 0x681C75805DE8EEA2;
	//auto entity_list = read<uint64_t>(this->m_base + OFFSET_GAME_MANAGER);
	entity_list = read<uint64_t>(entity_list + 0xB8) ^ 0x681C75805DE8EEA2;
	for (int i = 0; i < 11; ++i)
	{
		auto entity_address = read<uint64_t>(entity_list + (0x8 * i)); //entity_object
		auto buffer = read<uint64_t>(entity_address + 0x18);
		auto size = read<uint8_t>(buffer + 0xE0) & 0x4BF64B0; // entity_count
		auto list_address = read<uint64_t>(buffer + 0xD8);
		for (uint8_t i = 0; i < size; ++i)
		{
			auto pbuffer = read<uint64_t>(list_address + i * sizeof(uint64_t));
			auto current_vtable_rel = read<uint64_t>(pbuffer) - this->m_base;
			if (current_vtable_rel == ENTITY_MARKER_VT_OFFSET)
			{
				auto spotted = read<uint8_t>(pbuffer + 0x632);
				if (!spotted)
				{
					//updated = true;
					write<uint8_t>(pbuffer + 0x632, 1);
				}
			}
		}
		return true;
	}
}
*/

void LH_Rd::ESP() {
	VMProtectBegin("ESP");
	//Update addresses
	GameGlobal::Mem.UpdateAddresses();


	//Set our colors for later
	DWORD boxColor = ImGui_ARGB(255, 255, 255, 0); //white
	DWORD headCircleColor = ImGui_ARGB(255, 62, 229, 65); //green
	DWORD healthBarColor = ImGui_ARGB(255, 240, 96, 96); //redish
	DWORD nameColor = ImGui_ARGB(255, 250, 190, 253); //pinkish
	DWORD blackColor = ImGui_ARGB(255, 0, 0, 0); //black
	if (GetAsyncKeyState(VK_END) & 0x8000) {


		//dr->WBT(GameGlobal::GameRet, GameGlobal::Old,GameGlobal::oldsize);

		ExitProcess(0);
	}
	if (GetAsyncKeyState(VK_F1) &0x8000) {

		Sleep(100);
		baimbot = baimbot == false ? true : false;
	}
	if (GetAsyncKeyState(VK_F2) & 0x8000) {

		Sleep(100);
		bNoRecoil = bNoRecoil == false ? true : false;
	}
	if (GetAsyncKeyState(VK_F3) & 0x8000)
	{
		Sleep(100);
		bSpeedRun = bSpeedRun == false ? true : false;
	}
	if (GetAsyncKeyState(VK_F4) & 0x8000)
	{
		Sleep(100);
		bsilentaim = bsilentaim == false ? true : false;
	}
	if (GetAsyncKeyState(VK_NUMPAD0) & 0x8000) {

		Sleep(100);
		teammode = teammode > 1 ? 0 : teammode+1;
	}
	if (GetAsyncKeyState(VK_ADD) & 0x8000)
	{

		Sleep(100);
		smooth += 0.1;
	}
	if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
	{

		Sleep(100);
		smooth -= 0.1;
	}


	if (baimbot == true) {
		GameGlobal::Rd.DrawString(10, 10
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[F1][自苗開啟!!!!!!!!]"));
	}
	if (bNoRecoil == true)
	{

		GameGlobal::Rd.DrawString(10, 25
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[F2][無後座開啟!!!!!!!!]"));

	}
	if (bSpeedRun == true)
	{
		GameGlobal::Rd.DrawString(10, 40
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[F3][加速開啟!!!!!!!!]"));

	}
	if (bsilentaim == true)
	{
		GameGlobal::Rd.DrawString(10, 55
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[F4][無作用!!!!!!!!]"));

	}


	//Fov circle
	GameGlobal::Rd.DrawCircle(flCenterX, flCenterY, aimfov, 50, ImGui_ARGB(255, 255, 0, 0));
	
	
     //unlock_all();
	
	
	LocalEntity2 = GameGlobal::Mem.GetLocalEntity2(0);

	/*//檢查 玩家
	bool isPlayer(__int64 entity) {
    return __ROL1__(*(_BYTE*) (entity + 0x48) - 0x1D, 2);
}
	*/
	Glow();//glow player
	for (int i = 0; i < 50; i++) {// GameGlobal::Mem.GetPlayerCount()
	
				//Get the current entiENtyb
		DWORD_PTR Entity = GameGlobal::Mem.GetEntity(i);//read twice with EntityRef:0x20 
		DWORD_PTR Entity2 = GameGlobal::Mem.GetEntity2(i);//just read once
	

	
		
		//Get a PlayerInfo struct with all of this entity's info
		 Player = GameGlobal::Mem.GetAllEntityInfo(Entity, Entity2);
	
		 // Distance
		int Distance = LocalPlayer.Head.Distance(Player.Head);

	
		GameGlobal::Rd.DrawString(500, 10
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[自苗速度: %f ]"), smooth);
		GameGlobal::Rd.DrawString(500, 25
			, ImGui_ARGB(255, 0, 255, 0), GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"[團隊模式] : %d  : %X"), teammode, LocalPlayer.TeamId);

	

		//insane function
		ULONG64 fLocalPlayer = GameGlobal::Mem.get_localplayer();
		if (fLocalPlayer != 1)
			insane(fLocalPlayer);

		if (!(Player.TeamId == 3 || Player.TeamId == 4))//非實體玩家
			continue;
		if (LocalEntity2 != 0)//本地玩家存在
		{

			if (Entity2 == LocalEntity2)//過濾自己
				continue;

			//if (Player.TeamId == LocalPlayer.TeamId)//過濾團隊
			//	continue;



			if (teammode == 0) {
				if (Player.TeamId == LocalPlayer.TeamId)
				continue;


			}
			else if (teammode == 1) {
				if (Player.TeamId != LocalPlayer.TeamId)
					continue;
			}
			else if (teammode == 2) {

			}
		}
		//auto orange =dr->RPM<uint8_t>(GameGlobal::Mem.pGameManager + 0x6A8); //defense 80
		//auto blue = dr->RPM<uint8_t>(GameGlobal::Mem.pGameManager + 0x458);

	//	dr->WPMP<int>(GameGlobal::Mem.pGameManager + 0x6A8,80); //defense 80
		//dr->WPMP<int>(GameGlobal::Mem.pGameManager + 0x458, 81); //defense 81       82
		//auto blue = dr->WPMP<uint8_t>(GameGlobal::Mem.pGameManager + 0x458);
		//printf("orange: %d / Blue: %d\n", orange,blue); //attack 81)

		uint8_t shellcode[] = {117 };
		

		dr->WBT(GameGlobal::BaseAddress + 0x372F89B, shellcode, sizeof(shellcode));
		dr->WBT(GameGlobal::BaseAddress + 0x372FFC1, shellcode, sizeof(shellcode));
		dr->WPMP<float>(dr->RPM<uintptr_t>(GameGlobal::BaseAddress + 0x5e12f50) + 0x4, 99);
		
		if (
			Player.Health > 0 //Health over 0
			&& Player.Health <= 200 //Health less than 200
			&& Player.w2s.z >= 1.0f //Player is actually on the screen
			&& Player.w2sHead.z >= 1.0f //Player is actually on the screen
			//&& !Player.Name.empty() //Name isn't empty
			//&& Player.TeamId != LocalPlayer.TeamId
			&& ((Distance / 1000) /*km to m*/ < 2)
			) //Not on our team
		{

			
			ULONG64 Playerocnt = GameGlobal::Mem.GetPlayerController(Entity);
			ULONG64 PlayerInfo = Decryption::PlayerInfo(dr->RPM<ULONG64>(Playerocnt + Offset::PlayerInfo));
			BYTE CTU = dr->RPM<BYTE>(PlayerInfo + Offset::CTU);
			BYTE OP = dr->RPM<BYTE>(PlayerInfo + Offset::OP);
		
			if (CTU < 0 || OP < 0 || CTU > 255 || OP > 255)//no people
				continue;
		
			std::string  ThisOperatorName = OperatorName[CTU][OP];
		
			float BoxHeight1 = Player.w2s.y - Player.w2sHead.y;
			float BoxHeight2 = std::abs(Player.w2s.x - Player.w2sHead.x);
			float BoxHeight = (BoxHeight1 > BoxHeight2) ? BoxHeight1 : BoxHeight2;
			float BoxWidth = BoxHeight / 1.15f;


			//box 
			GameGlobal::Rd.DrawBox(Player.w2sHead.x - (BoxWidth / 2) - 1, Player.w2sHead.y - (BoxHeight / 5) - 1, BoxWidth + 2, BoxHeight + 12, blackColor);
			GameGlobal::Rd.DrawBox(Player.w2sHead.x - (BoxWidth / 2), Player.w2sHead.y - (BoxHeight / 5), BoxWidth, BoxHeight + 10, boxColor);

			//Draw Head
			GameGlobal::Rd.DrawCircle(Player.w2sHead.x, Player.w2sHead.y - 3, BoxHeight / 10, 20, headCircleColor);

			//Draw health bars
			//Find the current health
			//Health goes to 120, 0-20 is down, 20-120 is regular health
			float CurrentHealth = Player.Health / 120.0f;
			if (CurrentHealth < 0 || CurrentHealth >= 1) CurrentHealth = 1;

			//Draw the health bar
			GameGlobal::Rd.FillArea(Player.w2sHead.x - (BoxWidth / 2) - 6 - 1, Player.w2sHead.y - (BoxHeight / 5) - 1, 5, BoxHeight + 10 + 2, blackColor);
			GameGlobal::Rd.FillArea(Player.w2sHead.x - (BoxWidth / 2) - 6, Player.w2sHead.y - (BoxHeight / 5), 3, (BoxHeight + 10) * CurrentHealth, healthBarColor);

			
			//Draw Roles
			GameGlobal::Rd.DrawString(Player.w2sChest.x, Player.w2sChest.y
				, nameColor, GameGlobal::Rd.d3Font, VMProtectDecryptStringA(u8"人物: %s %d/%d"), ThisOperatorName, CTU, OP);

			//Skeleton
			GameGlobal::Mem.DrawSkeleton(Entity, Playerocnt, CTU, OP, ThisOperatorName);

			if (BestEntity == Entity) {
				GameGlobal::Rd.DrawString(
					Player.w2sHead.x - (BoxWidth / 2) - 1
					, (Player.w2sHead.y - (BoxHeight / 5)) + BoxHeight + 10 - 1
					, nameColor
					, GameGlobal::Rd.d3Font
					, VMProtectDecryptStringA(u8"|名稱: %s\n|ABS:%d\n|距離: %d\n"), Player.Name
					, (int)sqrt(((Player.w2sHead.x - flCenterX) * (Player.w2sHead.x - flCenterX)) + ((Player.w2sHead.y - flCenterY) * (Player.w2sHead.y - flCenterY)))
					, Distance
				);
			}



			float headX = Player.w2sHead.x - flCenterX;
			float headY = Player.w2sHead.y - flCenterY;

			float ChestX = Player.w2sChest.x - flCenterX;
			float ChestY = Player.w2sChest.y - flCenterY;
			if (baimbot == true) {

				if (Player.w2sHead.x - flCenterX < aimfov && flCenterX - Player.w2sHead.x < aimfov && flCenterY - Player.w2sHead.y < aimfov && Player.w2sHead.y - flCenterY < aimfov)
				{

					if (!(GetAsyncKeyState(VK_RBUTTON))) {
						AimBotInfo::Entity.clear();
						AimBotInfo::Health.clear();
						AimBotInfo::Distance.clear();
						AimBotInfo::ShotPos.clear();
						AimBotInfo::fov.clear();

						BestEntity = 0x0;
						TwoBsetEntity = 0x0;
						BestTwoEntityDistance = -1;
						LocalEntity2 = 0x0, LocalEntity = 0x0;
					}

					if (Player.Health < 20)

						continue;
					if (GetAsyncKeyState(VK_RBUTTON)) {
						if (PlayerInfo)
						{
							//if (ThisOperatorName != "") 
							{

								if (CTU >= 0 && OP >= 0 && CTU != 255 && OP != 255)
								{
									AimBotInfo::Entity.push_back(Entity);
									AimBotInfo::Health.push_back(Player.Health);
									AimBotInfo::Distance.push_back(Distance);
									int math = (float)sqrt((headX * headX) + (headY * headY));
									AimBotInfo::fov.push_back(math);
									AimBotInfo::ShotPos.push_back(Vector3{ ChestX,	ChestY,0 });
									Vector3 angles = GameGlobal::Mem.CalcAngle(Player.Head, GameGlobal::Mem.GetViewTranslation());//GameGlobal::Mem.GetViewTranslation() LocalPlayer.Hea
									//std::cout << "------------------------------------------" << std::endl;
									//std::cout << " TWo " << std::endl;
									//calculate_angle(GameGlobal::Mem.GetViewTranslation(), Player.Head)
									//Vector3 angles = GameGlobal::Mem.CalcAngle(LocalPlayer.Head, GameGlobal::Mem.GetViewTranslation());//GameGlobal::Mem.GetViewTranslation() LocalPlayer.Hea

									//GameGlobal::Mem.set_viewAngle(LocalEntity, LocalEntity2, angles, true);

									if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
										AimBotInfo::ShotPos.push_back(Vector3{ headX,headY,0 });


									}
								}
							}
						}
					}
				}
			}
		}

	}

	ShootTarget();
	VMProtectEnd();
}

void LH_Rd::ShootTarget() {


	if (baimbot == true && AimBotInfo::Entity.size() > 0) {

		int bsetEntityFov = AimBotInfo::fov[0];

		int  Count = 0;
		for (int i = 0; i < AimBotInfo::Entity.size(); i++) {
			int fov1 = AimBotInfo::fov[i];

			if (fov1 < bsetEntityFov) {
				bsetEntityFov = AimBotInfo::fov[i];
				Count = i;
				continue;
			}

		}
		// VK_SHIFT VK_RBUTTON
		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000 && Count != -1)// && m_target == Entity
		{
			
			 BestEntity = AimBotInfo::Entity[Count];
			 if (Count > 0) {
		
				  TwoBsetEntity = AimBotInfo::Entity[Count - 1];
				  BestTwoEntityDistance = AimBotInfo::Distance[Count - 1];
			 }
		
			int BestEntityHealth = AimBotInfo::Health[Count];
			int BestEntityDistance = AimBotInfo::Distance[Count];
			Vector3 BestEntityShotPos = AimBotInfo::ShotPos[Count];
			//if (BestTwoEntityDistance > 0 && BestTwoEntityDistance < BestEntityDistance && TwoBsetEntity != 0x0) //判斷最近
				//BestEntity = TwoBsetEntity;
		

			Vector3 BestChestPos = GameGlobal::Mem.WorldToScreen(GameGlobal::Mem.GetEntitChestPosition(BestEntity));
			Vector3 BestHeadPos = GameGlobal::Mem.WorldToScreen(GameGlobal::Mem.GetEntityHeadPosition(BestEntity));

			float ChestX = BestChestPos.x - (GameGlobal::Rd.ScreenWidth / 2);
			float ChestY = BestChestPos.y - (GameGlobal::Rd.ScreenHeight / 2);

			float HeadX = BestHeadPos.x - (GameGlobal::Rd.ScreenWidth / 2);
			float HeadY = BestHeadPos.y - (GameGlobal::Rd.ScreenHeight / 2);

			if (GetAsyncKeyState(VK_SPACE) & 0x8000)//VK_MENU
			{

			mouse_event(MOUSEEVENTF_MOVE, HeadX * smooth, HeadY * smooth, NULL, NULL);
		
			}
			//if (ChestX >= -50 && ChestX <= 50 && ChestY >= -50 && ChestY <= 50) {
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
				mouse_event(MOUSEEVENTF_MOVE, ChestX * smooth, ChestY * smooth, NULL, NULL);

				//set_viewAngle(LocalEntity, LocalEntity2, calculate_angle(GameGlobal::Mem.GetViewTranslation(), GameGlobal::Mem.GetEntityHeadPosition(BestEntity)));
			}

		}

	}
	AimBotInfo::Entity.clear();
	AimBotInfo::Health.clear();
	AimBotInfo::Distance.clear();
	AimBotInfo::ShotPos.clear();
	AimBotInfo::fov.clear();
	//BestEntity = 0x0;
	//TwoBsetEntity = 0x0;
	//BestTwoEntityDistance = -1;
	//LocalEntity2 = 0x0, LocalEntity = 0x0;
}


//一定要從頭開始學  回家後 先看youtube的 薩拉課程 或有骨骼的  然後再看國外的  明天看到後放到fb務必
//网易云课堂游戏逆向过检测：第七十一课：过掉驱动
//d3d render
/*
void LH_Rd::Render() {
	VMProtectBegin("Render");
	d3dev->Clear(0, NULL, D3DCLEAR_TARGET, ImGui_ARGB(0, 0, 0, 0), 1.0f, 0);
	d3dev->BeginScene();

	ESP();

	d3dev->EndScene();
	d3dev->Present(NULL, NULL, NULL, NULL);
	VMProtectEnd();
}


void LH_Rd::InitD3D(HWND handleWindow) {

	VMProtectBegin("InitD3D");
	//Create our direct3d object
	d3 = Direct3DCreate9(D3D_SDK_VERSION);
	if (d3 == NULL) {
		return ;
	}
	D3DPRESENT_PARAMETERS direct3DPresetParams;

	ZeroMemory(&direct3DPresetParams, sizeof(direct3DPresetParams));

	direct3DPresetParams.Windowed = true;
	direct3DPresetParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	direct3DPresetParams.hDeviceWindow = handleWindow;
	direct3DPresetParams.BackBufferFormat = D3DFMT_A8R8G8B8;
	direct3DPresetParams.BackBufferWidth = ScreenWidth;
	direct3DPresetParams.BackBufferHeight = ScreenHeight;

	direct3DPresetParams.EnableAutoDepthStencil = TRUE;
	direct3DPresetParams.AutoDepthStencilFormat = D3DFMT_D16;

	if(!FAILED(d3->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, handleWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &direct3DPresetParams, &d3dev))){

	D3DXCreateFontA(d3dev, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &d3Font);

	D3DXCreateLine(d3dev, &d3Line);
	VMProtectEnd();
}
}

void LH_Rd::DrawCircle(int X, int Y, int radius, int numSides, DWORD color) {
	VMProtectBegin("DrawCircle");
	D3DXVECTOR2 Line[128];
	float Step = M_PI * 2.0 / numSides;
	int Count = 0;
	for (float a = 0; a < M_PI * 2.0; a += Step)
	{
		float X1 = radius * cos(a) + X;
		float Y1 = radius * sin(a) + Y;
		float X2 = radius * cos(a + Step) + X;
		float Y2 = radius * sin(a + Step) + Y;
		Line[Count].x = X1;
		Line[Count].y = Y1;
		Line[Count + 1].x = X2;
		Line[Count + 1].y = Y2;
		Count += 2;
	}
	d3Line->Draw(Line, Count, color);
	VMProtectEnd();
}

void LH_Rd::FillArea(float x, float y, float width, float height, D3DCOLOR color) {
	VMProtectBegin("FillArea");
	D3DXVECTOR2 vectorLine[2];
	d3Line->SetWidth(width);

	vectorLine[0].x = x + width / 2;
	vectorLine[0].y = y;
	vectorLine[1].x = x + width / 2;
	vectorLine[1].y = y + height;

	d3Line->Draw(vectorLine, 2, color);
	VMProtectEnd();
}

void LH_Rd::FillRectangle(double x, double y, double w, double h, D3DCOLOR color)
{
	VMProtectBegin("FillRectangle");
	struct Vertex
	{
		float x, y, z, h;
		D3DCOLOR color;
	};

	Vertex vertices[4] =
	{
		x,  y, 0.0f, 1.0f, color,
		x + w, y, 0.0f, 1.0f, color,

		x + w, y + h, 0.0f, 1.0f, color,
		x,  y + h, 0.0f, 1.0f, color,
	};

	d3dev->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	d3dev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	d3dev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3dev->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, vertices, sizeof(Vertex));
	VMProtectEnd();
}

void LH_Rd::DrawBox(float x, float y, float width, float height, D3DCOLOR color) {
	VMProtectBegin("DrawBox");
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3Line->SetWidth(1);
	d3Line->Draw(points, 5, color);
	VMProtectEnd();
}

void LH_Rd::DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color) {
	VMProtectBegin("DrawLine");
	D3DXVECTOR2 points[2];
	points[0] = D3DXVECTOR2(x1, y1);
	points[1] = D3DXVECTOR2(x2, y2);
	d3Line->SetWidth(1);
	d3Line->Draw(points, 2, color);
	VMProtectEnd();
}

void LH_Rd::DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char* fmt, ...)
{
	VMProtectBegin("DrawString");
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
	VMProtectEnd();
}
*/

bool CreateDeviceD3D(HWND hWnd)
{
	VMProtectBegin("CreateDeviceD3D");
	// Setup swap chain
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
		return false;

	CreateRenderTarget();

	VMProtectEnd();
	return true;
}

void CleanupDeviceD3D()
{
	VMProtectBegin("CleanupDeviceD3D");
	CleanupRenderTarget();
	if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
	if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	VMProtectEnd();
}

void CreateRenderTarget()
{
	VMProtectBegin("CreateRenderTarget");
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
	pBackBuffer->Release();
	VMProtectEnd();
}

void CleanupRenderTarget()
{
	VMProtectBegin("CleanupRenderTarget");
	if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	VMProtectEnd();
}


void LH_Rd::InitD3D(HWND handleWindow) {
	VMProtectBegin("InitD3D");
	// Initialize Direct3D
	if (!CreateDeviceD3D(handleWindow))
	{
		CleanupDeviceD3D();
		//::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return;
	}
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("ARIALUNI.TTF", 17.0f,NULL, io.Fonts->GetGlyphRangesChineseFull());
	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplWin32_Init(handleWindow);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	VMProtectEnd();
}

void LH_Rd::Render() {
	VMProtectBegin("Render");
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ESP();

	// Rendering
	ImGui::Render();
	g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
	g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	g_pSwapChain->Present(1, 0); // Present with vsync
	//g_pSwapChain->Present(0, 0); // Present without vsync
	VMProtectEnd();
}


void LH_Rd::EndRender() {
	VMProtectBegin("EndRender");
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	VMProtectEnd();
}

void LH_Rd::DrawCircle(int X, int Y, int radius, int numSides, D3DCOLOR color) {

	VMProtectBegin("DrawCircle");
	auto DrawList = ImGui::GetOverlayDrawList();
	DrawList->AddCircle(ImVec2{ (float)X,(float)Y }, radius, color, numSides, 1.0f);
	VMProtectEnd();
}

void LH_Rd::FillArea(float x, float y, float width, float height, D3DCOLOR color) {
	VMProtectBegin("FillArea");
	auto DrawList = ImGui::GetOverlayDrawList();
	DrawList->AddRectFilled(ImVec2{ (float)x,(float)y }, ImVec2{ x + width, y + height }, color, 0, 15);
	VMProtectEnd();
}

void LH_Rd::FillRectangle(double x, double y, double w, double h, D3DCOLOR color)
{
	VMProtectBegin("FillRectangle");


	VMProtectEnd();
}

void LH_Rd::DrawBox(float x, float y, float width, float height, D3DCOLOR color) {
	VMProtectBegin("DrawBox");
	auto DrawList = ImGui::GetOverlayDrawList();

	DrawList->AddRect(ImVec2{ x,y }, ImVec2{ x + width, y + height }, color, 0, 15, 1);
	VMProtectEnd();
}

void LH_Rd::DrawLine(float x1, float y1, float x2, float y2, D3DCOLOR color) {
	VMProtectBegin("DrawLine");
	auto DrawList = ImGui::GetOverlayDrawList();

	DrawList->AddLine(ImVec2{ x1,y1 }, ImVec2{ x2, y2 }, color, 1);
	VMProtectEnd();
}

void LH_Rd::DrawString(int x, int y, D3DCOLOR color, LPD3DXFONT g_pFont, const char* fmt, ...)
{
	VMProtectBegin("DrawString");
	auto DrawList = ImGui::GetOverlayDrawList();

	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);

	DrawList->AddText(ImVec2{ (float)x,(float)y }, color, buf);
	VMProtectEnd();
}

// old windproc
/*
LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	VMProtectBegin("WindowProc");

		switch (msg)
		{
		case WM_PAINT:
		//	GameGlobal::Rd.Render();
			break;
		case WM_CREATE:

		//	DwmExtendFrameIntoClientArea(hWnd, &GameGlobal::Rd.margins);
			break;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		return DefWindowProc(hWnd, msg, wParam, lParam);

	VMProtectEnd();
}
*/

//imguu windproc
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}


void LH_Rd::CreateExEWindow() {

	VMProtectBegin("CreateExEWindow");
	RECT rect;

	while (!GWindow) {
		GWindow = FindWindow(NULL, VMProtectDecryptStringW(L"Rainbow Six"));
	}

	if (GWindow != NULL) {
		GetWindowRect(GWindow, &rect);
	}
	else {
		std::cout << VMProtectDecryptStringA("Application exiting, error preapring the window. Error code: ") << GetLastError() << std::endl;
		Sleep(3000);
		ExitProcess(0);
	}

	WNDCLASSEX windowClass;

	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = GetModuleHandle(0);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	windowClass.lpszClassName = VMProtectDecryptStringW(L"WeChat");

	RegisterClassEx(&windowClass);

	ExEWindow = CreateWindowEx(WS_EX_LAYERED | WS_EX_TRANSPARENT, windowClass.lpszClassName, L"", WS_POPUP, rect.left, rect.top, ScreenWidth, ScreenHeight, NULL, NULL, windowClass.hInstance, NULL);
	SetWindowLongA(ExEWindow, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);
	//MARGINS Margin = { -1 };

//	HRESULT HR =  DwmExtendFrameIntoClientArea(ExEWindow, &Margin);



	// Transparency
	//if (0) { // Without DWM
	//	SetLayeredWindowAttributes(ExEWindow, 0, 255, LWA_ALPHA);
		//SetLayeredWindowAttributes(ExEWindow, RGB(0, 0, 0), 0, ULW_COLORKEY);
	//	return ;
	//}
	//MARGINS margin = { 0,0,1920,1080 };

	//DwmExtendFrameIntoClientArea(ExEWindow, &margin);
//	MARGINS margin = { 1, 1, 1920, 1080 };
	//SetLayeredWindowAttributes(ExEWindow, RGB(0, 0, 0), 255, LWA_ALPHA);

	MARGINS Margin = { -1 };
	DwmExtendFrameIntoClientArea(ExEWindow, &Margin);
	SetLayeredWindowAttributes(ExEWindow, RGB(0, 0, 0), 178, /*LWA_COLORKEY |*/ LWA_ALPHA);
	//SetWindowDisplayAffinity(ExEWindow, 1); 截圖


	ShowWindow(ExEWindow, SW_SHOW);

	UpdateWindow(ExEWindow);
	InitD3D(ExEWindow);

	VMProtectEnd();
}
