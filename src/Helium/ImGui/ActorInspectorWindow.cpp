#include "Helium/HeliumPrecompiled.h"

#include "ActorInspectorWindow.h"
#include "Helium/Serialization/Serializer.h"
#include "Helium/Reflection/ManagedPropertyDescriptor.h"
#include "Helium/CoreGame/Actor.h"
#include "Helium/CoreGame/Transform.h"
#include "Helium/ImGui/ObjectInspector.h"

#include <utility>
#include <imgui.h>
#include <imgui_internal.h>

heliumBegin

ActorInspectorWindow::ActorInspectorWindow(Handle<Actor> actor)
    : m_Actor(actor)
{
}

static String LabelPrefix(const char* const label)
{
	float width = ImGui::CalcItemWidth();

	float x = ImGui::GetCursorPosX();
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::SetNextItemWidth(-1);

	std::string labelID = "##";
	labelID += label;

	return labelID;
}

void ActorInspectorWindow::OnGUIUpdate(float deltaTime)
{
    if (!m_IsOpen)
    {
        return;
    }

	if (ImGui::Begin("Inspector", &m_IsOpen))
	{
		if (m_Actor)
		{
			for (Handle<ActorComponent> component : m_Actor->m_ComponentStore.m_Components)
			{
				if (ImGui::CollapsingHeader(component->GetDescriptor()->GetClassName().c_str()))
				{
					ObjectInspector::Inspect(component);
				}
			}
		}
	}

	ImGui::End();
}

heliumEnd


