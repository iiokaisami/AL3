#pragma once
#include "BaseEnemyState.h"

class EnemyStateRoot : public BaseEnemyState {
public:

	EnemyStateRoot(Enemy* enemy);

	void Update();
};