#pragma once
#include<string>

class Player;

class BasePlayerState {
public:
	virtual ~BasePlayerState();

	BasePlayerState(const std::string& scene, Player* player) : scene_(scene), player_(player){};

	virtual void Update() = 0;

protected:

	std::string scene_;

	Player* player_ = nullptr;
};
