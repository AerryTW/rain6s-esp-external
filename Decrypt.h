#pragma once


namespace Decryption
{
	
	    uintptr_t FovManager(uintptr_t address);

		uintptr_t GameManager(uintptr_t address);

		uintptr_t ProfileManager(uintptr_t address);

		uintptr_t EntityList(uintptr_t address);

		uintptr_t EntityHolder(uintptr_t address);

		uintptr_t Profile(uintptr_t address);
		
		uintptr_t LocalPlayerController(uintptr_t address);

		uintptr_t Playercontroller(uintptr_t address);

		uintptr_t PlayerInfo(uintptr_t address);

		uintptr_t TeamClass(uintptr_t address);

		uint32_t TeamId(uint32_t value);

		uintptr_t WeaponClass(uintptr_t address);

		int32_t DecryptRoundManager(uint32_t address);
	
}

namespace Encryption
{

	uint32_t Spread(float value);

	BYTE EncryptEnableRecoil(bool toggleValue);

	void EncryptRecoilX(float value, uint32_t outEncryptedValue[3]);


	void EncryptRecoilY(float value, uint32_t outEncryptedValue[3]);
}