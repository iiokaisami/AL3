#include "EnemyStateApproach.h"
#include "Enemy.h";
#include "EnemyStateLeave.h";

EnemyStateApproach::EnemyStateApproach(Enemy* enemy) : BaseEnemyState("State Approach", enemy) { 

}

void EnemyStateApproach::Update() {
	enemy_->SetVel({0, 0, -0.05}); 	

	if (enemy_->GetWorldPosition().z < 5.0f) {

	    enemy_->ChangeState(std::make_unique<EnemyStateLeave>(enemy_));
	
	}
}
