#pragma once
#include "BaseEnemyState.h"

class EnemyStateLeave : public BaseEnemyState {
public:

	EnemyStateLeave(Enemy* enemy);

	void Update();

};
