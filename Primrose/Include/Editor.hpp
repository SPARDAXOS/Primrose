#pragma once
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw.h"
#include "ImGUI/imgui_impl_opengl3.h"

#include "Window.hpp"
#include <iostream>


class Editor final {
public:
	Editor() = delete;
	Editor(Window& window, EntityComponentSystem& ecs, TextureStorage& textureStorage, FileSystem& fileSystem)
		: m_WindowReference(&window), m_ECSReference(&ecs), m_TextureStorageReference(&textureStorage), m_FileSystemReference(&fileSystem)
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

		return true;
	}

public:
	void SaveEngineTexturesReferences() {

		if (!m_TextureStorageReference->GetTexture2D("Folder", m_FolderTexture))
			PrintMessage("Failed to save reference to engine texture [Folder]");

	}

private:
	void UpdateMenus() {//TODO: REturn const to this after getting rid of the text transform

		StartFrame();
		m_IsAnyWindowHovered = false;

		SetupEditorStyle();
		

		//TODO: All the menus sizes and positions are relative to each other. Make sure to make the values used relative to each other instead of relying on literals
		RenderDetailsMenu();
		RenderAddMenu();
		RenderHeirarchyMenu();
		RenderDirectoryExplorer();
		RenderContentBrowser();

		//TODO: Move into function of some sort - Needs to be after the render calls otherwise its always false cause the flag is reseted right on top of this function
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && m_SelectedGameObject != nullptr && !m_IsAnyWindowHovered) {
			//The only problem with this is that the IsAnyWindowHovered will not catch windows if a popup is open
			SetSelectedGameObject(nullptr);
		}

		ClearEditorStyle();

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

	void SetupEditorStyle() {

		ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.3f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.05f, 0.0f, 0.0f, 1.0f));

		ImGui::PushStyleColor(ImGuiCol_CheckMark, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.0f, 1.0f));
	}
	void ClearEditorStyle() {

		//12 - Make sure its the same amount of PushStyleColor() calls in SetupEditorStyle();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	void SetupContentBrowserStyle() {

		//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.0f, 0.0f, 0.5f));
		//ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.0f, 0.0f, 0.7f));
		//ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.0f, 0.0f, 0.5f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
	}
	void ClearContentBrowserStyle() {

		//3 - Make sure its the same amount of PushStyleColor() calls in SetupContentBrowserStyle();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
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

		ImGui::Begin("Details");
		CheckForHoveredWindows();

		if (m_SelectedGameObject != nullptr) {

			//TODO: Abstract these into separate functions for clarity

			////////////
			//Info
			////////////
			ImGui::Text("Name");
			ImGui::SameLine(50.0f);
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputText("##ObjectName", m_NameInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
				m_SelectedGameObject->SetName(m_NameInputBuffer);
			}

			ImGui::SameLine(175.0f);
			bool Enabled = m_SelectedGameObject->GetEnabled();
			if (ImGui::Checkbox("Enabled", &Enabled)) {
				m_SelectedGameObject->SetEnabled(Enabled);
			}

			ImGui::SameLine(300.0f);
			bool Static = m_SelectedGameObject->GetStatic();
			if (ImGui::Checkbox("Static", &Static)) {
				m_SelectedGameObject->SetStatic(Static);
			}

			ImGui::Text("Tag");
			ImGui::SameLine(50.0f);
			ImGui::SetNextItemWidth(100.0f);
			if (ImGui::InputText("##ObjectTag", m_TagInputBuffer, 32, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
				m_SelectedGameObject->SetTag(m_TagInputBuffer);
			}



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


			////////////
			//AddComponent
			////////////
			//Always at the bottom
			ImGui::Separator();
			AddSpacings(6);
			ImGui::SameLine(150);
			if (ImGui::Button("Add Component", ImVec2(100.0f, 20.0f))) {
				ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75, ImGui::GetMousePos().y }; //75 - Half box width
				ImGui::SetNextWindowPos(PopupPosition);
				ImGui::OpenPopup("AddComponentMenu");
			}

			if (ImGui::BeginPopup("AddComponentMenu")) {
				
				if (ImGui::Selectable("SpriteRenderer", false, 0, ImVec2(150, 20))) {
					m_SelectedGameObject->AddComponent<SpriteRenderer>();
				}
				else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
					m_SelectedGameObject->AddComponent<Camera>();
				}

				ImGui::EndPopup();
			}
			//Popup with options
		}

		ImGui::End();
	}
	void RenderHeirarchyMenu() {

		ImGui::SetNextWindowSize(ImVec2(200.0f, m_GUIViewport->Size.y - 350.0f)); // 50.0f Size of Add Menu
		ImGui::SetNextWindowPos(ImVec2(0.0f, 50.0f)); // 200 Size of Add Menu

		ImGui::Begin("Heirarchy");
		CheckForHoveredWindows();

		//TODO: This needs cleanup, especially the id check down there. Like test if the ID is reserved or something
		std::vector<GameObject*> GameObjects = m_ECSReference->GetGameObjects();
		for (uint32 i = 0; i < GameObjects.size(); i++) {
			if (GameObjects.at(i)->GetObjectID() != VIEWPORT_CAMERA_OBJECT_ID && GameObjects.at(i)->GetParent() == nullptr)
				AddHeirarchyEntry(GameObjects.at(i));
		}

		ImGui::End();
	}
	void RenderAddMenu() { //Terrible name
		ImGui::SetNextWindowSize(ImVec2(200.0f, 50.0f)); //Size of Add Menu
		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		bool True = true;
		ImGui::Begin("AddMenu", &True, ImGuiWindowFlags_NoTitleBar);
		CheckForHoveredWindows();

		AddSpacings(2);
		if (ImGui::Button("Add", ImVec2(50.0f, 20.0f))) {
			ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75, ImGui::GetMousePos().y }; //75 - Half box width
			ImGui::SetNextWindowPos(PopupPosition);
			ImGui::OpenPopup("AddGameObjectMenu");
		}

		if (ImGui::BeginPopup("AddGameObjectMenu")) {

			if (ImGui::Selectable("Empty GameObject", false, 0, ImVec2(150, 20))) {
				m_ECSReference->CreateGameObject();
			}
			else if (ImGui::Selectable("Cube", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECSReference->CreateGameObject("Cube");
				Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}
			else if (ImGui::Selectable("Sprite", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECSReference->CreateGameObject("Sprite"); //This breaks for some reason
				Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}
			else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECSReference->CreateGameObject("Camera"); //This breaks for some reason
				Object->AddComponent<Camera>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}
			

			ImGui::EndPopup();
		}

		ImGui::End();
	}
	void RenderDirectoryExplorer() {

		ImGui::SetNextWindowSize(m_DirectoryExplorerWindowSize); 
		ImGui::SetNextWindowPos(ImVec2(0.0f, m_GUIViewport->Size.y - m_DirectoryExplorerWindowSize.y));

		bool Open = true;
		ImGuiWindowFlags Flags = 0;
		Flags |= ImGuiWindowFlags_NoCollapse;
		Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

		ImGui::Begin("Directory Explorer", &Open, Flags);
		CheckForHoveredWindows();

		auto Root = m_FileSystemReference->GetRoot();
		if (m_SelectedDirectory == nullptr)
			m_SelectedDirectory = Root;

		AddFileExplorerEntry(Root);
		
		ImGui::End();
	}
	void RenderContentBrowser() {

		ImGui::SetNextWindowSize(m_ContentBrowserWindowSize);
		ImGui::SetNextWindowPos(ImVec2(m_DirectoryExplorerWindowSize.x, m_GUIViewport->Size.y - m_ContentBrowserWindowSize.y));

		bool Open = true;
		ImGuiWindowFlags Flags = 0;
		Flags |= ImGuiWindowFlags_NoCollapse;
		Flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
		Flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;

		ImGui::Begin("Content Browser", &Open, Flags);
		CheckForHoveredWindows();

		//NOTE: Use m_SelectedDirectory to figure out what to draw.


		//NOTE: Ideally, you would calculate the position and sizes depending on multiple factors such as name size and window size.
		//NOTE: It should be calculated depending on the resizing of the window
		//NOTE: The index 0 is the texture handle from opengl

		if (m_SelectedDirectory != nullptr) {

			SetupContentBrowserStyle();

			//Reset 
			NewContentBrowserFrame();
			UpdateContentBrowserFolderEntries();
			UpdateContentBrowserAssetEntries();

			//This needs major reworks. 
			//There are many cases where this will break.
			//The book keeping is the problem
			//Cases:
			//1. Folders finishes and its at the end of a line
			//2. Folders finishes and its in the middle of a line

			//Text buffers become a problem cause if its at then end of a line, Assets needs to create a new line otherwise not!

			//There is a line spacing problem that i fixed temporarily with a hack pretty much
			//The other problem is that the text flushers dont keep track of elements per line and expect to always fill up a full line!
			//They always start from 0 to count 



			//Solutions
			//Make one shared buffer of texts for both of them
			//It flushes under 2 conditions
			//1. Its the end of the line
			//2. If it is not empty and all rendering is over
			//This should fix many problems and reduce code by a lot
			//Every time a flush is needed or called, it would be on a line with no texts so no need to guess whether a space is needed or not

			//m_QueuedContentTexts

			if (m_QueuedContentTexts.size() > 0)
				FlushContentTexts();

			//If list not empty yet then flush it!
			//if (m_QueuedRenderFolderTexts.size() > 0)
			//	FlushFolderTexts((uint32)m_QueuedRenderFolderTexts.size());

			////If list not empty yet then flush it!
			//if (m_QueuedRenderAssetTexts.size() > 0)
			//	FlushAssetTexts((uint32)m_QueuedRenderAssetTexts.size(), false);


			ClearContentBrowserStyle();
		}

		ImGui::End();
	}


