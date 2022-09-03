#pragma once

#include "Helium/HeliumPrecompiled.h"
#include "VertexBufferLayout.h"

heliumBegin

class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    NODISCARD virtual const VertexBufferLayout& GetLayout() const = 0;
};

heliumEnd