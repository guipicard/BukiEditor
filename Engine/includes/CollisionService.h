#pragma once
#include "ICollision.h"
#include "BukiContainers.h"
#include <cstdlib>
#include <iostream>
	
namespace buki
{
	struct CollisionService : public ICollision
	{
		virtual void CheckCircles(Manifold* M) override;
		virtual void CheckRects(Manifold* M) override;
		virtual void CheckCircleRect(Manifold* M, Entity* a, Entity* b) override;
		virtual void CheckRectCircle(Manifold* M, Entity* a, Entity* b) override;
		virtual bool CheckAABB(AABB A, AABB B) override;
		virtual bool CheckPointCircle(float px, float py, float cx, float cy, float cr) override;
		virtual bool CheckPointRect(float px, float py, float rx, float ry, float rw, float rh) override;
	};
}
