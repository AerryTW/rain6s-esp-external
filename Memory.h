#pragma once
#include <Windows.h>
#include <string>
#include <xmmintrin.h>
#include <vector>
#include "EngineFunction.h"





//Struct to hold player info
struct PlayerInfo {
	DWORD		Health;
	Vector3		Position;
	Vector3		w2s; //Feet
	Vector3		Head;
	Vector3		w2sHead;
	Vector3     Chest;
	Vector3     w2sChest;
	uint8_t		TeamId;
	std::string	Name;
};


namespace AimBotInfo {



	std::vector<int> fov;

	std::vector<int> Distance;

	std::vector<ULONG64> Entity;

	std::vector<ULONG64> BestEntity;

	std::vector<Vector3> ShotPos;

	std::vector<int> Health;

};

//Memory(定義功能)   
//Global(局部宣告)
class LH_Mem {
	/*
private:
	//Game
	DWORD_PTR pGameManager;
	DWORD_PTR pEntityList;

	//Camera
	DWORD_PTR ProfileManager;
	DWORD_PTR pGameRender;
	DWORD_PTR pEngineLink;
	DWORD_PTR pEngine;
	DWORD_PTR pCamera;
	*/
public:

	DWORD_PTR pGameManager;
	DWORD_PTR pFovManager;
	DWORD_PTR pEntityList;

	//Camera
	DWORD_PTR ProfileManager;
	DWORD_PTR pGameRender;
	DWORD_PTR pEngineLink;
	DWORD_PTR pEngine;
	DWORD_PTR pCamera;

	//Hard coded display size cause lazy
	//int displayWidth = 1920;
	//int displayHeight = 1080;

	LH_Mem();

	//def driver

	//Method we use to ReadProcessMemory
	//Templated so we can read any type with the same function
	//Only param is the address to read
	//Returns whatever is read from that address
	//template<typename T> T RPM(ULONG64 address);

	//Method we use to WriteProcessMemory
	//Templated so we can write any type with the same function
	//2 params, the address to write to and a buffer to write from
	//template<typename T> bool WPM(ULONG64 address, T buffer);
	//bool WBT(DWORD_PTR address, byte* value, const int32_t w_readSize);

	//Method to read char arrays from memory
	//I split this into a seperate method cause my code was ran from a windows process which requires more checks
	//Pretty much just used for reading player names so far
	std::string RPMString(ULONG64 address);

	//Simple method to update all top-level addresses
	//Just in case something changes
	void UpdateAddresses();

	//Method to get i entity from the entity list
	//Just doest EntityList] + i * 0x0008
	DWORD_PTR GetEntity(int i);
	DWORD_PTR GetEntity2(int i);
	//method to return a pointer to the local entity
	DWORD_PTR GetLocalEntity2(ULONG64 entity);
	DWORD_PTR GetLocalEntity(ULONG64 entity);

	//Method to get an entity's health
	//Takes pointer to an entity as param
	//Returns health value as a DWORD (120 max health for pvp cause downed state)
	DWORD GetEntityHealth(DWORD_PTR entity);

	//Method to get entity's feet position
	//Takes pointer to an entity as param
	//Returns a Vector3 of the entity's 3D coordinates at their feet
	Vector3 GetEntityFeetPosition(DWORD_PTR entity);

	//Method to get an entity's head position
	//Takes pointer to an entity as param
	//Retunrs Vector3 of the entity's 3D coordiantes at their head
	Vector3 GetEntityHeadPosition(DWORD_PTR entity);

	Vector3 GetEntitChestPosition(DWORD_PTR entity);

	//Method to get an entity's name
	std::string GetEntityPlayerName(DWORD_PTR entity);

	//playrcontrol
	uint64_t GetPlayerController(uint64_t entity);
	//getloclpalyer
	uint64_t get_localplayer();;
	//Method to get an entities team id
	uint8_t GetEntityTeamId(DWORD_PTR entity2);

	//SK
	void DrawSkeleton(ULONG64 Entity, ULONG64 PlayerController, BYTE CTU, BYTE OP, std::string ThisOperatorName);
	Vector3 GetEntityBone(ULONG64 Entity, __int64 BoneID);

	//EntityCount
	int GetPlayerCount();

	//Method that gathers all the possible info about a player
	//Returns a PlayerInfo struct
	//Makes life easier
	//If you think this is a performance issues then fix it, but it really isn't
	PlayerInfo GetAllEntityInfo(DWORD_PTR entity, DWORD_PTR entity2);
	PlayerInfo GetAllEntityInfo2(DWORD_PTR entity, DWORD_PTR entity2);
	//GetRoles
	std::string Roles(ULONG64 entity2);
	//Method to remove recoil and spread from the current weapon
	void ZeroRecoilSpread(DWORD_PTR entity);

	//Method to get your view translation
	//Returns Vector3 of the view translation
	//Used for World2Screen stuff
	Vector3 GetViewTranslation();

	//Method to get your view right
	//Returns Vector3 of your view right
	//Used for World2Screen stuff
	Vector3 GetViewRight();

	//Method to get your view up
	//Returns Vector3 of your view up
	//USed for Wolrd2Screen stuff
	Vector3 GetViewUp();

	//Method to get your view forward
	//Returns Vector3 of your view forward
	//Used for World2Screen stuff
	Vector3 GetViewForward();

	//Method to get your current FOV X
	//Returns float of your FOV on the X axis
	//Used for Wolrd2Screen stuff
	//Vector2 GetFOV();

	//Method to get your current FOV Y
	//Returns float of your FOV on the Y axis
	//Used for World2Screen stuff
	float GetFOVY();
	float GetFOVX();
	//World 2 screen function
	//Translates 3d coordinates to 2d screen positions
	//Use that screen position to draw stuff
	Vector3 WorldToScreen(Vector3 position);


	//Vector3 CalcAngle(Vector3 enemypos, Vector3 camerapos);

	void TransformsCalculation(__int64 pBones, __m128* ResultPosition, __m128* BoneInfo);
	Vector4 CreateFromYawPitchRoll(float yaw, float pitch, float roll);
	void set_angles(uint64_t local_player, Vector3& angles);

	void set_viewAngle(ULONG64 entity, ULONG64 entity2, Vector3 angles, bool open);
//aim math
	Vector3 calculate_angle(Vector3 translation, Vector3 position);
    Vector3 calculate_euler(Vector4 quaternion);
    Vector3 CalcAngle(Vector3 enemypos, Vector3 camerapos);

};