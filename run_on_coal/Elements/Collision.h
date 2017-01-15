#pragma once
#define COLLISION_TYPE_SPHERE 0U
#define COLLISION_TYPE_BOX 1U
#define COLLISION_TYPE_CYLINDER 2U
#define COLLISION_TYPE_CAPSULE 3U
#define COLLISION_TYPE_CONE 4U

namespace ROC
{

class Collision
{
    btRigidBody *m_rigidBody;
public:
    void SetPosition(glm::vec3 &f_pos);
    void GetPosition(glm::vec3 &f_pos);
    void SetRotation(glm::quat &f_rot);
    void GetRotation(glm::quat &f_rot);
protected:
    Collision();
    ~Collision();
    bool Create(unsigned char f_type, glm::vec3 &f_size);
    inline btRigidBody* GetRigidBody() { return m_rigidBody; }
    friend class ElementManager;
    friend class PhysicsManager;
};

}