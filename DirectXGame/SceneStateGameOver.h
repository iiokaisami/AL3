#pragma once
#include "BaseSceneState.h"

class SceneStateGameOver : public BaseSceneState {
public:
	SceneStateGameOver(GameScene* gameScene);

	void Update();

	void Draw();

	void Draw2D();
};
