// Copyright Epic Games, Inc. All Rights Reserved.

#include "CyberInvasion.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, CyberInvasion, "CyberInvasion" );
 
int bnGameModeType = Enum_Single_GameMode;				//游戏模式，0:单人模式 1:多人模式

//uint8 maxPlayerNums = 10;								//多人模式下，最多可连接玩家数量

