#include "SceneStatePlay.h"
#include "SceneStateTitle.h"
#include "SceneStateGameOver.h"
#include "SceneStateClear.h"
#include "GameScene.h"

SceneStatePlay::SceneStatePlay(GameScene* gameScene) : BaseSceneState("State Title", gameScene) { 
	gameScene_->InitializePlay();
}

void SceneStatePlay::Update() { 
	gameScene_->UpdatePlay();

}

void SceneStatePlay::Draw() { 
	gameScene_->DrawPlay();

	if (gameScene_->PlayToGameOver()) {
		gameScene_->ChangeState(std::make_unique<SceneStateGameOver>(gameScene_));
	}else if (gameScene_->PlayToClear()) {
		gameScene_->ChangeState(std::make_unique<SceneStateClear>(gameScene_));
	}
}

void SceneStatePlay::Draw2D() {
	gameScene_->DrawPlay2D();
}
