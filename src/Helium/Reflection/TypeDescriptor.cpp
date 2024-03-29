#include "Helium/HeliumPrecompiled.h"

#include "TypeDescriptor.h"

#include <utility>

#include "Helium/Reflection/ManagedPropertyDescriptor.h"

heliumBegin

ManagedClassDescriptor::ManagedClassDescriptor(const String& className, CRC32 baseID, std::function<ManagedObject*()> factory) : m_ClassName(className)
    , m_ClassID(CRC32Compute(className.c_str(), className.length()))
    , m_BaseID(baseID)
    , m_Factory(std::move(factory))
{
}

String ManagedClassDescriptor::GetClassName() const
{
    return m_ClassName;
}

CRC32 ManagedClassDescriptor::GetClassID() const
{
    return m_ClassID;
}

ManagedObject* ManagedClassDescriptor::CreateInstance() const
{
    return m_Factory();
}

NODISCARD CRC32 ManagedClassDescriptor::GetBaseID() const
{
    return m_BaseID;
}

void ManagedClassDescriptor::AddProperty(StringView propertyName, PropertyType propertyType, TypeErasedGetAccessor getter, std::optional<TypeErasedSetAccessor> setter, std::variant<nullptr_t, UnsafeHandle<ManagedClassDescriptor>, UnsafeHandle<ManagedEnumDescriptor>> descriptor)
{
    m_Properties.emplace_back(new ManagedPropertyDescriptor(this, propertyName, propertyType, getter, setter, descriptor));
}

std::vector<UnsafeHandle<ManagedPropertyDescriptor>> ManagedClassDescriptor::GetProperties() const
{
    return m_Properties;
}

ManagedEnumDescriptor::ManagedEnumDescriptor(const String& enumName)
    : m_EnumName(enumName)
    , m_EnumID(CRC32Compute(enumName.c_str(), enumName.length()))
{

}

String ManagedEnumDescriptor::GetEnumName() const
{
    return m_EnumName;
}

CRC32 ManagedEnumDescriptor::GetEnumID() const
{
    return m_EnumID;
}

Size ManagedEnumDescriptor::GetEnumValueCount() const
{
    return m_EnumValues.size();
}

NODISCARD std::vector<std::pair<String, U64>> ManagedEnumDescriptor::GetEnumPairs() const
{
    std::vector<std::pair<String, U64>> pairs;
    for (const auto& pair : m_EnumValues)
    {
		pairs.emplace_back(pair.right, pair.left);
	}
	return pairs;
}

heliumEnd
