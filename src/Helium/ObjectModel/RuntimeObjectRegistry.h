#pragma once

#include <Helium/CoreGame/Actor.h>
#include "Helium/HeliumPrecompiled.h"
#include "Helium/ObjectModel/Singleton.h"
#include "Helium/ObjectModel/Handle.h"


heliumBegin

class ManagedObject;
class RuntimeObjectRegistry;

class RuntimeObjectRegistry final
{
public:
    static RuntimeObjectRegistry* GetInstance();

    template<typename T, typename ... TArgs> requires std::is_base_of_v<ManagedObject, T>
    Handle<T> CreateObject(TArgs&& ... args)
    {
        return CreateAndRegisterObject<T>(std::forward<TArgs>(args)...);
    }

    template<typename T> requires std::is_base_of_v<ManagedObject, T>
    void ObjectDestroyImmediate(Handle<T> object)
    {
        UnregisterAndDestroyObject(object);
    }

    template<typename T> requires std::is_base_of_v<ManagedObject, T>
    void ObjectQueueDestroyForEndOfFrame(Handle<T> object)
    {
        m_DestroyQueue.push_back(object);
    }

    void ObjectQueueDestroyForEndOfFrame(MonoObject* monoObject);

    void ReportFrameEnd();

    void InitializeMonoCache();

private:
    void UnregisterAndDestroyObject(Handle<ManagedObject> object) const;

    template <typename T, typename ... TArgs> requires std::is_base_of_v<ManagedObject, T>
    Handle<T> CreateAndRegisterObject(TArgs&& ... args)
    {
        Handle<T> object = new T(std::forward<TArgs>(args)...);
        Handle<ManagedObject> managedObject = object;
        MonoClass* cls = T::GetManagedClass();
        MonoObject* monoObject = mono_object_new(mono_domain_get(), cls);
        mono_runtime_object_init(monoObject);
        managedObject->m_ManagedInstance = monoObject;
        return object;
    }

private:
    std::deque<Handle<ManagedObject>> m_DestroyQueue;
    bool m_MonoCacheInitialized = false;
    struct
    {
        MonoClass* m_ManagedObjectClass = nullptr;
        MonoClassField* m_ManagedObjectNativeHandleField = nullptr;
    } m_MonoCache;
};

template<typename T, typename ... TArgs> requires std::is_base_of_v<ManagedObject, T>
Handle<T> CreateObject(TArgs&& ... args)
{
    return RuntimeObjectRegistry::GetInstance()->CreateObject<T>(std::forward<TArgs>(args)...);
}

heliumEnd
