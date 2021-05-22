#include <Windows.h>
#include <Psapi.h>

#include "Globals.h"

//All offset variables hardcoded
//namespace Takeoff
//namespace Quavo



LH_Mem::LH_Mem() {};



/*
template<typename T> T LH_Mem::RPM(ULONG64 address) {

	VMProtectBegin("RPM");
	//The buffer for data that is going to be read from memory
	T buffer;

	//The actual dr->RPM
	//ReadProcessMemory(GameGlobal::GameHandle, (LPCVOID)address, &buffer, sizeof(T), NULL);
	buffer = dr->RPM<T>(address);
	//Return our buffer
	return buffer;
	VMProtectEnd();
}


template<typename T> bool LH_Mem::WPM(ULONG64 address, T buffer) {
	//VMProtectBegin("WPM");
	//A couple checks to try and avoid crashing
	//These don't actually make sense, feel free to remove redundent ones
	//if (address == 0 || (LPVOID)address == nullptr || address == NULL) {
	//	return;
	//}

	//WriteProcessMemory(GameGlobal::GameHandle, (LPVOID)address, &buffer, sizeof(buffer), NULL);
	dr->WPM<T>(address, buffer);
	//VMProtectEnd();
}
bool LH_Mem::WBT(DWORD_PTR address, byte* value, const int32_t w_readSize) {
	VMProtectBegin("WPM");
	//A couple checks to try and avoid crashing
	//These don't actually make sense, feel free to remove redundent ones
	//if (address == 0 || (LPVOID)address == nullptr || address == NULL) {
		//return 0;
	//}

	//WriteProcessMemory(GameGlobal::GameHandle, (LPVOID)address, &buffer, sizeof(buffer), NULL);
	dr->WBT( address,value,  w_readSize);
	return 0;
	VMProtectEnd();
}
*/





std::string LH_Mem::RPMString(ULONG64 address) {
	VMProtectBegin("RPMString");
	//Make a char array of 20 bytes
	char name[20];

	//The actual dr->RPM
	//ReadProcessMemory(GameGlobal::GameHandle, (LPCVOID)address, &name, sizeof(name), NULL);

	dr->ReadStr(address, name, sizeof(name));
	//Add each char to our string
	//While also checking for a null terminator to end the string
	std::string nameString;

	for (int i = 0; i < sizeof(name); i++) {
		if (name[i] == 0)
			break;
		else
			nameString += name[i];
	}

	return nameString;
	VMProtectEnd();
}

void LH_Mem::UpdateAddresses() {
	VMProtectBegin("UpdateAddresses");
	//Game manager pointer from games base address + the GameManager offset
	pGameManager = Decryption::GameManager(dr->RPM<DWORD_PTR>(GameGlobal::BaseAddress + Offset::GameManager));



	//fov沒recoil用不到
	pFovManager =  dr->RPM<ULONG64>(GameGlobal::BaseAddress + Offset::FovManager);
	pFovManager = Decryption::FovManager(pFovManager);


	//Entity list pointer from the GameManager + EntityList offset
	pEntityList = dr->RPM<DWORD_PTR>(pGameManager + Offset::EntityList) ;
	pEntityList = Decryption::EntityList(pEntityList);
	//pEntityList = ((pEntityList ^ 0xEC2581B1EC5CC398) + 0x71B65F65B607DE6F) ^ 0xC6554B199E494084;
	

	
	//Renderer pointer from games base address + Renderer offset
	//ProfileManager = dr->RPM<DWORD_PTR>(GameGlobal::BaseAddress + Offset::ProfileManager);
	ProfileManager = (Decryption::ProfileManager(dr->RPM<uint64_t>(GameGlobal::BaseAddress + Offset::ProfileManager)));


	/*old method
	//Game Renderer pointer from Renderer + GameRenderer offset
	pGameRender = dr->RPM<DWORD_PTR>(ProfileManager + Offset::GameRenderer);

	//EngineLink pointer from GameRenderer + EngineLink offset
	pEngineLink = dr->RPM<DWORD_PTR>(pGameRender + Offset::EngineLink);
	//Engine pointer from EngineLink + Engine offset
	pEngine = dr->RPM<DWORD_PTR>(pEngineLink + Offset::Engine);
	//Camera pointer from Engine + Camera offset
	*/

	 pEngine = dr->RPM<uintptr_t>(ProfileManager + Offset::Engine);
	pCamera = dr->RPM<DWORD_PTR>(pEngine + Offset::Camera);
	VMProtectEnd();
}

