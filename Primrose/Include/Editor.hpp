#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Window.hpp"
#include <iostream>


class Editor final {
public:
	Editor() = delete;
	Editor(Window& window, EntityComponentSystem& ecs)
		: m_WindowReference(&window), m_ECSReference(&ecs)
	{
		IMGUI_CHECKVERSION();
		m_GUIContext = ImGui::CreateContext();
		m_GUIViewport = ImGui::GetMainViewport();

		m_IO = &ImGui::GetIO();
		m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		m_IO->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// IF using Docking Branch

		ImGui_ImplGlfw_InitForOpenGL(m_WindowReference->GetWindowResource().m_ptr, true);
		ImGui_ImplOpenGL3_Init();
	}
	~Editor() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	Editor(const Editor&) = delete;
	Editor& operator=(const Editor&) = delete;

	Editor(Editor&&) = delete;
	Editor& operator=(Editor&&) = delete;

public:
	[[nodiscard]] bool Update() {

		UpdateMenus();

		//Clear selected on right click

		return true;
	}

private:
	void UpdateMenus() {//TODO: REturn const to this after getting rid of the text transform

		StartFrame();
		
		RenderDetailsMenu();
		RenderHeirarchyMenu();

		RenderFrame();
	}
	void StartFrame() const {
		//Feed input and start GUI frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
	}
	void RenderFrame() const {
		//Render GUI to screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

private:
	void RenderDetailsMenu()  { //TODO: REturn const to this after getting rid of the text transform

		//Notes:
		//This could be abstracted away since DRY


		//TODO: Add the ability to reorder these one day
		//TODO: Move viewport related code to this class


		//Render GUI
		//Note: Will also lock position and size
		ImGui::SetNextWindowSize(ImVec2(400.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(m_GUIViewport->Size.x - 400.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		
		ImGui::Begin("Details");
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		
		//TODO: Should probably apply these somewhere before everything unless i want to do some spefic styling at some places

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.05f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (m_SelectedGameObject != nullptr) {


			//TODO: Abstract these into separate functions for clarity

			////////////
			//Transform
			////////////
			Transform* SelectedTransform = &m_SelectedGameObject->GetTransform();
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

				ImGui::Text("Position");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PX", &SelectedTransform->m_Position.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PY", &SelectedTransform->m_Position.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##PZ", &SelectedTransform->m_Position.m_Z);


				ImGui::Text("Rotation");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RX", &SelectedTransform->m_Rotation.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RY", &SelectedTransform->m_Rotation.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##RZ", &SelectedTransform->m_Rotation.m_Z);


				ImGui::Text("Scale");

				ImGui::SameLine(100.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				ImGui::Text("X");
				ImGui::PopStyleColor();

				ImGui::SameLine(120.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SX", &SelectedTransform->m_Scale.m_X);

				ImGui::SameLine(200.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				ImGui::Text("Y");
				ImGui::PopStyleColor();

				ImGui::SameLine(220.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SY", &SelectedTransform->m_Scale.m_Y);

				ImGui::SameLine(300.0f);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				ImGui::Text("Z");
				ImGui::PopStyleColor();

				ImGui::SameLine(320.0f);
				ImGui::PushItemWidth(50.0F);
				ImGui::InputFloat("##SZ", &SelectedTransform->m_Scale.m_Z);

			}
			

			////////////
			//SpriteRenderer
			////////////
			if (m_SelectedGameObject->HasComponent<SpriteRenderer>()) {
				SpriteRenderer* SelectedSpriteRenderer = m_SelectedGameObject->GetComponent<SpriteRenderer>();
				if (ImGui::CollapsingHeader("SpriteRenderer", ImGuiTreeNodeFlags_DefaultOpen)) {

					//TINT
					Color CurrentColor = SelectedSpriteRenderer->GetTint();
					if (ImGui::ColorEdit4("Tint", &CurrentColor.m_R, ImGuiColorEditFlags_NoInputs)) {
						SelectedSpriteRenderer->SetTint(CurrentColor);
					}
				
					ImGui::Spacing();
					ImGui::Spacing();

					//NOTE: this is getting weird with the sets and gets - Might be fine for the sake of using the setters to track changes to values
					//FLIP
					bool FlipX = SelectedSpriteRenderer->GetFlipX();
					bool FlipY = SelectedSpriteRenderer->GetFlipY();
					if (ImGui::Checkbox("FlipX", &FlipX)) {
						SelectedSpriteRenderer->SetFlipX(FlipX);
					}
					ImGui::SameLine(100);
					if (ImGui::Checkbox("FlipY", &FlipY)) {
						SelectedSpriteRenderer->SetFlipY(FlipY);
					}


					ImGui::Spacing();
					ImGui::Spacing();
					//BLEND EQUATION
					ImGui::PushItemWidth(200.0f);
					if (ImGui::BeginCombo("Blend Equation", EnumToText::ToText(SelectedSpriteRenderer->GetBlendEquation()))) {
						
						if (ImGui::Selectable("Additive", false)) {
							SelectedSpriteRenderer->SetBlendEquation(BlendEquation::ADDITIVE);
						}
						if (ImGui::Selectable("Subtractive", false)) {
							SelectedSpriteRenderer->SetBlendEquation(BlendEquation::SUBTRACTIVE);
						}
						if (ImGui::Selectable("Reverse Subtractive", false)) {
							SelectedSpriteRenderer->SetBlendEquation(BlendEquation::REVERSE_SUBTRACTIVE);
						}

						ImGui::EndCombo();
					}

					//SOURCE BLEND MODE
					ImGui::PushItemWidth(200.0f);
					if (ImGui::BeginCombo("Source Blend Mode", EnumToText::ToText(SelectedSpriteRenderer->GetSourceBlendMode()))) {

						if (AddBlendModeSelectable(SourceBlendMode::ZERO)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ZERO);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_SOURCE_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_SOURCE_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::DESTINATION_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::DESTINATION_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_DESTINATION_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_DESTINATION_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_SOURCE_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_SOURCE_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::DESTINATION_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::DESTINATION_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_DESTINATION_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_DESTINATION_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::CONSTANT_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::CONSTANT_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_CONSTANT_COLOR)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_CONSTANT_COLOR);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::CONSTANT_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::CONSTANT_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::ONE_MINUS_CONSTANT_ALPHA)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::ONE_MINUS_CONSTANT_ALPHA);
						}
						else if (AddBlendModeSelectable(SourceBlendMode::SOURCE_ALPHA_SATURATE)) {
							SelectedSpriteRenderer->SetSourceBlendMode(SourceBlendMode::SOURCE_ALPHA_SATURATE);
						}

						ImGui::EndCombo();
					}

					//DESTINATION BLEND MODE
					ImGui::PushItemWidth(200.0f);
					if (ImGui::BeginCombo("Destination Blend Mode", EnumToText::ToText(SelectedSpriteRenderer->GetDestinationBlendMode()))) {

						if (AddBlendModeSelectable(DestinationBlendMode::ZERO)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ZERO);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::SOURCE_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::SOURCE_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_SOURCE_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_SOURCE_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::DESTINATION_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::DESTINATION_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_DESTINATION_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_DESTINATION_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::SOURCE_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::SOURCE_ALPHA);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_SOURCE_ALPHA);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::DESTINATION_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::DESTINATION_ALPHA);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_DESTINATION_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_DESTINATION_ALPHA);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::CONSTANT_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::CONSTANT_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_CONSTANT_COLOR)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_CONSTANT_COLOR);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::CONSTANT_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::CONSTANT_ALPHA);
						}
						else if (AddBlendModeSelectable(DestinationBlendMode::ONE_MINUS_CONSTANT_ALPHA)) {
							SelectedSpriteRenderer->SetDestinationBlendMode(DestinationBlendMode::ONE_MINUS_CONSTANT_ALPHA);
						}

						ImGui::EndCombo();
					}


					ImGui::Spacing();
					ImGui::Spacing();
					//ADDRESSING MODE
					ImGui::PushItemWidth(100.0f);
					if (ImGui::BeginCombo("Addressing Mode S", EnumToText::ToText(SelectedSpriteRenderer->GetAddressingModeS()))) {

						if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_BORDER)) {
							SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::CLAMPED_TO_BORDER);
						}
						else if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_EDGE)) {
							SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::CLAMPED_TO_EDGE);
						}
						else if (AddAddressingModeSelectable(AddressingMode::MIRRORED)) {
							SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::MIRRORED);
						}
						else if (AddAddressingModeSelectable(AddressingMode::REPEAT)) {
							SelectedSpriteRenderer->SetAddressingModeS(AddressingMode::REPEAT);
						}

						ImGui::EndCombo();
					}
					ImGui::PushItemWidth(100.0f);
					if (ImGui::BeginCombo("Addressing Mode T", EnumToText::ToText(SelectedSpriteRenderer->GetAddressingModeT()))) {

						if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_BORDER)) {
							SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::CLAMPED_TO_BORDER);
						}
						else if (AddAddressingModeSelectable(AddressingMode::CLAMPED_TO_EDGE)) {
							SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::CLAMPED_TO_EDGE);
						}
						else if (AddAddressingModeSelectable(AddressingMode::MIRRORED)) {
							SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::MIRRORED);
						}
						else if (AddAddressingModeSelectable(AddressingMode::REPEAT)) {
							SelectedSpriteRenderer->SetAddressingModeT(AddressingMode::REPEAT);
						}

						ImGui::EndCombo();
					}


					ImGui::Spacing();
					ImGui::Spacing();
					//FILTERING MODE
					ImGui::PushItemWidth(170.0f);
					if (ImGui::BeginCombo("Filtering Mode Min", EnumToText::ToText(SelectedSpriteRenderer->GetFilteringModeMin()))) {

						if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR);
						}
						else if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR_MIPMAP_LINEAR)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR_MIPMAP_LINEAR);
						}
						else if (AddFilteringModeMinSelectable(FilteringModeMin::LINEAR_MIPMAP_NEAREST)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::LINEAR_MIPMAP_NEAREST);
						}
						else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST);
						}
						else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST_MIPMAP_LINEAR)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST_MIPMAP_LINEAR);
						}
						else if (AddFilteringModeMinSelectable(FilteringModeMin::NEAREST_MIPMAP_NEAREST)) {
							SelectedSpriteRenderer->SetFilteringModeMin(FilteringModeMin::NEAREST_MIPMAP_NEAREST);
						}

						ImGui::EndCombo();
					}
					ImGui::PushItemWidth(170.0f);
					if (ImGui::BeginCombo("Filtering Mode Mag", EnumToText::ToText(SelectedSpriteRenderer->GetFilteringModeMag()))) {

						if (AddFilteringModeMagSelectable(FilteringModeMag::LINEAR)) {
							SelectedSpriteRenderer->SetFilteringModeMag(FilteringModeMag::LINEAR);
						}
						else if (AddFilteringModeMagSelectable(FilteringModeMag::NEAREST)) {
							SelectedSpriteRenderer->SetFilteringModeMag(FilteringModeMag::NEAREST);
						}

						ImGui::EndCombo();
					}


				}
			}
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::End();
	}
	void RenderHeirarchyMenu() {

		ImGui::SetNextWindowSize(ImVec2(200.0f, m_GUIViewport->Size.y));
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

		ImGui::Begin("Heirarchy");


		//TODO: This needs cleanup, especially the id check down there. Like test if the ID is reserved or something
		std::vector<GameObject*> GameObjects = m_ECSReference->GetGameObjects();
		for (uint32 i = 0; i < GameObjects.size(); i++) {
			if (GameObjects.at(i)->GetObjectID() != VIEWPORT_CAMERA_OBJECT_ID && GameObjects.at(i)->GetParent() == nullptr)
				AddHeirarchyEntry(GameObjects.at(i));
		}

		ImGui::End();
	}


