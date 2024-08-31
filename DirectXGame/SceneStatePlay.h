#pragma once
#include "BaseSceneState.h"

class SceneStatePlay : public BaseSceneState {
public:
	SceneStatePlay(GameScene* gameScene);

	void Update();

	void Draw();

	void Draw2D();
};
