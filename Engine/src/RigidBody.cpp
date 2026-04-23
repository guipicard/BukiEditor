#include "RigidBody.h"

#include "Entity.h"
#include "ILogger.h"
#include "IWorld.h"

void buki::RigidBody::FixedUpdate(float dt)
{
}

void buki::RigidBody::Start()
{
}

void buki::RigidBody::Destroy()
{
}

json buki::RigidBody::Serialize()
{
    json doc;
    doc["type"] = static_cast<int>(def.type);
    doc["motionLocks"] =
    {
        { "linearX", def.motionLocks.linearX },
        { "linearY", def.motionLocks.linearY },
        { "angularZ", def.motionLocks.angularZ }
    };
    return doc;
}

void buki::RigidBody::Deserialize(json _doc)
{
    def.type = static_cast<BodyType>(_doc.value("type", static_cast<int>(BodyType::Dynamic)));

    if (_doc.contains("motionLocks"))
    {
        const json& locks = _doc["motionLocks"];
        def.motionLocks.linearX = locks.value("linearX", false);
        def.motionLocks.linearY = locks.value("linearY", false);
        def.motionLocks.angularZ = locks.value("angularZ", false);
    }
}

void buki::RigidBody::Set()
{
}

float buki::RigidBody::GetMass() const
{
    return Physics().GetMass(bodyId);
}

buki::Vector2 buki::RigidBody::GetVelocity() const
{
    return Physics().GetVelocity(m_Entity);
}

void buki::RigidBody::SetForce(Vector2 _force)
{
}
