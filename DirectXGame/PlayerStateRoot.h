#pragma once
#include "BasePlayerState.h"

class PlayerStateRoot : public BasePlayerState {
 public:

	PlayerStateRoot(Player* player);

	 void Update();
 };
