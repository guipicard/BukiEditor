#pragma once
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

namespace buki
{
    struct Manifold;
    struct Entity;
    struct AABB;

    struct ICollision
    {
    public:

        virtual ~ICollision() = default; // Destructeur virtuel par défaut.

        // Vérifie si un point est à l'intérieur d'un cercle.
        virtual bool CheckPointCircle(float px, float py, float cx, float cy, float cr) = 0;

        // Vérifie si deux cercles se chevauchent.
        virtual void CheckCircles(Manifold* M) = 0;

        // Vérifie si un point est à l'intérieur d'un rectangle.
        virtual bool CheckPointRect(float px, float py, float rx, float ry, float rw, float rh) = 0;

        // Vérifie si deux objets rectangulaires se chevauchent.
        virtual void CheckRects(Manifold* M) = 0;

        // Verifie si deux rectangles se chevauchent
        virtual bool CheckAABB(AABB A, AABB B) = 0;

        // Vérifie si un rectangle et un cercle se chevauchent.
        virtual void CheckCircleRect(Manifold* M, Entity* a, Entity* b) = 0;

        // Vérifie si un rectangle et un cercle se chevauchent.
        virtual void CheckRectCircle(Manifold* M, Entity* a, Entity* b) = 0;
    };
}
