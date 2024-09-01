#include "SceneStateClear.h"
#include "SceneStateTitle.h"
#include "GameScene.h"

SceneStateClear::SceneStateClear(GameScene* gameScene) : BaseSceneState("State Clear", gameScene) { 
	gameScene_->InitializeClear();
}

void SceneStateClear::Update() {
	
	gameScene_->UpdateClear();

	if (gameScene_->ToTitle()) {
		gameScene_->ChangeState(std::make_unique<SceneStateTitle>(gameScene_));
	}
}

void SceneStateClear::Draw() { 
	gameScene_->DrawClear(); 
}

void SceneStateClear::Draw2D() { 
	gameScene_->DrawClear2D();
}
