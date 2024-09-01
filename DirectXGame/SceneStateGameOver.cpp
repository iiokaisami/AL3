#include "SceneStateGameOver.h"
#include "SceneStateTitle.h"
#include "SceneStatePlay.h"
#include "GameScene.h"

SceneStateGameOver::SceneStateGameOver(GameScene* gameScene) : BaseSceneState("State GameOver", gameScene) { 
	gameScene_->InitializeGameOver();

}

void SceneStateGameOver::Update() {

	gameScene_->UpdateGameOver();

	if (gameScene_->ToPlay()) {
		gameScene_->ChangeState(std::make_unique<SceneStatePlay>(gameScene_));
	} else if (gameScene_->ToTitle()) {
		gameScene_->ChangeState(std::make_unique<SceneStateTitle>(gameScene_));
	}
}

void SceneStateGameOver::Draw() { 
	gameScene_->DrawGameOver(); 
}

void SceneStateGameOver::Draw2D() { 
	
	gameScene_->DrawGameOver2D();
}