private:
	void AddHeirarchyEntry(GameObject* entry) {

		if (entry == nullptr)
			return;

		bool IsLeaf = false;
		if (!entry->HasChildren())
			IsLeaf = true;

		//Refactor this func into becoming 1 part cause the only difference is the leaf flag as far as im aware
		if (IsLeaf) {
			if (ImGui::TreeNodeEx(entry->GetName().data(), ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_OpenOnArrow)) {

				if (ImGui::IsItemClicked()) {
					m_SelectedGameObject = entry;
				}
				else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("EditMenu");
					m_SelectedGameObject = entry;
				}

				bool Renaming = false; //Need this cause i think EndPopup() clears the rename popup id
				bool Deleting = false;
				if (ImGui::BeginPopup("EditMenu")) {

					ImGui::Text(std::string("Edit: " + entry->GetName()).data());

					if (ImGui::Selectable("Delete", false)) {
						ImGui::CloseCurrentPopup();
						Deleting = true;
					}
					if (ImGui::Selectable("Rename", false)) {
						ImGui::CloseCurrentPopup();
						Renaming = true;
					}

					ImGui::EndPopup();
				}

				//Deleting
				if (Deleting) {
					ImGui::OpenPopup("GODeleteConfirmation");
				}
				if (ImGui::BeginPopup("GODeleteConfirmation")) {
					if (ImGui::Button("Yes")) {
						if (entry == m_SelectedGameObject) //Note: Any pointers to objects like these need to be reseted to nullptr somehow.
							m_SelectedGameObject = nullptr;

						entry->Destroy();
						ImGui::EndPopup();
						ImGui::TreePop();
						return; //It would break otherwise?
					}
					if (ImGui::Button("No")) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				//Renaming
				if (Renaming) {
					strcpy_s(m_NameInputBuffer, entry->GetName().c_str());
					ImGui::OpenPopup("NameInput");
				}
				if (ImGui::BeginPopup("NameInput")) {
					if (ImGui::InputText("Name", m_NameInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
						entry->SetName(m_NameInputBuffer);
					}

					ImGui::EndPopup();
				}



				if (ImGui::BeginDragDropSource()) {
					auto PointerToReference = &entry;
					ImGui::SetDragDropPayload("HeirarchyEntry", PointerToReference, sizeof(PointerToReference));


					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget()) {

					auto Payload = ImGui::AcceptDragDropPayload("HeirarchyEntry");
					if (Payload != nullptr) {
						auto PointerToReference = static_cast<GameObject**>(Payload->Data);
						GameObject* PointerToEntry = *PointerToReference;
						entry->AddChild(PointerToEntry);
					}

					ImGui::EndDragDropTarget();
				}

				ImGui::TreePop();
			}
		}
		else {
			if (ImGui::TreeNodeEx(entry->GetName().data(), ImGuiTreeNodeFlags_OpenOnArrow)) {

				if (ImGui::IsItemClicked()) {
					m_SelectedGameObject = entry;

				}
				else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
					ImGui::OpenPopup("EditMenu");
					m_SelectedGameObject = entry;
				}

				bool Renaming = false; //Need this cause i think EndPopup() clears the rename popup id
				bool Deleting = false;
				if (ImGui::BeginPopup("EditMenu")) {

					ImGui::Text(std::string("Edit: " + entry->GetName()).data());

					if (ImGui::Selectable("Delete", false)) {
						ImGui::CloseCurrentPopup();
						Deleting = true;
					}
					if (ImGui::Selectable("Rename", false)) {
						ImGui::CloseCurrentPopup();
						Renaming = true;
					}

					ImGui::EndPopup();
				}

				//Deleting
				if (Deleting) {
					ImGui::OpenPopup("GODeleteConfirmation");
				}
				if (ImGui::BeginPopup("GODeleteConfirmation")) {
					if (ImGui::Button("Yes")) {
						if (entry == m_SelectedGameObject) //Note: Any pointers to objects like these need to be reseted to nullptr somehow.
							m_SelectedGameObject = nullptr;

						entry->Destroy();
						ImGui::EndPopup();
						ImGui::TreePop();
						return; //It would break otherwise?
					}
					if (ImGui::Button("No")) {
						ImGui::CloseCurrentPopup();
					}

					ImGui::EndPopup();
				}

				//Renaming
				if (Renaming) {
					strcpy_s(m_NameInputBuffer, entry->GetName().c_str());
					ImGui::OpenPopup("NameInput");
				}
				if (ImGui::BeginPopup("NameInput")) {
					if (ImGui::InputText("Name", m_NameInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
						entry->SetName(m_NameInputBuffer);
					}

					ImGui::EndPopup();
				}





				if (ImGui::BeginDragDropSource()) {
					auto PointerToReference = &entry;
					ImGui::SetDragDropPayload("HeirarchyEntry", PointerToReference, sizeof(PointerToReference));

					ImGui::EndDragDropSource();
				}
				if (ImGui::BeginDragDropTarget()) {

					auto Payload = ImGui::AcceptDragDropPayload("HeirarchyEntry");
					if (Payload != nullptr) {
						auto PointerToReference = static_cast<GameObject**>(Payload->Data);
						GameObject* PointerToEntry = *PointerToReference;
						entry->AddChild(PointerToEntry);
					}

					ImGui::EndDragDropTarget();
				}

				for (auto& x : entry->GetChildren())
					AddHeirarchyEntry(x);

				ImGui::TreePop();
			}
		}
	}

	bool AddBlendModeSelectable(SourceBlendMode mode) {
		using namespace EnumToText;

		if (ImGui::Selectable(ToText(mode), false))
			return true;
		else
			return false;
	}
	bool AddBlendModeSelectable(DestinationBlendMode mode) {
		using namespace EnumToText;

		if (ImGui::Selectable(ToText(mode), false))
			return true;
		else
			return false;
	}
	bool AddAddressingModeSelectable(AddressingMode mode) {
		using namespace EnumToText;

		if (ImGui::Selectable(ToText(mode), false))
			return true;
		else
			return false;
	}
	bool AddFilteringModeMinSelectable(FilteringModeMin mode) {
		using namespace EnumToText;

		if (ImGui::Selectable(ToText(mode), false))
			return true;
		else
			return false;
	}
	bool AddFilteringModeMagSelectable(FilteringModeMag mode) {
		using namespace EnumToText;

		if (ImGui::Selectable(ToText(mode), false))
			return true;
		else
			return false;
	}


private:
	GameObject* m_SelectedGameObject;
	char m_NameInputBuffer[33];

private:
	Window* m_WindowReference;
	EntityComponentSystem* m_ECSReference;

private:
	ImGuiContext* m_GUIContext;
	ImGuiViewport* m_GUIViewport;
	ImGuiIO* m_IO;
};