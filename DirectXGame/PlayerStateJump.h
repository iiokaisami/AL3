#pragma once
#include "BasePlayerState.h"

class PlayerStateJump : public BasePlayerState {
public:
	PlayerStateJump(Player* player);

	void Update();
};