DWORD_PTR LH_Mem::GetEntity2(int i) {
       VMProtectBegin("GetEntity2");




	DWORD_PTR entityBase = dr->RPM<DWORD_PTR>(pEntityList + (i * Offset::Entity));
	//return dr->RPM<DWORD_PTR>(entityBase + Offset::EntityRef);;
	return entityBase;//RPM<DWORD_PTR>(entityBase + Offset::EntityRef);
	VMProtectEnd();
}

DWORD_PTR LH_Mem::GetEntity(int i) {
	VMProtectBegin("GetEntity");




	DWORD_PTR entityBase = dr->RPM<DWORD_PTR>(pEntityList + (i * Offset::Entity));
	DWORD_PTR entityHandle = dr->RPM<DWORD_PTR>(entityBase + Offset::EntityHolder);// +0x50是pawn



	return  Decryption::EntityHolder(entityHandle);
	//return  dr->RPM<DWORD_PTR>( dr->RP M<DWORD_PTR>(entityBase + Offset::EntityRef) + Offset::EntityRef2);
	VMProtectEnd();
}

DWORD_PTR LH_Mem::GetLocalEntity2(ULONG64 entity) {
	VMProtectBegin("Entity");
	//Loop through the first 12

New:



	for (int i = 0; i < 50; i++) {


		
		DWORD_PTR Entity = GameGlobal::Mem.GetEntity(i);

	
		LocalEntity2 = GameGlobal::Mem.GetEntity2(i);
		LocalEntity = Entity;//GameGlobal::Mem.GetLocalEntity(LocalEntity2);
		
		LocalPlayer = GameGlobal::Mem.GetAllEntityInfo(LocalEntity, LocalEntity2);
	
		
		//	//check it against our local name
		if (strcmp(LocalPlayer.Name.c_str(), GameGlobal::LocalName.c_str()) == 0) {
		
		
			
		
			
			return LocalEntity2;//找到跳出
		
		}
	
	}

	

	//goto New;//沒找到就重找
	return 0;

	//return GetEntity2(0);
	VMProtectEnd();
}


/*用不太到
DWORD_PTR LH_Mem::GetLocalEntity(ULONG64 entity) {
	VMProtectBegin("GetLocalEntity");

	DWORD_PTR entityHandle = dr->RPM<DWORD_PTR>(entity + Offset::EntityRef);

	return   Decryption::EntityHolder(entityHandle);
	VMProtectEnd();
}*/

DWORD LH_Mem::GetEntityHealth(DWORD_PTR entity) {
	VMProtectBegin("GetEntityHealth");
	//Entity info pointer from the Entity


	DWORD_PTR EntityInfo = dr->RPM<DWORD_PTR>(entity + Offset::EntityInfo);
	//Main component pointer from the entity info
	DWORD_PTR MainComponent = dr->RPM<DWORD_PTR>(EntityInfo + Offset::MainComponent);
	//Child component pointer form the main component
	DWORD_PTR ChildComponent = dr->RPM<DWORD_PTR>(MainComponent + Offset::ChildComponent);

	//Finally health from the child component
	return dr->RPM<int32_t>(ChildComponent + Offset::Health);
	VMProtectEnd();
}

Vector3 LH_Mem::GetEntityFeetPosition(DWORD_PTR entity) {
	VMProtectBegin("GetEntityFeetPosition");
	//We get the feet position straight from the entity
	return dr->RPM<Vector3>(entity + Offset::FeetPosition);
	VMProtectEnd();
}

