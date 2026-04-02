#pragma once
#include "BricksBehaviour.h"
#include "RigidBody.h"
#include "Engine.h"
#include "Sprite.h"
#include "ComponentRegistration.h"

REGISTER_COMPONENT(BricksBehaviour, "BricksBehaviour");

buki::BricksBehaviour::BricksBehaviour(Entity* _entity)
	: MonoBehaviour(_entity)
{
	spriteComponent = m_Entity->GetComponent<Sprite>();
	if (!spriteComponent)
	{
		spriteComponent = m_Entity->AddComponent<Sprite>();
	}
	health = maxHealth;
}
void buki::BricksBehaviour::Awake()
{

}
void buki::BricksBehaviour::Start()
{
	SetMaxHealth(maxHealth);
}
void buki::BricksBehaviour::Destroy()
{
	stageImages.clear();
	collisionSounds.clear();
	DamageSounds.clear();
	BreakSounds.clear();
	soundMap.clear();
}
void buki::BricksBehaviour::FixedUpdate(const float dt)
{

}
void buki::BricksBehaviour::Update(const float dt)
{

}
void buki::BricksBehaviour::OnCollisionEnter(Entity* other)
{
	if (other == nullptr) return;
	RigidBody* rbA = other->GetComponent<RigidBody>();
	RigidBody* rbB = m_Entity->GetComponent<RigidBody>();
	if (rbA)
	{
		Vector2 velA = rbA->GetVelocity();
		Vector2 velB = rbB->GetVelocity();
		float massA = rbA->GetMass();
		float massB = rbB->GetMass();
		Vector2 vel = velA + velB;
		float mass = massA + massB;
		float dmg = (vel.Length() * mass);
		TakeDamage(dmg);
	}
}
void buki::BricksBehaviour::OnCollisionExit(Entity* other)
{

}
void buki::BricksBehaviour::OnCollisionHit(Entity* other)
{

}

json buki::BricksBehaviour::Serialize()
{
	json doc;
	doc["maxHealth"] = maxHealth;
	doc["health"] = health;
	doc["stageImages"] = stageImages;
	doc["collisionSounds"] = collisionSounds;
	doc["DamageSounds"] = DamageSounds;
	doc["BreakSounds"] = BreakSounds;
	return doc;
}

void buki::BricksBehaviour::Deserialize(json _doc)
{
	maxHealth = _doc["maxHealth"].get<float>();
	health = _doc["health"].get<float>();

	stageImages = _doc["stageImages"].get<std::vector<std::string>>();
	collisionSounds = _doc["collisionSounds"].get<std::vector<std::string>>();
	BreakSounds = _doc["BreakSounds"].get<std::vector<std::string>>();
	DamageSounds = _doc["DamageSounds"].get<std::vector<std::string>>();
}

void buki::BricksBehaviour::Set()
{
	for (auto& img : stageImages)
	{
		spriteComponent->Load(img);
	}
	spriteComponent->Load(stageImages[0]);
	for (auto& sound : collisionSounds)
	{
		size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
		soundMap[sound] = id;
	}
	for (auto& sound : DamageSounds)
	{
		size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
		soundMap[sound] = id;
	}
	for (auto& sound : BreakSounds)
	{
		size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
		soundMap[sound] = id;
	}
}

void buki::BricksBehaviour::AddImage(const std::string& image)
{
	stageImages.push_back(image);
	if (stageImages.size() == 1)
	{
		spriteComponent->Load(image);
	}
}

void buki::BricksBehaviour::AddCollisionSound(const std::string& sound)
{
	size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
	collisionSounds.push_back(sound);
	soundMap[sound] = id;
}

void buki::BricksBehaviour::AddDamageSound(const std::string& sound)
{
	size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
	DamageSounds.push_back(sound);
	soundMap[sound] = id;
}

void buki::BricksBehaviour::AddBreakSound(const std::string& sound)
{
	size_t id = buki::Engine::GetInstance().Audio().LoadSound(sound);
	BreakSounds.push_back(sound);
	soundMap[sound] = id;
}

void buki::BricksBehaviour::PlayCollisionSound()
{
	if (collisionSounds.empty()) return;

	int index = rand() % collisionSounds.size();
	Audio().PlaySFX(soundMap[collisionSounds[index]]);
}

void buki::BricksBehaviour::PlayDamageSound()
{
	if (DamageSounds.empty()) return;

	int index = rand() % DamageSounds.size();
	Audio().PlaySFX(soundMap[DamageSounds[index]]);
}

void buki::BricksBehaviour::PlayBreakSound()
{
	if (BreakSounds.empty()) return;

	int index = rand() % BreakSounds.size();
	Audio().PlaySFX(soundMap[BreakSounds[index]]);
}

void buki::BricksBehaviour::SetMaxHealth(const float amount)
{
	maxHealth = amount;
	health = amount;
}

void buki::BricksBehaviour::TakeDamage(float damage)
{
	if (health <= 0.0f) return;
	health -= damage;
	if (health <= 0.0f)
	{
		PlayBreakSound();
		World().Remove(m_Entity);
		return;
	}
	float stageHealth = maxHealth / stageImages.size();
	float dmgDone = maxHealth - health;
	const int stageIndex = static_cast<int>(dmgDone / stageHealth);
	if (stageIndex < stageImages.size())
	{
		std::string currentImage = spriteComponent->GetPath();
		spriteComponent->Load(stageImages[stageIndex]);
		if (currentImage != stageImages[stageIndex])
		{
			PlayDamageSound();
		}
		else
		{
			if (damage > 1.0f)
			{
				PlayCollisionSound();
			}
		}
	}
	else
	{
		buki::Engine::GetInstance().Log().LogError("Stage Out of Bounds: " + std::to_string(static_cast<int>(dmgDone / stageHealth)));
	}
}
