#include "Helium/HeliumPrecompiled.h"

#include "MonoRuntime.h"
#include "Helium/CoreGame/Debug.h"
#include "Helium/ObjectModel/ManagedObject.h"
#include "Helium/CoreGame/Actor.h"
#include "Helium/CoreGame/Transform.h"

#include <mono/metadata/mono-config.h>

heliumBegin

MonoRuntime::MonoRuntime()
{
    if (s_Instance)
    {
        Assert(false, "MonoRuntime already exists!");
    }
    mono_set_dirs("/Library/Frameworks/Mono.framework/Home/lib", "/Library/Frameworks/Mono.framework/Home/etc");
    mono_config_parse(nullptr);
    m_Domain = mono_jit_init("Helium");
    // get all assemblies in folder
    std::string path = "Managed/bin/";
    std::string extension = ".dll";
    // get all files in folder
    for (auto &p : std::filesystem::recursive_directory_iterator(path))
    {
        if (p.path().extension() == extension)
        {
            std::string assemblyPath = p.path().string();
            MonoAssembly* assembly = mono_domain_assembly_open(m_Domain, assemblyPath.c_str());
            if (assembly == nullptr)
            {
                // mono print why the assembly failed to load
                spdlog::error("Failed to load assembly: {}", assemblyPath);
                continue;
            }
            else
            {
                spdlog::info("Loaded assembly: {}", assemblyPath);
            }
            std::string stem = p.path().stem().string();
            if (stem == "HeliumManaged")
            {
                m_MainAssembly = assembly;
            }
            m_LoadedAssemblies.left.insert({stem, assembly});
        }
    }
    Assert(m_MainAssembly != nullptr);

    // register internal calls
    Debug::RegisterInternalCalls();
    ManagedObject::RegisterInternalCalls();
    Actor::RegisterInternalCalls();
    Transform::RegisterInternalCalls();
    s_Instance = this;
}

MonoRuntime::~MonoRuntime()
{
    mono_jit_cleanup(m_Domain);
}

MonoRuntime* MonoRuntime::GetInstance()
{
    return s_Instance;
}

MonoAssembly* MonoRuntime::GetMainAssembly() const
{
    return m_MainAssembly;
}

heliumEnd