Vector3 LH_Mem::GetEntityHeadPosition(DWORD_PTR entity) {
	VMProtectBegin("GetEntityHeadPosition");
	//We get the head position straight from the entity
	return dr->RPM<Vector3>(entity + Offset::HeadPosition);
	VMProtectEnd();
}

Vector3 LH_Mem::GetEntitChestPosition(DWORD_PTR entity) {
	VMProtectBegin("GetEntitChestPosition");
	//We get the head position straight from the entity
	return dr->RPM<Vector3>(entity + Offset::ChestPosition);
	VMProtectEnd();
}
//666
uint64_t LH_Mem::get_localplayer()//666
{
	uintptr_t chain = NULL;
	auto profile_manager = ProfileManager;
	if (!(chain = dr->RPM<uintptr_t>(profile_manager + Offset::LocalPlayer))) return 1;//3
	chain = Decryption::Profile(chain);
	/*if (!(chain = dr->RPM<uintptr_t>(chain + 0x0))) return 1;
	if (!(chain = dr->RPM<uintptr_t>(chain + 0x28))) return 1;
	chain += 0xBADD3F9015EF5524;
	chain ^= 0x23;
	chain += 0xB0EB703322DA824D;
	//(PVOID64)(temp + 0x50)  //LocalEntity*/
	return  (Decryption::EntityHolder(dr->RPM<uint64_t>(chain + Offset::EntityHolder)));
}
//666
uint8_t LH_Mem::GetEntityTeamId(DWORD_PTR entity) {
	VMProtectBegin("GetEntityTeamId");
	//Team id comes from player info
	entity = GameGlobal::Mem.GetPlayerController(entity);
	DWORD_PTR playerInfo = dr->RPM<DWORD_PTR>(entity + Offset::PlayerInfo);
	//We have to derefrnce it as 0x0
	DWORD_PTR playerInfoD = Decryption::PlayerInfo(playerInfo);// dr->RPM<DWORD_PTR>(playerInfo + Offset::PlayerInfoDeref);
	auto  TeamClass = dr->RPM<DWORD_PTR>(playerInfoD + Offset::PlayerTeamClass);
	TeamClass = Decryption::TeamClass(TeamClass);


	auto teamIdEncrypted = dr->RPM<uint32_t>(TeamClass + Offset::PlayerTeamId);
	if (teamIdEncrypted == NULL)
		return 0xff;

	return Decryption::TeamId(teamIdEncrypted);

	VMProtectEnd();
}

//(__ROL8__(*(uintptr_t *)(address + 0x50), 1) - 0x53) ^ 0x84B4E3BD4F9014AF; 加速用
uint64_t LH_Mem::GetPlayerController(uint64_t entity) {
	
	//entity = dr->RPM<uint64_t>(entity + Offset::EntityRef2);

	//entity= ((entity + 0xEA8197B6897F684D) ^ 0x57) + 0x691F96E37785CCD0;
	//return entity;
	return Decryption::Playercontroller(dr->RPM<uintptr_t>(entity + Offset::EntityRef2));

	
}
//666
std::string LH_Mem::GetEntityPlayerName(DWORD_PTR entity) {
	VMProtectBegin("GetEntityPlayerName");
	entity = GameGlobal::Mem.GetPlayerController(entity);
	DWORD_PTR playerInfo = dr->RPM<DWORD_PTR>(entity + Offset::PlayerInfo);
	DWORD_PTR playerInfoD = Decryption::PlayerInfo(playerInfo);// dr->RPM<DWORD_PTR>(playerInfo + Offset::PlayerInfoDeref);

	DWORD_PTR pname = dr->RPM<uintptr_t>(playerInfoD + Offset::PlayerName);



	//偏移查找
	/*std::string a;
	for (DWORD i = 0x140; i < 0x300; i++) {
		a = GameGlobal::Mem.RPMString(dr->RPM<DWORD_PTR>(playerInfoD + i) + 0x0);
		printf("%x \n", i);
		if (strcmp(a.c_str(), GameGlobal::LocalName.c_str()) == 0)
			system("pause");
	}
	return a;*/
	return  GameGlobal::Mem.RPMString((pname+ 0x0));
	
	VMProtectEnd();
}



