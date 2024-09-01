#pragma once
#include "BaseSceneState.h"

class SceneStateClear : public BaseSceneState {
public:
	SceneStateClear(GameScene* gameScene);

	void Update();

	void Draw();

	void Draw2D();
};