private:
	//They should be const since they shouldnt change anything
	void AddHeirarchyEntry(GameObject* entry) {

		if (entry == nullptr)
			return;

		ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
		bool IsLeaf = false;
		if (!entry->HasChildren()) {
			Flags |= ImGuiTreeNodeFlags_Leaf;
			IsLeaf = true;
		}

		if (entry == m_SelectedGameObject)
			Flags |= ImGuiTreeNodeFlags_Selected;

		uint32 PushedStyles = 0;
		if (!entry->GetActiveInHeirarchy()) {
			PushedStyles++;
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::TreeNodeEx(entry->GetName().data(), Flags)) {

			if (ImGui::IsItemClicked()) {
				SetSelectedGameObject(entry);
			}
			else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
				ImGui::OpenPopup("EditMenu");
				SetSelectedGameObject(entry);
			}

			//EditMenu
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

				ImGui::Text("Are you sure?");
				ImGui::Spacing();

				ImGui::PushItemWidth(40.0f);
				if (ImGui::Button("Yes")) {
					if (entry == m_SelectedGameObject) //Note: Any pointers to objects like these need to be reseted to nullptr somehow.
						SetSelectedGameObject(nullptr);
					//m_SelectedGameObject = nullptr;

					entry->Destroy();
					ImGui::EndPopup();
					ImGui::TreePop();
					return; //It would break otherwise?
				}
				ImGui::SameLine(80.0f);

				ImGui::PushItemWidth(40.0f);
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

			//Parent-Child
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

			if (!IsLeaf) {
				if (PushedStyles > 0) {
					ImGui::PopStyleVar();
					PushedStyles--;
				}

				for (auto& x : entry->GetChildren())
					AddHeirarchyEntry(x);
			}

			ImGui::TreePop();
		}

		if (PushedStyles > 0) {
			ImGui::PopStyleVar();
			PushedStyles--;
		}
	}
	void AddFileExplorerEntry(Directory* entry) {

		if (entry == nullptr)
			return;

		ImGuiTreeNodeFlags Flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
		bool IsLeaf = false;
		if (entry->m_Folders.size() == 0) {
			Flags |= ImGuiTreeNodeFlags_Leaf;
			IsLeaf = true;
		}

		if (entry == m_SelectedDirectory)
			Flags |= ImGuiTreeNodeFlags_Selected;

		if (ImGui::TreeNodeEx(entry->m_Path.filename().string().data(), Flags)) {

			if (ImGui::IsItemClicked()) {
				SetSelectedDirectory(entry);
			}

			if (!IsLeaf) {
				for (auto& x : entry->m_Folders)
					AddFileExplorerEntry(x);
			}

			ImGui::TreePop();
		}

	}

