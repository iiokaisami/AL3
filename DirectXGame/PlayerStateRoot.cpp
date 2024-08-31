#include "PlayerStateRoot.h"
#include "PlayerStateAttack.h"
#include "PlayerStateJump.h"
#include "Player.h"

PlayerStateRoot::PlayerStateRoot(Player* player) : BasePlayerState("State Root", player) {
	
}

void PlayerStateRoot::Update() {
	player_->UpdateFloatingGimick();

	player_->Move();

	player_->Turn();

	player_->Attack();

	player_->SetJumpParameter();
	
	if (player_->GetIsAttack()) {
		player_->ChangeState(std::make_unique<PlayerStateAttack>(player_));
	}else if (player_->GetIsJump())
	{
		player_->ChangeState(std::make_unique<PlayerStateJump>(player_));
	}
}
