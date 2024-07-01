#include "EnemyStateLeave.h"
#include "Enemy.h"
#include "EnemyStateApproach.h"

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy) {
	
	 enemy_->SetVel({-0.2f, -0.2f, -0.05f});

}

void EnemyStateLeave::Update() {}