private:
	void NewContentBrowserFrame() {
		ContentElementCursor = 0.0f;
		ContentLineElementsCount = 0;
	}
	void UpdateContentBrowserFolderEntries() {

		//TODO: Implement some countermeasure in case the texture was not found
		//Bind texture
		void* TextureID = nullptr;
		if (m_FolderTexture) {
			m_FolderTexture->Bind();
			TextureID = (void*)(intptr_t)(m_FolderTexture->GetID());
		}

		for (auto& Folder : m_SelectedDirectory->m_Folders) {

			std::string Name = Folder->m_Path.filename().replace_extension().string();
			std::string ID = "##" + Name;

			//Calculate position and check if new line is necessary
			ContentElementCursor = (m_ContentBrowserElementPadding * (ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * ContentLineElementsCount;
			if (ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowSize.x) {
				FlushContentTexts();
				ContentElementCursor = m_ContentBrowserElementPadding;
				ContentLineElementsCount = 0; //This means its a new line!
			}

			//Render content
			ImGui::SameLine(ContentElementCursor);
			if (ImGui::ImageButton(ID.data(), TextureID, m_ContentBrowserElementSize)) {
				m_SelectedDirectory = Folder;
				break;
			}

			//Book keeping
			m_QueuedContentTexts.emplace_back(Name);

			ContentLineElementsCount++;
		}

		//Unbind texture
		if (m_FolderTexture)
			m_FolderTexture->Unbind();
	}
	void UpdateContentBrowserAssetEntries() {

		for (auto& Asset : m_SelectedDirectory->m_Assets) {

			//TODO: Deal with in case it wasnt found!
			void* TextureID = nullptr;
			const Texture2D* IconTexture = GetIconTexture(*Asset);
			if (IconTexture != nullptr) {
				TextureID = (void*)(intptr_t)(IconTexture->GetID());
				IconTexture->Bind();
			}


			std::string Name = Asset->m_Path.filename().replace_extension().string();
			std::string ID = "##" + Name;

			//Calculate position and check if new line is necessary
			ContentElementCursor = (m_ContentBrowserElementPadding * (ContentLineElementsCount + 1)) + m_ContentBrowserElementSize.x * ContentLineElementsCount;
			if (ContentElementCursor + m_ContentBrowserElementSize.x >= m_ContentBrowserWindowSize.x) {
				
				FlushContentTexts();
				ContentElementCursor = m_ContentBrowserElementPadding;
				ContentLineElementsCount = 0; //This means its a new line!
			}

			//Render content
			ImGui::SameLine(ContentElementCursor);
			if (ImGui::ImageButton(ID.data(), TextureID, m_ContentBrowserElementSize)) {

			}

			m_QueuedContentTexts.emplace_back(Name);
			ContentLineElementsCount++;

			if (IconTexture != nullptr)
				IconTexture->Unbind();
		}
	}
	void FlushContentTexts() {

		AddSpacings(1);

		float CurrentX = 0.0f;

		for (uint32 Index = 0; Index < m_QueuedContentTexts.size(); Index++) {

			std::string ElementName = m_QueuedContentTexts.at(Index);

			ImVec2 TextSize = ImGui::CalcTextSize(ElementName.data());
			CurrentX = (m_ContentBrowserElementPadding * (Index + 1)) + m_ContentBrowserElementSize.x * Index; //Same pos as content
			CurrentX += m_ContentBrowserElementSize.x / 2; //Shift it by button half button size
			CurrentX -= TextSize.x / 2; //Shift it back by text half width

			ImGui::SameLine(CurrentX);
			ImGui::Text(ElementName.data());
		}

		m_QueuedContentTexts.clear();
		AddSpacings(2);
	}

private:
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
	void AddSpacings(uint32 count) {
		for (uint32 index = 0; index < count; index++)
			ImGui::Spacing();
	}
	void SetSelectedGameObject(GameObject* object) noexcept {

		m_SelectedGameObject = object;
		if (m_SelectedGameObject != nullptr) {
			strcpy_s(m_NameInputBuffer, m_SelectedGameObject->GetName().c_str());
			strcpy_s(m_TagInputBuffer, m_SelectedGameObject->GetTag().c_str());
		}
	}
	void SetSelectedDirectory(Directory* directory) noexcept {

		m_SelectedDirectory = directory;
	}
	void CheckForHoveredWindows() noexcept {

		if (ImGui::IsWindowHovered())
			m_IsAnyWindowHovered = true;
	}
	Texture2D* GetIconTexture(Asset& asset) {

		Texture2D* IconTexture;

		//TODO: Set the alpha blending for these icons cause transparency is borked currenrtly

		switch (asset.m_Type)
		{
		case AssetType::TEXTURE: {

			//TODO: Add name to the asset and remove it from Texture2D and update that everywhere! It makes more sense and all asset types will have it
			if (!m_TextureStorageReference->GetTexture2D(asset.m_Path.filename().replace_extension().string(), IconTexture))
				return nullptr;
			return IconTexture;
		}
		default:
			if (m_TextureStorageReference->GetTexture2D("Unknown", IconTexture))
				return IconTexture;
			return nullptr;
		}
	}

private:
	GameObject* m_SelectedGameObject;
	Directory* m_SelectedDirectory;
	char m_NameInputBuffer[33];
	char m_TagInputBuffer[33];
	bool m_IsAnyWindowHovered{ false };

private: //Most of these are relative to each other. Calculate them in runtime and maybe once at the start. Think about this.
	ImVec2 m_ContentBrowserWindowSize { 1170.0f, 300.0f };
	ImVec2 m_ContentBrowserElementSize{ 100.0f, 100.0f };
	float m_ContentBrowserElementPadding = 50.0f;

	ImVec2 m_DirectoryExplorerWindowSize{ 350.0f, 300.0f };

private:
	Texture2D* m_FolderTexture;
	float ContentElementCursor = 0.0f;
	uint32 ContentLineElementsCount = 0;
	std::vector<std::string> m_QueuedContentTexts;

private:
	Window* m_WindowReference;
	EntityComponentSystem* m_ECSReference;
	TextureStorage* m_TextureStorageReference;
	FileSystem* m_FileSystemReference;

private:
	ImGuiContext* m_GUIContext;
	ImGuiViewport* m_GUIViewport;
	ImGuiIO* m_IO;
};