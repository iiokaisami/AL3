#include "EnemyStateRoot.h"
#include "Enemy.h"
#include "EnemyStateLeave.h"

EnemyStateRoot::EnemyStateRoot(Enemy* enemy) : BaseEnemyState("State Root", enemy) { 
	//enemy_->SetVel({0, 0, -0.05f});
	enemy->SetFireTimer();
}

void EnemyStateRoot::Update() {

	/*if (enemy_->GetWorldPosition().z < -6.0f) {

		enemy_->ClearTimeCalls(enemy_->GetTimeCalls());
	    enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	
	}*/
}
