#pragma once

#include "Helium/HeliumPrecompiled.h"

#include "Helium/ObjectModel/UnsafeHandle.h"
#include "Helium/Utility/CRC32.h"
#include "Helium/Utility/Hex.h"
#include "Helium/Reflection/TypeRegistry.h"
#include "Helium/Reflection/TypeDescriptor.h"
#include "Helium/ObjectModel/InstanceID.h"

#include "yaml-cpp/yaml.h"

heliumBegin

template <typename T, typename ... Args> requires std::is_base_of_v<ManagedObject, T>
Reference<T> MakeManaged(Args&& ... args);

namespace Internal
{
template<typename T, typename ... Args>
requires std::is_base_of_v<ManagedObject, T> && (!std::is_abstract_v<T>)
Reference<T> MakeManagedConditional(Args&& ... args)
{
    return MakeManaged<T>(std::forward<Args>(args)...);
}

template<typename T, typename ... Args>
requires std::is_base_of_v<ManagedObject, T> && std::is_abstract_v<T>
Reference<T> MakeManagedConditional(Args&& ... args)
{
    return nullptr;
}
}

class ManagedClassDescriptor;

class ManagedObject
{
private:
    static ManagedObject* StaticConstruct();

private:
    constexpr static Bool s_IsSerializable = true;
    constexpr static CRC32 s_TypeID = CRC32_COMPUTE(nameof(ManagedObject));
    constexpr static const char* s_TypeName = nameof(ManagedObject);
    static ManagedClassDescriptor s_TypeDescriptor;

    friend class RuntimeObjectRegistry;

    InstanceID m_InstanceID = DefaultInstanceID;

    friend class RuntimeObjectRegistry;

public:
    InstanceID GetInstanceID() const { return m_InstanceID; }
    constexpr static U64 DefaultInstanceID = std::numeric_limits<U64>::max();

    using self = ManagedObject;
    virtual ~ManagedObject() = default;
    static void RegisterMembers();
    NODISCARD virtual Bool IsSerializable() const;
    NODISCARD static Bool ClassIsSerializable();
    NODISCARD virtual String ToString() const;
    NODISCARD virtual String GetTypeName() const;
    NODISCARD virtual CRC32 GetTypeID() const;
    NODISCARD static CRC32 GetClassTypeID();
    NODISCARD virtual UnsafeHandle<ManagedClassDescriptor> GetDescriptor() const;
    NODISCARD static UnsafeHandle<ManagedClassDescriptor> GetClassDescriptor();
    virtual void Serialize(YAML::Node& out) const;
    virtual void Deserialize(const YAML::Node& in);
    virtual void DestroyChildObjects() {}

};

template <typename T> requires std::is_abstract_v<T>
T* ConstructRawConditional()
{
    return nullptr;
}

template <typename T> requires (!std::is_abstract_v<T>)
T* ConstructRawConditional()
{
    return new T;
}

using StaticConstructType = ManagedObject* (*)();

#define MANAGED_CLASS(className, superClass, isSerializable) \
    private:                                                  \
    static className* StaticConstruct() { return ConstructRawConditional<className>(); } \
    private:                                      \
    constexpr static Bool s_IsSerializable = (isSerializable); \
    constexpr static CRC32 s_TypeID = CRC32_COMPUTE(nameof(className)); \
    constexpr static CRC32 s_BaseID = CRC32_COMPUTE(nameof(superClass)); \
    constexpr static const char* s_TypeName = nameof(className);        \
    inline static const ManagedClassDescriptor s_TypeDescriptor = ManagedClassDescriptor( \
        s_TypeName, \
        s_BaseID, \
        reinterpret_cast<StaticConstructType>(reinterpret_cast<void*>(StaticConstruct)) \
    );\
    public:                                                   \
    using base = superClass;                                \
    using self = className; \
    static void RegisterMembers(); \
    NODISCARD virtual Bool IsSerializable() const override { return s_IsSerializable; } \
    NODISCARD static Bool ClassIsSerializable() { return s_IsSerializable; } \
    NODISCARD virtual String GetTypeName() const override { return s_TypeName; } \
    NODISCARD virtual CRC32 GetTypeID() const override { return s_TypeID; } \
    NODISCARD static CRC32 GetClassTypeID() { return s_TypeID; }        \
    NODISCARD virtual UnsafeHandle<ManagedClassDescriptor> GetDescriptor() const override { return UnsafeHandle<ManagedClassDescriptor>(const_cast<ManagedClassDescriptor*>(&s_TypeDescriptor)); } \
    NODISCARD static UnsafeHandle<ManagedClassDescriptor> GetClassDescriptor() { return UnsafeHandle<ManagedClassDescriptor>(const_cast<ManagedClassDescriptor*>(&s_TypeDescriptor)); } \
    private:

template <typename T, typename ... Args> requires std::is_base_of_v<ManagedObject, T>
Reference<T> MakeManaged(Args&& ... args)
{
    return std::make_shared<T>(std::forward<Args>(args) ...);
}

#define DECLARE_SERIALIZE() \
    static_assert(s_IsSerializable);                        \
    virtual void Serialize(YAML::Node& out) const override; \
    virtual void Deserialize(const YAML::Node& in) override;

#define BEGIN_IMPLEMENT_SERIALIZE() \
    {                               \
        YAML::Node superNode;       \
        base::Serialize(superNode);\
        out["Super"] = superNode;   \
        out["TypeID"] = GetClassTypeID(); \
    }

#define END_IMPLEMENT_SERIALIZE()

#define SERIALIZE_REFERENCE(ref) \
    { \
        YAML::Node refOut;       \
        ref->Serialize(refOut);  \
        out[#ref] = refOut;      \
    }

#define SERIALIZE_PLAIN(field) \
    {                          \
        out[#field] = field;   \
    }

#define SERIALIZE_ENUM(field) \
    {                          \
        out[#field] = (U64)field;   \
    }

#define BEGIN_IMPLEMENT_DESERIALIZE() \
    {                                 \
        base::Deserialize(in["Super"]); \
        if (in["TypeID"].as<CRC32>() != GetClassTypeID()) \
        { \
            Assert(false, "Type mismatch"); \
        } \
    }

#define END_IMPLEMENT_DESERIALIZE()

#define DESERIALIZE_PLAIN(field) \
    {                          \
        field = in[#field].as<decltype(field)>(); \
    }

#define DESERIALIZE_REFERENCE(ref) \
    {                              \
        ref = Serializer::Deserialize<typename decltype(ref)::element_type>(in[#ref]); \
    }

#define DESERIALIZE_ENUM(field) \
    {                           \
        field = (decltype(field))in[#field].as<U64>(); \
    }

heliumEnd
