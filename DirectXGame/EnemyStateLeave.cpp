#include "EnemyStateLeave.h"
#include "Enemy.h";
#include "EnemyStateApproach.h";

EnemyStateLeave::EnemyStateLeave(Enemy* enemy) : BaseEnemyState("State Leave", enemy) {
	
	

}

void EnemyStateLeave::Update() { enemy_->SetVel({-0.2, -0.2, -0.05}); }
