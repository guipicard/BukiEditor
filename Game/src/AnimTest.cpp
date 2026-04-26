#pragma once
#include "AnimTest.h"

#include "Engine.h"
#include "Animation.h"
#include "Box.h"
#include "Circle.h"
#include "Polygon.h"
#include "RigidBody.h"
#include "EKey.h"
#include "BukiContainers.h"

buki::AnimTest::AnimTest(Entity* entity)
	: MonoBehaviour(entity)
{
	Awake();
}

void buki::AnimTest::Awake()
{
	if (!anim)
	{
		anim = m_Entity->AddComponent<Animation>();
	}
	if (!rigidbody)
	{
		rigidbody = m_Entity->AddComponent<RigidBody>();
	}
	if (!boxCollider)
	{
		//boxCollider = m_Entity->AddComponent<Box>();
	}
	if (!circleCollider)
	{
		circleCollider = m_Entity->AddComponent<Circle>();
	}
	if (!polygonCollider)
	{
		polygonCollider = m_Entity->AddComponent<Polygon>();
	}
}

void buki::AnimTest::Start()
{
	if (!Audio().LoadSound("/audio/AngryBird/Sfx - Wood Collision A1.mp3"))
	{
		Log().LogError("Failed to load jump sound effect.");
		Log().LogSdlError();
	}
	jumpSFX = Audio().LoadSound("/audio/AngryBird/Sfx - Wood Collision A1.mp3");
	if (rigidbody)
	{
		rigidbody->def.type = RigidBodyDef::BodyType::Dynamic;
		rigidbody->def.motionLocks.linearX = false;
		rigidbody->def.motionLocks.linearY = false;
		rigidbody->def.motionLocks.angularZ = false;
	}
	if (boxCollider)
	{
		boxCollider->def.shapeDraw = true;
		boxCollider->def.fillDraw = true;

		boxCollider->def.size = Vector2{ -1.0f, -1.0f } + m_Entity->T()->GetSize() / 2;
	}
	if (circleCollider)
	{
		circleCollider->def.shapeDraw = true;
		circleCollider->def.fillDraw = true;

		circleCollider->def.radius = 0.1f + m_Entity->T()->GetSize().x / 4;
		circleCollider->def.isSensor = true;
	}
	if (polygonCollider)
	{
		polygonCollider->def.shapeDraw = true;
		polygonCollider->def.fillDraw = true;

		polygonCollider->def.radius = m_Entity->T()->GetSize().x / 4;
		polygonCollider->def.segments = 5;
	}
	m_Entity->ActivatePhysics();

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
			{ frameWidth*0, 0.0f, frameWidth, frameHeight },
			frameDuration
		}
		, SpriteFrame{
				jumpTexture,
				{ frameWidth*1, 0.0f, frameWidth, frameHeight },
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
	//anim->m_ClipLibrary.LoadFromFile("./assets/Animations/PlayerIdle.anim");

	/*anim->Deserialize({
		{"clips", {
			{"idle", {
				{"name", "idle"},
				{"loop", true},
				{"frames", {
					{
						{"sourceRect", {{"x", 0}, {"y", 0}, {"w", 96}, {"h", 96}}},
						{"originX", 48.0f},
						{"originY", 48.0f},
						{"duration", 0.15f}
					},
					{
						{"sourceRect", {{"x", 96}, {"y", 0}, {"w", 96}, {"h", 96}}},
						{"originX", 48.0f},
						{"originY", 48.0f},
						{"duration", 0.15f}
					},
					{
						{"sourceRect", {{"x", 192}, {"y", 0}, {"w", 96}, {"h", 96}}},
						{"originX", 48.0f},
						{"originY", 48.0f},
						{"duration", 0.15f}
					},
					{
						{"sourceRect", {{"x", 288}, {"y", 0}, {"w", 96}, {"h", 96}}},
						{"originX", 48.0f},
						{"originY", 48.0f},
						{"duration", 0.15f}
					}
				}}
			}}
		}},
		});*/
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

json buki::AnimTest::Serialize()
{
	return json();
}

void buki::AnimTest::Deserialize(json _doc)
{
}

void buki::AnimTest::Set()
{
}