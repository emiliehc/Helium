#include "Helium/HeliumPrecompiled.h"

#include "ActorInspectorWindow.h"
#include "Helium/Serialization/Serializer.h"
#include "Helium/Reflection/ManagedPropertyDescriptor.h"
#include "Helium/CoreGame/Actor.h"
#include "Helium/CoreGame/Transform.h"
#include "Helium/ImGui/ObjectInspector.h"
#include "Helium/Reflection/TypeRegistry.h"

#include <utility>
#include <imgui.h>
#include <imgui_internal.h>


heliumBegin

ActorInspectorWindow::ActorInspectorWindow()
{
	MESSAGE_SUBSCRIBE(ActorSelectedMessage, MESSAGE_LISTENER(OnActorSelected));
}

ActorInspectorWindow::~ActorInspectorWindow()
{
	MESSAGE_UNSUBSCRIBE(ActorSelectedMessage, MESSAGE_LISTENER(OnActorSelected));
}

ActorInspectorWindow::ActorInspectorWindow(Handle<Actor> actor)
    : self()
{
	SetActor(actor);
}

static String LabelPrefix(const char* const label)
{
	const float width = ImGui::CalcItemWidth();

	const float x = ImGui::GetCursorPosX();
	ImGui::Text(label);
	ImGui::SameLine();
	ImGui::SetCursorPosX(x + width * 0.5f + ImGui::GetStyle().ItemInnerSpacing.x);
	ImGui::SetNextItemWidth(-1);

	return "##"s + label;
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
				const String className = component->GetDescriptor()->GetClassName();
				const String id = "##"s + className;
				const String contextMenuID = "##Menu"s + className;
				
				if (ImGui::CollapsingHeader(className.c_str()))
				{
					ImGui::OpenPopupOnItemClick(contextMenuID.c_str(), ImGuiPopupFlags_MouseButtonRight);
					if (ImGui::BeginPopup(contextMenuID.c_str()))
					{
						if (component->GetDescriptor() != Transform::GetClassDescriptor())
						{
							if (ImGui::MenuItem("Delete Component"))
							{
								m_Actor->RemoveComponentByTypeID(component->GetTypeID());
							}
						}
						ImGui::EndPopup();
					}
					ObjectInspector::Inspect(component);
				}
			}
		}

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponentPopup");
		}
		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			std::vector<UnsafeHandle<ManagedClassDescriptor>> descriptors = TypeRegistry::GetInstance()->GetAddableActorComponents();
			for (UnsafeHandle<ManagedClassDescriptor> descriptor : descriptors)
			{
				if (ImGui::MenuItem(descriptor->GetClassName().c_str()))
				{
					m_Actor->AddComponentByTypeID(descriptor->GetClassID());
				}
			}
			ImGui::EndPopup();
		}
	}

	ImGui::End();
}

void ActorInspectorWindow::OnActorSelected(const ActorSelectedMessage& message)
{
	SetActor(message.Actor);
}

heliumEnd


