#include "SceneStateTitle.h"
#include "SceneStatePlay.h"
#include "GameScene.h"

SceneStateTitle::SceneStateTitle(GameScene* gameScene) : BaseSceneState("State Title", gameScene) {}

void SceneStateTitle::Update() {

	if (gameScene_->TitleToPlay())
	{
		gameScene_->ChangeState(std::make_unique<SceneStatePlay>(gameScene_));
	}
}

void SceneStateTitle::Draw() {
	gameScene_->DrawTitle();
}

void SceneStateTitle::Draw2D() { 
	gameScene_->DrawTitle2D(); 
}
