#pragma once

#include "skse64/GameData.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameTypes.h"
#include "skse64/GameMenus.h"
#include "skse64/ScaleformMovie.h"
#include "skse64/PapyrusEvents.h"
#include <string.h>

using namespace std;

extern string g_currentMenu;

class AHZEventHandler : public BSTEventSink <MenuOpenCloseEvent> {

   EventResult ReceiveEvent(MenuOpenCloseEvent * evn, EventDispatcher<MenuOpenCloseEvent> * dispatcher);
};

