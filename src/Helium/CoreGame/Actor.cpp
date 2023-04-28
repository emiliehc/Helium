#include "Helium/HeliumPrecompiled.h"

#include "Helium/CoreGame/Actor.h"
#include "Helium/ObjectModel/RuntimeObjectRegistry.h"
#include "Helium/CoreGame/Transform.h"

heliumBegin

Actor::Actor()
{
	// immediately add a transform component, which is mandatory for all actors
	TryAddComponent<Transform>();
}

Actor::~Actor()
{

}

void Actor::Awake()
{

}

void Actor::Start()
{

}

void Actor::Update()
{

}

void Actor::FixedUpdate()
{

}

void Actor::OnDestroy()
{

}

void Actor::DestroyChildObjects()
{
    base::DestroyChildObjects();
    // queue its components for destruction
    for (Handle<ActorComponent> component : m_ComponentStore.m_Components)
    {
        QueueDestroyForEndOfFrame(component);
    }
}

void Actor::AddChild(Handle<Actor> child)
{
    if (child->GetParent() == this)
    {
        return;
    }
    child->SetParent(this);
	m_Children.emplace_back(child);
}

void Actor::RemoveChild(Handle<Actor> child)
{
    if (child->GetParent() != this)
    {
        throw std::runtime_error("Cannot remove child from actor that is not its parent");
		return;
	}
    child->SetParent(nullptr);
    m_Children.erase(std::find(m_Children.begin(), m_Children.end(), child));
}

void Actor::SetParent(Handle<Actor> parent)
{
    if (m_Parent == parent)
    {
		return;
	}
    if (m_Parent)
    {
		m_Parent->RemoveChild(this);
    }
    m_Parent = parent;
}


Handle<ActorComponent> ActorComponentStore::GetComponentByTypeID(const CRC32 typeID)
{
    if (m_ComponentLookupTable.contains(typeID))
    {
        return m_ComponentLookupTable[typeID];
    }

    return nullptr;
}

void ActorComponentStore::AddComponent(Handle<ActorComponent> component)
{
    m_Components.emplace_back(component);
    m_ComponentLookupTable[component->GetTypeID()] = component;
}

void ActorComponentStore::RemoveComponentByTypeID(const CRC32 typeID)
{
    if (m_ComponentLookupTable.contains(typeID))
    {
        Handle<ActorComponent> component = m_ComponentLookupTable[typeID];
        m_ComponentLookupTable.erase(typeID);
        m_Components.erase(std::find(m_Components.begin(), m_Components.end(), component));
    }
}

bool ActorComponentStore::HasComponentByTypeID(const CRC32 typeID)
{
    return m_ComponentLookupTable.contains(typeID);
}

heliumEnd

