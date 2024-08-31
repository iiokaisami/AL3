#pragma once
#include <string>

class GameScene;

class BaseSceneState {
public:
	virtual ~BaseSceneState();

	BaseSceneState(const std::string& scene, GameScene* gameScene) : scene_(scene), gameScene_(gameScene){};

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual void Draw2D() = 0;

protected:
	std::string scene_;

	GameScene* gameScene_ = nullptr;
};
