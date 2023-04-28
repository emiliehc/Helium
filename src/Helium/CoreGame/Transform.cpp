#include "Helium/HeliumPrecompiled.h"

#include "Helium/CoreGame/Transform.h"

#include "Helium/ObjectModel/RuntimeObjectRegistry.h"
#include "Helium/ObjectModel/ManagedObject.h"


heliumBegin

#pragma region Generated
void Transform::RegisterMembers()
{
}
#pragma endregion

void Transform::UpdateLocalMatrix()
{
    auto mat = glm::identity<glm::mat4>();
    mat = glm::scale(mat, m_LocalScale);
    mat = glm::mat4_cast(m_LocalRotation) * mat;
    mat = glm::translate(mat, m_LocalTranslation);
    m_LocalMatrix = mat;
    m_LocalMatrixIsDirty = false;
}

void Transform::SetLocalTranslation(const vec3& value)
{
    m_LocalTranslation = value;
	m_LocalMatrixIsDirty = true;
}

vec3 Transform::GetLocalTranslation() const
{
    return m_LocalTranslation;
}

void Transform::SetLocalRotation(const quat& value)
{
    m_LocalRotation = value;
	m_LocalMatrixIsDirty = true;
}

quat Transform::GetLocalRotation() const
{
   return m_LocalRotation;
}

void Transform::SetLocalScale(const vec3& value)
{
    m_LocalScale = value;
	m_LocalMatrixIsDirty = true;
}

vec3 Transform::GetLocalScale() const
{
    return m_LocalScale;
}

glm::mat4 Transform::GetLocalMatrix()
{
    if (m_LocalMatrixIsDirty)
    {
        UpdateLocalMatrix();
    }
    return m_LocalMatrix;
}


heliumEnd

