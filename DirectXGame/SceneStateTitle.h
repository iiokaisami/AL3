#pragma once
#include "BaseSceneState.h"

class SceneStateTitle : public BaseSceneState {
public:
	SceneStateTitle(GameScene* gameScene);

	void Update();

	void Draw();

	void Draw2D();
};