Vector3 LH_Mem::GetEntityBone(ULONG64 Entity, __int64 BoneID)
{
	__m128 Output;



	__int64 pBonesChain1 = dr->RPM<__int64>(Entity + Offset::pBonesChain1);
	__int64 pBonesChain2 = dr->RPM<__int64>(pBonesChain1 + Offset::pBonesChain2); 
	__int64 pBones = dr->RPM<__int64>(pBonesChain2 + Offset::pBones); 
	__int64 pBonesData = dr->RPM<__int64>(pBones + Offset::pBonesData);
	
	__m128 BoneInfo = dr->RPM<__m128>((0x20 * BoneID) + pBonesData);

	GameGlobal::Mem.TransformsCalculation(pBones, &Output, &BoneInfo);
	
	return  Vector3(Output.m128_f32[0], Output.m128_f32[1], Output.m128_f32[2]);
}

//int nowd = 0x10;
void LH_Mem::DrawSkeleton(ULONG64 Entity, ULONG64 PlayerController, BYTE CTU, BYTE OP,std::string ThisOperatorName) {





	//std::string rolse = GameGlobal::Mem.Roles(Entity);
	/*
	if (GetAsyncKeyState(VK_ADD) & 0x8000) {
		Sleep(50);
		nowd += 0x1;
		std::cout << "nowd: " << nowd << std::endl;
	}*/

	
	ULONG64 PlayerInfo = Decryption::PlayerInfo(dr->RPM<ULONG64>(PlayerController + Offset::PlayerInfo));
	//PlayerInfo OR 0有偏移的方法
	if (0)
	{
		

		if (ThisOperatorName != "") {//|| ThisOperatorName.c_str() != NULL 
			if (CTU >= 0 && OP >= 0 && CTU != 255 && OP != 255) {
				/*
				if (CTU == 24 && OP == 1) {//IANA
					
					for (int i = 0x0; i <= nowd; i++) {
						
						Vector3 aa = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, i));
						GameGlobal::Rd.DrawString(aa.x, aa.y, ImGui_ARGB(255, 0, 255, 255), GameGlobal::Rd.d3Font, "0x%x", i);
					}

				}
		      */

				Vector3 head = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][0]));
				Vector3 high_neck = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][1]));
				Vector3 low_neck = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][2]));
				Vector3 r_shoulder = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][3]));
				Vector3 l_shoulder = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][4]));
				Vector3 r_elbow = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][5]));
				Vector3 l_elbow = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][6]));
				Vector3 r_hand = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][7]));
				Vector3 l_hand = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][8]));
				Vector3 high_stomach = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][9]));
				Vector3 low_stomach = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][10]));
				Vector3 pelvis = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][11]));
				Vector3 r_knee = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][12]));
				Vector3 l_knee = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][13]));
				Vector3 r_foot = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][14]));
				Vector3 l_foot = GameGlobal::Mem.WorldToScreen(GetEntityBone(Entity, BoneId[CTU][OP][15]));

				GameGlobal::Rd.DrawLine(head.x, head.y, high_neck.x, high_neck.y, ImGui_ARGB(255, 255, 255, 255));//繷
				GameGlobal::Rd.DrawLine(high_neck.x, high_neck.y, low_neck.x, low_neck.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(low_neck.x, low_neck.y, r_shoulder.x, r_shoulder.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(low_neck.x, low_neck.y, l_shoulder.x, l_shoulder.y, ImGui_ARGB(255, 255, 255, 255));//

				GameGlobal::Rd.DrawLine(r_shoulder.x, r_shoulder.y, r_elbow.x, r_elbow.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(l_shoulder.x, l_shoulder.y, l_elbow.x, l_elbow.y, ImGui_ARGB(255, 255, 255, 255));//

				GameGlobal::Rd.DrawLine(r_elbow.x, r_elbow.y, r_hand.x, r_hand.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(l_elbow.x, l_elbow.y, l_hand.x, l_hand.y, ImGui_ARGB(255, 255, 255, 255));//

				GameGlobal::Rd.DrawLine(low_neck.x, low_neck.y, high_stomach.x, high_stomach.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(high_stomach.x, high_stomach.y, low_stomach.x, low_stomach.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(low_stomach.x, low_stomach.y, pelvis.x, pelvis.y, ImGui_ARGB(255, 255, 255, 255));//

				GameGlobal::Rd.DrawLine(pelvis.x, pelvis.y, r_knee.x, r_knee.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(pelvis.x, pelvis.y, l_knee.x, l_knee.y, ImGui_ARGB(255, 255, 255, 255));//

				GameGlobal::Rd.DrawLine(r_knee.x, r_knee.y, r_foot.x, r_foot.y, ImGui_ARGB(255, 255, 255, 255));//
				GameGlobal::Rd.DrawLine(l_knee.x, l_knee.y, l_foot.x, l_foot.y, ImGui_ARGB(255, 255, 255, 255));//
			}
		}
	}
}

std::string LH_Mem::Roles(ULONG64 entity2) {

	ULONG64 PlayerInfo = dr->RPM<ULONG64>(entity2 + Offset::PlayerInfo);
	BYTE CTU = dr->RPM<ULONG64>(PlayerInfo + Offset::CTU);
	BYTE OP = dr->RPM<ULONG64>(PlayerInfo + Offset::OP);

	std::string ThisOperatorName = OperatorName[CTU][OP];

	return ThisOperatorName;
}

int LH_Mem::GetPlayerCount() {
	VMProtectBegin("GetPlayerCount");
	int count;
	//count = dr->RPM<int>(pGameManager + Offset::EntityCount) ^ 0x681C75805DE8EEA2;
	int  count2 = dr->RPM<int >(GameGlobal::Mem.pGameManager + Offset::EntityList) + 0x681C75805DE8EEA2 & 0xFFFF;
	
	return count2;//count & 0x3fffffff;
	VMProtectEnd();
}

PlayerInfo LH_Mem::GetAllEntityInfo(DWORD_PTR entity, DWORD_PTR entity2) {
	VMProtectBegin("GetAllEntityInfo");
	PlayerInfo p;


	p.Health = GetEntityHealth(entity);
	p.Name = GetEntityPlayerName(entity);
	p.Position = GetEntityFeetPosition(entity);
	p.w2s = WorldToScreen(p.Position);
	p.Head = GetEntityHeadPosition(entity);
	p.w2sHead = WorldToScreen(p.Head);

	p.Chest = GetEntitChestPosition(entity);
	p.w2sChest = WorldToScreen(p.Chest);
	p.TeamId = GetEntityTeamId(entity);

	return p;
	VMProtectEnd();
}


PlayerInfo LH_Mem::GetAllEntityInfo2(DWORD_PTR entity, DWORD_PTR entity2) {

	PlayerInfo p;

	p.Health = GetEntityHealth(entity2);
	p.Name = GetEntityPlayerName(entity2);
	p.Position = GetEntityFeetPosition(entity);
	p.w2s = WorldToScreen(p.Position);
	p.Head = GetEntityHeadPosition(entity);
	p.w2sHead = WorldToScreen(p.Head);

	p.Chest = GetEntitChestPosition(entity);
	p.w2sChest = WorldToScreen(p.Chest);
	p.TeamId = GetEntityTeamId(entity2);

	return p;
	
}



/*
void LH_Mem::ZeroRecoilSpread(DWORD_PTR entity) {
	DWORD_PTR entityInfo = dr->RPM<DWORD_PTR>(entity + Offset::EntityInfo);
	DWORD_PTR mainComp = dr->RPM<DWORD_PTR>(entityInfo + Offset::MainComponent);
	DWORD_PTR weaponComp = dr->RPM<DWORD_PTR>(mainComp + Offset::weaponComp);
	DWORD_PTR weaponProc = dr->RPM<DWORD_PTR>(weaponComp + Offset::WeaponProcessor);
	DWORD_PTR weapon = dr->RPM<DWORD_PTR>(weaponProc + Offset::Weapon);
	DWORD_PTR weaponInfo = dr->RPM<DWORD_PTR>(weapon + Offset::WeaponInfo);

	float recoil = dr->RPM<float>(weaponInfo + Offset::Recoil);
	float recoil2 = dr->RPM<float>(weaponInfo + Offset::Recoil2);
	Vector4 recoil3 = dr->RPM<Vector4>(weaponInfo + Offset::Recoil3);
	Vector2 adsRecoil = dr->RPM<Vector2>(weaponInfo + Offset::AdsRecoil);

	float spread = dr->RPM<float>(weaponInfo + Offset::Spread);

	//Make sure values aren't already 0 before we set them to 0, avoids wasting some time and writes

	if (recoil != 0) {
		WPM<float>(weaponInfo + Offset::Recoil, 0);
	}

	if (recoil2 != 0) {
		WPM<float>(weaponInfo + Offset::Recoil2, 0);
	}

	if (!(recoil3.x == 0 && recoil3.y == 0 && recoil3.z == 0 && recoil3.w == 0)) {
		WPM<Vector4>(weaponInfo + Offset::Recoil3, Vector4(0, 0, 0, 0));
	}

	if (!(adsRecoil.x == 0 && adsRecoil.y == 0)) {
		WPM<Vector2>(weaponInfo + Offset::AdsRecoil, Vector2(0, 0));
	}

	if (spread != 0) {
		WPM<float>(weaponInfo + Offset::Spread, 0);
	}

}
*/


Vector3 LH_Mem::GetViewTranslation() {
	VMProtectBegin("GetViewTranslation");
	//View translation comes straight from the camera
	return dr->RPM<Vector3>(pCamera + Offset::ViewTranslastion);
	VMProtectEnd();
}

Vector3 LH_Mem::GetViewRight() {
	VMProtectBegin("GetViewRight");
	//View right comes directly from the camera
	return dr->RPM<Vector3>(pCamera + Offset::ViewRight);
	VMProtectEnd();
}

Vector3 LH_Mem::GetViewUp() {
	VMProtectBegin("GetViewUp");
	//View up comes directly from the camera
	return dr->RPM<Vector3>(pCamera + Offset::ViewUp);
	VMProtectEnd();
}

Vector3 LH_Mem::GetViewForward() {
	VMProtectBegin("GetViewForward");
	//View forward comes directly from the camera
	return dr->RPM<Vector3>(pCamera + Offset::ViewForward);
	VMProtectEnd();
}
/*
Vector2 LH_Mem::GetFOV() {
	VMProtectBegin("GetFOV");
	//FOV comes directly from the camera
	Vector2 fov = (dr->RPM<Vector2>(pCamera + Offset::FOVX));
	fov.x = std::abs(fov.x);
	fov.y = std::abs(fov.y);
	return fov;
	VMProtectEnd();
}*/

float LH_Mem::GetFOVX() {
	VMProtectBegin("GetFOVY");
	//FOV comes directly from the camera
	return std::abs(dr->RPM<float>(pCamera + Offset::FOVX));
	VMProtectEnd();
}
float LH_Mem::GetFOVY() {
	VMProtectBegin("GetFOVY");
	//FOV comes directly from the camera
	return std::abs(dr->RPM<float>(pCamera + Offset::FOVY));
	VMProtectEnd();
}

Vector3 LH_Mem::WorldToScreen(Vector3 position) {
	VMProtectBegin("WorldToScreen");
	Vector3 temp = position - GetViewTranslation();

	float x = temp.Dot(GetViewRight());
	float y = temp.Dot(GetViewUp());
	float z = temp.Dot(GetViewForward() * -1);
	
	return Vector3((GameGlobal::Rd.ScreenWidth / 2) * (1 + x / GetFOVX() / z), (GameGlobal::Rd.ScreenHeight / 2) * (1 - y / GetFOVY() / z), z);
	VMProtectEnd();
}

/*
Vector3 LH_Mem::CalcAngle(Vector3 enemypos, Vector3 camerapos)
{
	float r2d = 57.2957795131f;

	Vector3 dir = enemypos - camerapos;

	float x = asin(dir.z / dir.Length()) * r2d;
	float z = atan(dir.y / dir.x) * r2d;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 180.0f) x -= 360.f;
	else if (x < -180.0f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);

}*/


void LH_Mem::TransformsCalculation(__int64 pBones, __m128* ResultPosition, __m128* BoneInfo)
{
	__m128 v5; // xmm2
	__m128 v6; // xmm3
	__m128 v7; // xmm0
	__m128 v8; // xmm4
	__m128 v9; // xmm1

	__m128 v10 = { 0.500f, 0.500f, 0.500f, 0.500f };
	__m128 v11 = { 2.000f, 2.000f, 2.000f, 0.000f };

	__m128 v12 = dr->RPM<__m128>(pBones);
	__m128 v13 = dr->RPM<__m128>(pBones + 0x10);

	v5 = v13;

	v6 = _mm_mul_ps(*(__m128*)BoneInfo, v5);
	v6.m128_f32[0] = v6.m128_f32[0]
		+ (float)(_mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0x55)) + _mm_cvtss_f32(_mm_shuffle_ps(v6, v6, 0xAA)));
	v7 = _mm_shuffle_ps(v13, v5, 0xFF);
	v8 = _mm_sub_ps(
		_mm_mul_ps(_mm_shuffle_ps(v5, v5, 0xD2), *(__m128*)BoneInfo),
		_mm_mul_ps(_mm_shuffle_ps(*(__m128*)BoneInfo, *(__m128*)BoneInfo, 0xD2), v5));
	v9 = _mm_shuffle_ps(v12, v12, 0x93);
	v9.m128_f32[0] = 0.0;
	*(__m128*)ResultPosition = _mm_add_ps(
		_mm_shuffle_ps(v9, v9, 0x39),
		_mm_mul_ps(
			_mm_add_ps(
				_mm_add_ps(
					_mm_mul_ps(_mm_shuffle_ps(v8, v8, 0xD2), v7),
					_mm_mul_ps(_mm_shuffle_ps(v6, v6, 0), v5)),
				_mm_mul_ps(
					_mm_sub_ps(_mm_mul_ps(v7, v7), (__m128)v10),
					*(__m128*)BoneInfo)),
					(__m128)v11));
}


Vector4 LH_Mem::CreateFromYawPitchRoll(float yaw, float pitch, float roll)
{
	Vector4 result;
	float cy = cos(yaw * 0.5);
	float sy = sin(yaw * 0.5);
	float cr = cos(roll * 0.5);
	float sr = sin(roll * 0.5);
	float cp = cos(pitch * 0.5);
	float sp = sin(pitch * 0.5);


	//constexpr auto sp = 0.f;
	//constexpr auto cp = 1.f;
	result.w = cy * cr * cp + sy * sr * sp;
	result.x = cy * sr * cp - sy * cr * sp;
	result.y = cy * cr * sp + sy * sr * cp;
	result.z = sy * cr * cp - cy * sr * sp;

	return result;
}

void LH_Mem::set_angles(uint64_t local_player, Vector3& angles)
{
	float d2r = 0.01745329251f;
	Vector4 new_angles = CreateFromYawPitchRoll(angles.z * d2r, 0.f, angles.x * d2r);

	uint64_t pSkeleton = local_player;//RPM<uint64_t>(local_player + Offset::EntityRef);
//	std::cout << "hey" << std::endl;
	if (pSkeleton)
	{
		//std::cout << "sucess" << std::endl;
		uint64_t viewAngle2 = dr->RPM<uint64_t>(pSkeleton + 0x11A0);
		dr->WPM<Vector4>(viewAngle2 + 0x20, new_angles);//0xc0 0x20
	}
}


Vector3  LH_Mem::calculate_angle(Vector3 translation, Vector3 position) {
	auto delta = translation - position;

	auto y = -(std::asin(delta.z / translation.Distance(position))) * 57.2957795131f;
	auto x = std::atan2(delta.y, delta.x) * 57.2957795131f;

	return Vector3(x + 90.f, y, 0.f);

}

Vector3  LH_Mem::calculate_euler(Vector4 quaternion) {
	return Vector3(std::atan2(2.f * (quaternion.w * quaternion.z + quaternion.x * quaternion.y), (1.f - 2.f * (quaternion.y * quaternion.y + quaternion.z * quaternion.z))) * 57.2957795131f,
		std::atan2(2.f * (quaternion.w * quaternion.x + quaternion.y * quaternion.z), (1.f - 2.f * (quaternion.x * quaternion.x + quaternion.y * quaternion.y))) * 57.2957795131f,
		0.f);
}
Vector3  LH_Mem::CalcAngle(Vector3 enemypos, Vector3 camerapos)
{
	float r2d = 57.2957795131f;

	Vector3 dir = enemypos - camerapos;

	float x = asin(dir.z / dir.Length()) * r2d;
	float z = atan(dir.y / dir.x) * r2d;

	if (dir.x >= 0.f) z += 180.f;
	if (x > 180.0f) x -= 360.f;
	else if (x < -180.0f) x += 360.f;

	return Vector3(x, 0.f, z + 90.f);

}
void  LH_Mem::set_viewAngle(ULONG64 entity, ULONG64 entity2, Vector3 angles, bool open) {
	
	float d2r = 0.01745329251f;
	Vector4 new_angles = GameGlobal::Mem.CreateFromYawPitchRoll(angles.z * d2r, 0.f, angles.x * d2r);
	auto r1 = dr->RPM<ULONG64>(entity2 + 0x20);
	auto r2 = dr->RPM<ULONG64>(r1 + 0x1270);//0x1200
	//auto r3 = dr->RPM<ULONG64>(r2 + 0xc0);//0x1200 c0
	auto r4 = dr->RPM<Vector4>(r2 + 0xC0);//0x1200 c0
	//std::cout << "X    " << new_angles.x << "    Y    " << new_angles.y << "    Z    " << new_angles.z << std::endl;
	//std::cout << std::hex << "r2 " << r2 << std::endl;
	dr->WPMP<Vector4>(r2 + 0x134, new_angles);
	dr->WPMP<Vector4>(r2 + 0xc0, new_angles);



	//std::cout << "aX    " << r4.x << "    Y    " << r4.y << "    Z    " << r4.z << std::endl;

	//std::cout << "    AX    " << r4.x << "    AY    " << r4.y << "    AZ    " << r4.z << std::endl;

	

	//WeaponManager = LocalPlayer + 0x90 + 0xC8 + 0x278
	//GunAngles = WeaponManager + 0x118 (Vector3 write delta between original angles and wanted angles)

	//auto r1 = dr->RPM<ULONG64>(entity + 0x20);
	//auto r2 = dr->RPM<ULONG64>(entity + 0x1200);//0x1200
	//OR
	/*if (entity2 == 0x0 )
		return;
	auto r1 = dr->RPM<ULONG64>(entity2 + 0x20);
	auto r2 = dr->RPM<ULONG64>(r1 + 0x1200);//0x1200


	auto r4 = dr->RPM<Vector4>(r2 + 0xC0);//0x1200 c0
	float d2r = 0.01745329251f;
	Vector4 new_angles = GameGlobal::Mem.CreateFromYawPitchRoll(angles.z * d2r, 0.f, angles.x * d2r);

	ULONG64 WeaponManager = dr->RPM<ULONG64>(entity2 + 0x90);
	WeaponManager = dr->RPM<ULONG64>(WeaponManager + 0xC8);
	WeaponManager = dr->RPM<ULONG64>(WeaponManager + 0x278);
	Vector3 old = dr->RPM<Vector3>(WeaponManager + 0x118);

	// std::cout << "aX    " << old.x << "    Y    " << old.y << "    Z    " << old.z << std::endl;
	auto new_angle = (angles - calculate_euler(r4)) * 0.01745329251f;
	Vector3 old_angle = { 0,0,0 };
	//old.x += 10;
	//old.y += 10;
  //  old.z += 10;
	dr->WPMP<Vector3>(WeaponManager + 0x118, (open == true) ? new_angle : old_angle);

	*/
}