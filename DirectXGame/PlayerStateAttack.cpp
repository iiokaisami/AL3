#include "PlayerStateAttack.h"
#include "PlayerStateRoot.h"
#include "Player.h"

PlayerStateAttack::PlayerStateAttack(Player* player) : BasePlayerState("State Attack", player) { 
	player_->SetAttackParameter(0.0f, 18);
}

void PlayerStateAttack::Update() { 
	player_->AttackMotion();

	player_->UpdateMatrix();
	if (!player_->GetIsAttack())
	{
		player_->ChangeState(std::make_unique<PlayerStateRoot>(player_));
	}
}
