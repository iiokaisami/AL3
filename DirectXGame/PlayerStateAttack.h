#pragma once
#include "BasePlayerState.h"

class PlayerStateAttack : public BasePlayerState {
public:
	PlayerStateAttack(Player* player);

	void Update();
};
