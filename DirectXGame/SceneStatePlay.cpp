#include "SceneStatePlay.h"
#include "SceneStateTitle.h"
#include "GameScene.h"

SceneStatePlay::SceneStatePlay(GameScene* gameScene) : BaseSceneState("State Title", gameScene) {}

void SceneStatePlay::Update() {}

void SceneStatePlay::Draw() { 
	gameScene_->DrawPlay();
}

void SceneStatePlay::Draw2D() {
	gameScene_->DrawPlay2D();
}
