#pragma once

#include "AnimTest.h"
#include "Engine.h"
#include "Entity.h"
#include "Animation.h"
#include "Box.h"
#include "Circle.h"
#include "Polygon.h"
#include "RigidBody.h"
#include "EKey.h"
#include "BukiContainers.h"
#include "Text.h"

buki::AnimTest::AnimTest(Entity* entity) : MonoBehaviour(entity) {}

void buki::AnimTest::Start()
{
	anim = m_Entity->GetComponent<Animation>();
	if (!anim)
	{
		anim = m_Entity->AddComponent<Animation>();
	}
	rigidbody = m_Entity->GetComponent<RigidBody>();
	if (!rigidbody)
	{
		rigidbody = m_Entity->AddComponent<RigidBody>();
	}
	boxCollider = m_Entity->GetComponent<Box>();
	if (!boxCollider)
	{
		boxCollider = m_Entity->AddComponent<Box>();
	}
	text = m_Entity->GetComponent<Text>();
	if (!text)
	{
		text = m_Entity->AddComponent<Text>();
	}
	if (!Audio().LoadSound("/audio/AngryBird/Sfx - Wood Collision A1.mp3"))
	{
		Log().LogError("Failed to load jump sound effect.");
		Log().LogSdlError();
	}
	jumpSFX = Audio().LoadSound(jumpSFXPath);

	Texture2D* idleTexture = Textures().Load("./assets/Samurai/IDLE.png");
	const float frameWidth = 96.0f;
	const float frameHeight = 96.0f;
	const float frameDuration = 0.2f;
	AnimationClip idleClip;
	idleClip.name = "idle";
	idleClip.loop = true;
	idleClip.frames =
	{
		SpriteFrame{
			idleTexture,
			{ frameWidth * 0, 0.0f, frameWidth, frameHeight },
			frameDuration
		},
			SpriteFrame{
				idleTexture,
				{ frameWidth * 1, 0.0f, frameWidth, frameHeight },
				frameDuration
		},
			SpriteFrame{
				idleTexture,
				{ frameWidth * 2, 0.0f, frameWidth, frameHeight },
				frameDuration
		},
			SpriteFrame{
				idleTexture,
				{ frameWidth * 3, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 4, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 5, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 6, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 7, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 8, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				idleTexture,
				{ frameWidth * 9, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
	};
	anim->AddClip(idleClip);
	AnimationTransition idleToWalkTransition;
	idleToWalkTransition.to = "walk";
	idleToWalkTransition.condition = [this]()
		{
			return Input().IsKeyPressed(EKey::EKEY_A) || Input().IsKeyPressed(EKey::EKEY_D);
		};
	AnimationState idleState;
	idleState.clipName = "idle";
	idleState.name = "idle";
	idleState.onEnter = [this]()
		{
			anim->m_Animator.Play("idle");
		};
	idleState.transitions.push_back(idleToWalkTransition);
	idleState.onExit = [this]()
		{
			anim->m_Animator.Stop();
		};
	anim->m_StateMachine.AddState(idleState);

	Texture2D* walkTexture = Textures().Load("./assets/Samurai/WALK.png");
	AnimationClip walkClip;
	walkClip.name = "walk";
	walkClip.loop = true;
	walkClip.frames =
	{
		SpriteFrame{
			walkTexture,
			{ frameWidth * 0, 0.0f, frameWidth, frameHeight },
			frameDuration
		},
			SpriteFrame{
				walkTexture,
				{ frameWidth * 1, 0.0f, frameWidth, frameHeight },
				frameDuration
		},
			SpriteFrame{
				walkTexture,
				{ frameWidth * 2, 0.0f, frameWidth, frameHeight },
				frameDuration
		},
			SpriteFrame{
				walkTexture,
				{ frameWidth * 3, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 4, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 5, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 6, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 7, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 8, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 9, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 10, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		,
			SpriteFrame{
				walkTexture,
				{ frameWidth * 11, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
	};
	anim->AddClip(walkClip);
	AnimationState walkState;
	walkState.name = "walk";
	walkState.clipName = "walk";
	walkState.onEnter = [this]()
		{
			anim->m_Animator.Play("walk");
		};
	AnimationTransition walkToIdleTransition;
	walkToIdleTransition.to = "idle";
	walkToIdleTransition.forceRestart = false;
	walkToIdleTransition.condition = [this]()
		{
			return !Input().IsKeyPressed(EKey::EKEY_A) && !Input().IsKeyPressed(EKey::EKEY_D);
		};
	walkState.transitions.push_back(walkToIdleTransition);
	anim->m_StateMachine.AddState(walkState);

	Texture2D* runTexture = Textures().Load("./assets/Samurai/RUN.png");
	AnimationClip runClip;
	runClip.name = "run";
	runClip.loop = true;
	runClip.frames =
	{
		SpriteFrame{
			runTexture,
			{ frameWidth * 0, 0.0f, frameWidth, frameHeight },
			frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 1, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 2, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 3, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 4, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 5, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 6, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 7, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 8, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 9, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 10, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 11, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 12, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 13, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 14, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				runTexture,
				{ frameWidth * 15, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
	};
	anim->AddClip(runClip);
	AnimationState runState;
	runState.name = "run";
	runState.clipName = "run";
	runState.onEnter = [this]()
		{
			anim->m_Animator.Play("run");
		};
	anim->m_StateMachine.AddState(runState);

	Texture2D* jumpTexture = Textures().Load("./assets/Samurai/JUMP.png");
	AnimationClip jumpClip;
	jumpClip.name = "jump";
	jumpClip.loop = true;
	jumpClip.frames =
	{
		SpriteFrame{
			jumpTexture,
			{ frameWidth * 0, 0.0f, frameWidth, frameHeight },
			frameDuration
		}
		, SpriteFrame{
				jumpTexture,
				{ frameWidth * 1, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
		, SpriteFrame{
				jumpTexture,
				{ frameWidth * 2, 0.0f, frameWidth, frameHeight },
				frameDuration
		}
	};
	anim->AddClip(jumpClip);
	AnimationState jumpState;
	jumpState.name = "jump";
	jumpState.clipName = "jump";
	jumpState.onEnter = [this]()
		{
			anim->m_Animator.Play("jump");
		};
	anim->m_StateMachine.AddState(jumpState);

	anim->params.params["flipx"] = false;

	anim->m_StateMachine.SetInitialState("idle");

	text->SetFontPath("./fonts/Kenney/Kenney Future Narrow.ttf");
	text->SetFontSize(24);
	text->SetText(anim->m_StateMachine.GetCurrentState());
	text->SetPositionOffset({ 0.0f, -3.0f });
	text->Set();
	anim->Set();
}

void buki::AnimTest::FixedUpdate(const float dt)
{
}

void buki::AnimTest::Destroy()
{
}

void buki::AnimTest::Update(const float dt)
{
	if (anim == nullptr)
	{
		return;
	}

	if (Input().IsKeyDown(EKey::EKEY_A))
	{
		anim->params.Set("flipx", true);
	}
	else if (Input().IsKeyDown(EKey::EKEY_D))
	{
		anim->params.Set("flipx", false);
	}

	if (Input().IsKeyDown(EKey::EKEY_SPACE))
	{
		Audio().PlaySFX(jumpSFX);
	}

	const std::string currentState = anim->m_StateMachine.GetCurrentState();
	if (currentState != text->GetText())
	{
		text->SetText(currentState);
		text->Set();
	}
}

void buki::AnimTest::OnCollisionEnter(Entity* other)
{
	Log().LogMessage("COLLISION ENTER with entity: " + other->GetName());
	Audio().PlaySFX(jumpSFX);
}

void buki::AnimTest::OnCollisionExit(Entity* other)
{
	Log().LogMessage("COLLISION EXIT with entity: " + other->GetName());
}

void buki::AnimTest::OnCollisionHit(Entity* other)
{
	Log().LogMessage("COLLISION HIT with entity: " + other->GetName());
}

void buki::AnimTest::OnSensorEnter(Entity* other)
{
	Log().LogMessage("SENSOR ENTER with entity: " + other->GetName());
}

void buki::AnimTest::OnSensorExit(Entity* other)
{
	Log().LogMessage("SENSOR EXIT with entity: " + other->GetName());
}

void buki::AnimTest::OnSet()
{
}
