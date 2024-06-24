#pragma once
#include<string>;

class Enemy;

class BaseEnemyState {
public:

	virtual ~BaseEnemyState();

	BaseEnemyState(const std::string& scene, Enemy* enemy) : scene_(scene), enemy_(enemy){};

	virtual void Update() = 0;

protected:

	std::string scene_;

	Enemy* enemy_ = nullptr;

};
