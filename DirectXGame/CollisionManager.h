#pragma once
#include "Collider.h"
#include <list>
#include "Player.h"
#include "PlayerBullet.h"
#include "Enemy.h"
#include "EnemyBullet.h"


class CollisionManager {
public:

	CollisionManager();

	~CollisionManager();

	void Initialize();

	void UpData(Player* player, 
		const std::list<PlayerBullet*>& playerBullets,
		const std::list<EnemyBullet*>& enemyBullets,
	    Enemy* enemy /*const std::list<Enemy*>& enemys*/);

	void RegistrationList(Player* player, 
		const std::list<PlayerBullet*>& playerBullets,
		const std::list<EnemyBullet*>& enemyBullets,
	    Enemy* enemy /*const std::list<Enemy*>& enemys*/);

	void ClearList();

	/// <summary>
	/// 衝突判定と応答
	/// </summary>
	void CheckAllCollisions();

private:

	// コライダーリスト
	std::list<Collider*> colliders_;
	
	/// <summary>
	/// コライダーを2つの衝突判定と応答
	/// </summary>
	/// <param name="collierA">コライダーA</param>
	/// <param name="colliderB">コライダーB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

};
