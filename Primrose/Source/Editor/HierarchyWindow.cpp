#include "Editor/HierarchyWindow.hpp"
#include "Systems/Core.hpp"
#include "Systems/Editor.hpp"
#include "Systems/EntityComponentSystem.hpp"
#include "Editor/ContentBrowser.hpp"
#include "Editor/DetailsWindow.hpp"

#include "GameObject.hpp"



HierarchyWindow::HierarchyWindow(Core& core, Editor& editor) noexcept 
	:	m_Core(&core), m_Editor(&editor)
{
	m_ECS = core.GetECS();
}


void HierarchyWindow::Update() {

	//Unselect from hieracrchy
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered() && m_SelectedGameObject != nullptr && !m_Editor->IsAnyWindowHovered()) {
		//The only problem with this is that the IsAnyWindowHovered will not catch windows if a popup is open
		ClearSelectedGameObject();
	}

	//Delete selected from hieracrchy
	if (ImGui::IsKeyReleased(ImGuiKey_Delete)) {

		//if (m_SelectedContentElement != nullptr) {
		//	//Shit...
		//}
		if (m_SelectedGameObject != nullptr) {
			m_SelectedGameObject->Destroy();
			ClearSelectedGameObject();
		}
	}
}
void HierarchyWindow::Render() {

	if (!m_Open)
		return;

	m_Size = ImVec2(m_ImGuiViewport->Size.x * 0.1f, m_ImGuiViewport->Size.y - m_ContentBrowser->GetDirectoryExplorerWindowSize().y - m_Size.y); //ContentBrowser
	m_Position = ImVec2(0.0f, m_Size.y);

	RenderHierarchy();
	RenderAddGameObjectMenu();
}
void HierarchyWindow::Init() {

	m_ImGuiViewport = m_Editor->GetGUIViewport();
	m_ContentBrowser = m_Editor->GetContentBrowser();
	m_DetailsWindow = m_Editor->GetDetailsWindow();
}

void HierarchyWindow::RenderHierarchy() {

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse;

	ImGui::SetNextWindowSize(m_Size);
	ImGui::SetNextWindowPos(m_Position);

	//ImGuiWindowFlags_NoBringToFrontOnFocus
	if (ImGui::Begin("Heirarchy", &m_Open, Flags)) {
		m_Editor->CheckForHoveredWindows();

		//TODO: This needs cleanup, especially the id check down there. Like test if the ID is reserved or something
		std::vector<GameObject*> GameObjects = m_ECS->GetGameObjects();
		for (uint32 i = 0; i < GameObjects.size(); i++) {
			if (!m_ECS->IsReserved(GameObjects.at(i)->GetObjectID()) && GameObjects.at(i)->GetParent() == nullptr)
				AddHeirarchyEntry(GameObjects.at(i));
		}
	}

	ImGui::End();
}
void HierarchyWindow::RenderAddGameObjectMenu() {

	ImGuiWindowFlags Flags = 0;
	Flags |= ImGuiWindowFlags_NoTitleBar;
	Flags |= ImGuiWindowFlags_NoResize;
	Flags |= ImGuiWindowFlags_NoMove;
	Flags |= ImGuiWindowFlags_NoScrollbar;
	Flags |= ImGuiWindowFlags_NoBackground;
	Flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	Flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

	//NOTE: This whole calculation thing here needs to be redone.

	ImGui::SetNextWindowSize(ImVec2(100.0f, 0.0f)); //As big as the button
	ImGui::SetNextWindowPos(ImVec2(m_Position.x + m_Size.x - 25.0f, -7.5f + m_Size.y)); //BUG: Even after all this hardcoding this is still slightly off. It is visible
	
	if (ImGui::Begin("##AddGameObjectMenu", &m_Open, Flags)) {
		m_Editor->CheckForHoveredWindows();

		if (ImGui::Button("##AddGameObjectMenuButton", ImVec2(70.0f, 19.0f))) {
			const ImVec2 PopupPosition = { ImGui::GetMousePos().x - 75.0f, ImGui::GetMousePos().y }; //75 - Half box width
			ImGui::SetNextWindowPos(PopupPosition);
			ImGui::OpenPopup("AddGameObjectMenu");
		}

		ImGui::SameLine(30.0f, 1.0f); //Manually adjusted
		ImGui::Text("Add..");

		if (ImGui::BeginPopup("AddGameObjectMenu")) {

			if (ImGui::Selectable("Empty GameObject", false, 0, ImVec2(150, 20))) {
				m_ECS->CreateGameObject();
			}
			else if (ImGui::Selectable("Cube", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECS->CreateGameObject("Cube");
				Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}
			else if (ImGui::Selectable("Sprite", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECS->CreateGameObject("Sprite"); //This breaks for some reason
				Object->AddComponent<SpriteRenderer>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}
			else if (ImGui::Selectable("Camera", false, 0, ImVec2(150, 20))) {
				GameObject* Object = &m_ECS->CreateGameObject("Camera"); //This breaks for some reason
				Object->AddComponent<Camera>(); //CHANGE THIS WHEN I HAVE ACTUAL 3D MESHES
			}


			ImGui::EndPopup();
		}

	}

	ImGui::End();
}

void HierarchyWindow::AddHeirarchyEntry(GameObject* entry) {

	if (entry == nullptr)
		return;
	//NOTE: SelectedGameObject is Target in the details window. 
	//Maybe i should move the whole thing to editor? Otherwise cant decide which thing should own the whole thing, Maybe this class?
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
			SetSelectedGameObject(*entry);
		}
		else if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
			ImGui::OpenPopup("EditMenu");
			SetSelectedGameObject(*entry);
		}

		//EditMenu
		bool Renaming = false; //Need this cause i think EndPopup() clears the rename popup id
		bool Deleting = false;
		if (ImGui::BeginPopup("EditMenu")) {

			ImGui::Text(std::string("Edit: " + entry->GetName()).data());

			if (ImGui::Selectable("Delete", false))
				Deleting = true;
			if (ImGui::Selectable("Rename", false))
				Renaming = true;

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
					ClearSelectedGameObject();

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
			strcpy_s(m_DetailsWindow->GetNameInputBuffer(), m_DetailsWindow->GetNameInputBufferSize(), entry->GetName().c_str());
			ImGui::OpenPopup("NameInput");
		}
		if (ImGui::BeginPopup("NameInput")) {
			if (ImGui::InputText("Name", m_DetailsWindow->GetNameInputBuffer(), m_DetailsWindow->GetNameInputBufferSize() - 1, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll)) {
				entry->SetName(m_DetailsWindow->GetNameInputBuffer());
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