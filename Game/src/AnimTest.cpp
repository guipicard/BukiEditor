#pragma once
#include "AnimTest.h"

#include "Engine.h"
#include "Animation.h"
#include "Box.h"
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
	if (!collider)
	{
		collider = m_Entity->AddComponent<Box>();
		collider->fillDraw = true;
		collider->shapeDraw = true;
		collider->ShapeColor = { 0.0f, 1.0f, 0.0f, 1.0f };
		collider->DebugColor = { 1.0f, 0.0f, 0.0f, 1.0f };
		collider->Collider.Size = { 48.0f*5, 48.0f*5 };
	}
	if (!rigidbody)
	{
		rigidbody = m_Entity->AddComponent<RigidBody>();
		rigidbody->motionLocks.linearY = true;
		rigidbody->motionLocks.linearX = true;
	}
	m_Entity->ActivatePhysics();
}

void buki::AnimTest::Start()
{
	Texture2D* idleTexture = Textures().Load("./assets/Samurai/IDLE.png");
	AnimationClip idleClip;
	idleClip.name = "idle";
	idleClip.loop = true;
	idleClip.frames =
	{
		SpriteFrame{
			idleTexture,
			{ 0.0f, 0.0f, 96.0f, 96.0f },
			48.0f,
			48.0f,
			0.15f
		},
			SpriteFrame{
				idleTexture,
				{ 96.0f, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		},
			SpriteFrame{
				idleTexture,
				{ 96.0f * 2, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		},
			SpriteFrame{
				idleTexture,
				{ 96.0f * 3, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 4, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 5, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 6, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 7, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 8, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				idleTexture,
				{ 96.0f * 9, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
	};
	anim->AddClip(idleClip);
	AnimationState idleState{
	"idle",
	"idle",
		{AnimationTransition{
		"walk",
		[this]()
		{
			return Input().IsKeyPressed(EKey::EKEY_A) || Input().IsKeyPressed(EKey::EKEY_D);
		},
		false
		}},
		{[this]()
		{
			anim->m_Animator.Play("walk");
		} },
		nullptr
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
			{ 0.0f, 0.0f, 96.0f, 96.0f },
			48.0f,
			48.0f,
			0.15f
		},
			SpriteFrame{
				walkTexture,
				{ 96.0f, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		},
			SpriteFrame{
				walkTexture,
				{ 96.0f * 2, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		},
			SpriteFrame{
				walkTexture,
				{ 96.0f * 3, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 4, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 5, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 6, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 7, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 8, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 9, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 10, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		,
			SpriteFrame{
				walkTexture,
				{ 96.0f * 11, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
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
	anim->m_StateMachine.AddState(walkState);

	Texture2D* runTexture = Textures().Load("./assets/Samurai/RUN.png");
	AnimationClip runClip;
	runClip.name = "run";
	runClip.loop = true;
	runClip.frames =
	{
		SpriteFrame{
			runTexture,
			{ 0.0f, 0.0f, 96.0f, 96.0f },
			48.0f,
			48.0f,
			0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 2, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 3, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 4, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 5, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 6, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 7, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 8, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 9, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 10, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 11, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 12, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 13, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 14, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				runTexture,
				{ 96.0f * 15, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
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
			{ 0.0f, 0.0f, 96.0f, 96.0f },
			48.0f,
			48.0f,
			0.15f
		}
		, SpriteFrame{
				jumpTexture,
				{ 96.0f, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
		}
		, SpriteFrame{
				jumpTexture,
				{ 96.0f * 2, 0.0f, 96.0f, 96.0f },
				48.0f,
				48.0f,
				0.15f
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

	anim->m_StateMachine.SetInitialState("idle");

	Log().LogMessage(std::to_string(m_Entity->GetTransform()->position.x));
	Log().LogMessage(std::to_string(m_Entity->GetTransform()->position.y));
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
}

void buki::AnimTest::OnCollisionEnter(Entity* other)
{
}

void buki::AnimTest::OnCollisionExit(Entity* other)
{
}

void buki::AnimTest::OnCollisionHit(Entity* other)
{
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