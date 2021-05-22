#include "Globals.h"


namespace Decryption
{
	//Updated 20/01/2021
	uintptr_t FovManager(uintptr_t address)
	{

		uint64_t encrypted = dr->RPM<uint64_t>(address + 0x120);

		uint64_t decrypted = __ROL8__(__ROL8__(encrypted, 0x3C) ^ 0x3FEE88E3FF0DC2E4, 0xA);
		return dr->RPM <uint64_t>(decrypted);

	}
	//
	uintptr_t GameManager(uintptr_t address)
	{

		return _rotl64(address + 0xB93C160FC138943A, 0x2c) - 0x63;
	}

	//Updated
	uintptr_t ProfileManager(uintptr_t address)
	{
		return (_rotl64(address, 0x2) + 0x52a0257cf75192a0) ^ 0x7d8c393dfbd75346;
	}

	//Updated
	uintptr_t EntityList(uintptr_t address)
	{
		uintptr_t pointer_encrypted = address;
		uintptr_t copy = pointer_encrypted;
		copy = _rotl64(copy, 0x2B);
		pointer_encrypted = _rotr64(pointer_encrypted, 0x15);
		pointer_encrypted |= copy;
		pointer_encrypted += 0xFFFFFFFFFFFFFF9E;
		pointer_encrypted = _rotl64(pointer_encrypted, 0xC);

		return ((((address >> 0x15) | (address << 0x2B)) + 0xFFFFFFFFFFFFFF9E) >> 0x34 |
			(((address >> 0x15) | (address << 0x2B)) + 0xFFFFFFFFFFFFFF9E) << 0xC);
	}

	//Updateds
	uintptr_t EntityHolder(uintptr_t address)
	{
		return (((address - 0xAECEA7D82999F16B) ^ 0xC) - 3);//hex 0x51315827D6660E95i64
	}

	//Updated
	uintptr_t Profile(uintptr_t address)
	{
		address = 0x95C2CB62693E67A0 + address;

		address = dr->RPM<uintptr_t>((address ^ 0x54) - 0x45);
		address = _rotl64(dr->RPM<uintptr_t>(address + 0x118), 0x2A) + 0x82570245E68CBB04;
		return address;
	}

	//Updated 20/01/2021 沒用到 不需要更新
	uintptr_t LocalPlayerController(uintptr_t address)
	{
		return (address ^ 0x76i64) - 0x44A8D271187EFD62i64 ^ 0x4E0415630446B569i64;
	}

	//Updated 20/01/2021 
	uintptr_t Playercontroller(uintptr_t address)
	{
		return (address ^ 0xBFBC0F652D65D6C7);
	}

	//Updated
	uintptr_t PlayerInfo(uintptr_t address)
	{
		return (((address - 0x16797D1B979C5DE3) ^ 0x3B) - 0x2A);;
	}

	//Updated
	uintptr_t TeamClass(uintptr_t address)
	{
		return  __ROL8__(address - 0x1B, 14) + 0x4041879CD9EED0C;
	}

	//Updated
	uint32_t TeamId(uint32_t value)
	{
		return (__ROL4__(value, 0x1B)) - 0x39B86B79; // or + 0C6479487
	}

	//Updated 20/01/2021
	uintptr_t WeaponClass(uintptr_t address)
	{
		return (address - 0x3E564CDF94EFC7C7i64 ^ 0x28) - 7;
	}

	//Updated 20/01/2021 沒用到 不一定要更新
	int32_t DecryptRoundManager(uint32_t address)
	{
		return __ROL4__(address - 0x4B701DE6, 0xF) | 1;
	}
}

namespace Encryption
{
	//Updated 20/01/2021 沒用到 不一定要更新
	uint32_t Spread(float value)
	{
		return __ROL4__((*(uint32_t*)&value - 0x7D18B017), 0x1) - 0xBAB3EF99;
	}

	// Updated 20/01/2021
	BYTE EncryptEnableRecoil(bool toggleValue)
	{
		// __ROL1__((0xC0 ^ 0x17) - 0x57, 1); <- Current decryption
		return static_cast<BYTE>(__ROR1__(toggleValue, 1) - 0xa9 ^ 0x17);
	}

	// Updated 20/01/2021
	void EncryptRecoilX(float value, uint32_t outEncryptedValue[3])
	{
		outEncryptedValue[0] = (*(uint32_t*)&value ^ 0x7E52F095) + 0x47;
		outEncryptedValue[1] = (__ROL4__(*(uint32_t*)&value, 0x9) ^ 0x6658423C) + 0x3FE3574D;
		outEncryptedValue[2] = __ROL4__((__ROL4__(*(uint32_t*)&value, 0x3) ^ 0x1B73D411), 4);

	}

	// Updated 20/01/2021
	void EncryptRecoilY(float value, uint32_t outEncryptedValue[3])
	{
		outEncryptedValue[0] = __ROL4__((__ROL4__(*(uint32_t*)&value, 0xC) ^ 0xBF9F579B), 7);
		outEncryptedValue[1] = __ROL4__((__ROL4__(*(uint32_t*)&value, 0x17) + 0x20), 7);
		outEncryptedValue[2] = __ROL4__(*(uint32_t*)&value + 0x78E60195, 0x12);
	}
}