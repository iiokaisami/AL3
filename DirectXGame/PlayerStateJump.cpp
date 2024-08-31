#include "PlayerStateJump.h"
#include "PlayerStateRoot.h"
#include "Player.h"

PlayerStateJump::PlayerStateJump(Player* player) : BasePlayerState("State Jump", player) {
	
}

void PlayerStateJump::Update() {
	player_->Jump();


	if (!player_->GetIsJump()) {
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
	}
}
