#include "CollisionManager.h"


CollisionManager::CollisionManager() {}

CollisionManager::~CollisionManager() {
	
	colliders_.clear(); 
}

void CollisionManager::Initialize() {}

void CollisionManager::UpData(Player* player,
	const std::list<PlayerBullet*>& playerBullets,
	const std::list<EnemyBullet*>& enemyBullets, 
	const std::list<Enemy*>& enemys) { 
	
	ClearList();
	RegistrationList(player,playerBullets,enemyBullets,enemys);
	CheckAllCollisions();

}

void CollisionManager::RegistrationList(Player* player, 
	const std::list<PlayerBullet*>& playerBullets, 
	const std::list<EnemyBullet*>& enemyBullets, 
	const std::list<Enemy*>& enemys) {

	// コライダーをリストに登録
	colliders_.push_back(player);
	// 自弾全てについて
	for (PlayerBullet* playerBullet : playerBullets) {
		colliders_.push_back(playerBullet);
	}
	// 敵弾全てについて
	for (EnemyBullet* enemyBullet : enemyBullets) {
		colliders_.push_back(enemyBullet);
	}
	// 敵全てについて
	for (Enemy* enemy : enemys) {
		colliders_.push_back(enemy);
	}
}

void CollisionManager::ClearList() { 
	
	colliders_.clear();

}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	Vector3 posA, posB;

	posA = colliderA->GetWorldPosition();
	posB = colliderB->GetWorldPosition();

	float dir, rad, radA, radB;
	dir = sqrtf((posB.x - posA.x) * (posB.x - posA.x) + (posB.y - posA.y) * (posB.y - posA.y) + (posB.z - posA.z) * (posB.z - posA.z));

	radA = colliderA->GetRadius();
	radB = colliderB->GetRadius();

	rad = sqrtf((radA + radB) * (radA + radB));

	if (colliderA->GetCollisionAttribute() != colliderB->GetCollisionMask() or colliderA->GetCollisionMask() != colliderB->GetCollisionAttribute()) {
		return;
	}

	// 球と球の交差判定
	if (dir <= rad) {
		// コライダーAの衝突時コールバックを呼び出す
		colliderA->OnCollision();
		// コライダーBの衝突時コールバックを呼び出す
		colliderB->OnCollision();
	}
}

void CollisionManager::CheckAllCollisions() {

	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;

		// イテレータBはイテレータAの次の要素から回す（重複判定を回避）
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colliderA, colliderB);
		}
	}
}