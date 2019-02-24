// dogshit.cpp : Определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
#include <Windows.h>

struct values
{
	DWORD localPlayer;
	DWORD client;
	DWORD engine;
	BYTE flag;
}values;

struct Offsets
{
	DWORD m_bSpotted = 0x93D;
	DWORD m_fFlags = 0x104;
	DWORD m_bIsScoped = 0x390A;
	DWORD m_iGlowIndex = 0xA3F8;
	DWORD m_iCrosshairId = 0xB394;
	DWORD m_iTeamNum = 0xF4;
	DWORD m_lifeState = 0x25F;
	DWORD m_vecOrigin = 0x138;
	DWORD m_vecVelocity = 0x114;
	DWORD dwEntityList = 0x4CDB00C;
	DWORD dwForceAttack = 0x310C710;
	DWORD dwForceJump = 0x517E1C4;
	DWORD dwLocalPlayer = 0xCCA6A4;
	DWORD dwGlowObjectManager = 0x521AF50;
	DWORD m_bDormant = 0xED;
	DWORD m_hActiveWeapon = 0x2EF8;
	DWORD m_iItemDefinitionIndex = 0x2FAA;
	DWORD m_iHealth = 0x100;
}offset;

struct vector
{
	float x, y, z;
};

struct Player
{
	int Team;
	int Health;
	bool Dormant;
	vector Position;
	vector Velocity;

	/*
	Player()
	{
		Team = *(INT32*)(GetLocalPlayer() + offset.m_iTeamNum);
		Health = *(INT32*)(GetLocalPlayer() + offset.m_iHealth);
		Dormant = false;
		Position.x = *(FLOAT*)(GetLocalPlayer() + offset.m_vecOrigin);
		Position.y = *(FLOAT*)(GetLocalPlayer() + offset.m_vecOrigin + 0x4);
		Position.z = *(FLOAT*)(GetLocalPlayer() + offset.m_vecOrigin + 0x8);
		Velocity.x = *(FLOAT*)(GetLocalPlayer() + offset.m_vecVelocity);
		Velocity.y = *(FLOAT*)(GetLocalPlayer() + offset.m_vecVelocity + 0x4);
		Velocity.z = *(FLOAT*)(GetLocalPlayer() + offset.m_vecVelocity + 0x8);
	}*/

	Player(DWORD entity)
	{
		Team = *(INT32*)(entity + offset.m_iTeamNum);
		Health = *(INT32*)(entity + offset.m_iHealth);
		Dormant = *(BOOL*)(entity + offset.m_bDormant);
		Position.x = *(FLOAT*)(entity + offset.m_vecOrigin);
		Position.y = *(FLOAT*)(entity + offset.m_vecOrigin + 0x4);
		Position.z = *(FLOAT*)(entity + offset.m_vecOrigin + 0x8);
		Velocity.x = *(FLOAT*)(entity + offset.m_vecVelocity);
		Velocity.y = *(FLOAT*)(entity + offset.m_vecVelocity + 0x4);
		Velocity.z = *(FLOAT*)(entity + offset.m_vecVelocity + 0x8);
	}
};



DWORD GetLocalPlayer()
{
	return *(DWORD*)(values.client + offset.dwLocalPlayer);
}

DWORD GetGlowObject()
{
	return *(DWORD*)(values.client + offset.dwGlowObjectManager);
}

DWORD GetObjectCount()
{
	return *(INT32*)(values.client + offset.dwGlowObjectManager + 0xC);
}

DWORD GetEntity(int i)
{
	return *(DWORD*)(values.client + offset.dwEntityList + (i-1) * 0x10);
}



void Glow()
{
	DWORD GlowObject = GetGlowObject();
	DWORD objectCount = GetObjectCount();
	int i = 1;
	do
	{
		DWORD Entity = GetEntity(i);
		if (Entity != NULL)
		{
			Player MyPlayer(values.localPlayer);
			Player Player(Entity);

			DWORD GlowPtr = GlowObject;

			if (MyPlayer.Team != Player.Team)
			{
				*(BOOL*)(Entity + offset.m_bSpotted) = true;
			}
			int GlowIndex = *(INT32*)(Entity + offset.m_iGlowIndex);
			if (!Player.Dormant)
			{
				if (MyPlayer.Team == Player.Team)
				{
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x4)) = 0; //red
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x8)) = 0; //green
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0xC)) = 1; //blue
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x10)) = 0.7f; //alpha
					*(BOOL*)(GlowPtr + ((0x38 * GlowIndex) + 0x24)) = true; //render
					*(BOOL*)(GlowPtr + ((0x38 * GlowIndex) + 0x25)) = false; //render
				}
				else
				{
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x4)) = 1; //red
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x8)) = 0; //green
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0xC)) = 0; //blue
					*(FLOAT*)(GlowPtr + ((0x38 * GlowIndex) + 0x10)) = 0.9f; //alpha
					*(BOOL*)(GlowPtr + ((0x38 * GlowIndex) + 0x24)) = true; //render
					*(BOOL*)(GlowPtr + ((0x38 * GlowIndex) + 0x25)) = false; //render
				}
				
			}

		}
		i++;

	} while (i < objectCount);

}









void main()
{
	values.client = (DWORD)GetModuleHandle("client_panorama.dll");
	values.engine = (DWORD)GetModuleHandle("engine.dll");
	while (true)
	{
		values.localPlayer = GetLocalPlayer();
		Glow();
		Sleep(1);
	}
	

}







BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		DisableThreadLibraryCalls(hModule);
		CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)main, NULL, NULL, NULL);
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

