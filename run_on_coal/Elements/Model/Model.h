#pragma once
#include "Elements/Element.h"
#define ROC_MODEL_TYPE_NONE 0
#define ROC_MODEL_TYPE_STATIC 1
#define ROC_MODEL_TYPE_ANIMATED 2

namespace ROC
{

class Collision;
class Geometry;
class Skeleton;
class Animation;
class Model final : public Element
{
    Geometry *m_geometry;

    glm::vec3 m_localPosition;
    glm::vec3 m_globalPosition;
    glm::quat m_localRotation;
    glm::quat m_globalRotation;
    glm::vec3 m_localScale;
    glm::vec3 m_globalScale;
    glm::mat4 m_localMatrix;
    glm::mat4 m_globalMatrix;
    float m_boundSphereRaduis;
    bool m_useScale;
    bool m_rebuildMatrix;
    bool m_rebuilded;

    Model *m_parent;
    int m_parentBone;

    Animation *m_animation;
    unsigned int m_animationTick;
    enum AnimationState { None = 0U, Paused, Playing } m_animState;
    float m_animationSpeed;

    Skeleton *m_skeleton;

    Collision *m_collision;

    void UpdateGlobalTransform();

    Model(const Model& that);
    Model &operator =(const Model &that);
public:
    inline bool HasGeometry() const { return (m_geometry != nullptr); }
    inline Geometry* GetGeometry() { return m_geometry; }

    void SetLocalPosition(const glm::vec3 &f_pos);
    inline const glm::vec3& GetLocalPosition() const { return m_localPosition; }
    inline const glm::vec3& GetGlobalPosition() const { return m_globalPosition; }

    void SetLocalRotation(const glm::quat &f_rot);
    inline const glm::quat& GetLocalRotation() const { return m_localRotation; }
    inline const glm::quat& GetGlobalRotation() const { return m_globalRotation; }

    void SetLocalScale(const glm::vec3 &f_scl);
    inline const glm::vec3& GetLocalScale() const { return m_localScale; }
    inline const glm::vec3& GetGlobalScale() const { return m_globalScale; }

    inline const glm::mat4& GetLocalMatrix() const { return m_localMatrix; }
    inline const glm::mat4& GetGlobalMatrix() const { return m_globalMatrix; }

    float inline GetBoundSphereRadius() const { return m_boundSphereRaduis; }

    inline Model* GetParent() { return m_parent; }

    inline bool HasAnimation() const { return (m_animation != nullptr); }
    inline Animation* GetAnimation() { return m_animation; }
    bool PlayAnimation();
    bool PauseAnimation();
    bool ResetAnimation();
    bool SetAnimationSpeed(float f_val);
    inline float GetAnimationSpeed() const { return (m_animation ? m_animationSpeed : -1.f); }
    bool SetAnimationProgress(float f_val);
    float GetAnimationProgress() const;
    bool SetAnimationBlendFactor(float f_val);
    float GetAnimationBlendFactor() const;

    inline bool HasSkeleton() const { return (m_skeleton != nullptr); }

    inline bool HasCollision() const { return (m_collision != nullptr); }
    inline Collision* GetCollision() { return m_collision; }
protected:
    explicit Model(Geometry *f_geometry);
    ~Model();

    inline void SetGeometry(Geometry *f_geometry) { m_geometry = f_geometry; }

    void UpdateMatrix();

    void SetParent(Model *f_model, int f_bone = -1);

    void SetAnimation(Animation *f_anim);
    void UpdateAnimation();

    inline Skeleton* GetSkeleton() { return m_skeleton; }

    void SetCollision(Collision *f_col);
    void UpdateCollision();

    friend class ElementManager;
    friend class InheritanceManager;
    friend class PhysicsManager;
    friend class PreRenderManager;
    friend class RenderManager;
};

}
