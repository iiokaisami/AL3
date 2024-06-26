#pragma once
#include <iostream>
#include <functional>

class TimeCall {
public:
	//コンストラクタ
	TimeCall(std::function<void(void)> f, uint32_t time);

	//更新
	void Update();

	//完了ならtrueを返す
	bool IsFinished() { return isFinished_ = true; }

private:

	//コールバック
	std::function<void(void)> f_;

	//残り時間
	uint32_t time_ = 0;

	//完了フラグ
	bool isFinished_ = false;

};
