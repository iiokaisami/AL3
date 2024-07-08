#include "EnemyStateApproach.h"
#include "Enemy.h"
#include "EnemyStateLeave.h"

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy) { 
	//enemy_->SetVel({0, 0, -0.05f});
	enemy->SetFireTimer();
}

void EnemyStateApproach::Update() {

	if (enemy_->GetWorldPosition().z < -6.0f) {

		enemy_->ClearTimeCalls(enemy_->GetTimeCalls());
	    enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	
	}
}
