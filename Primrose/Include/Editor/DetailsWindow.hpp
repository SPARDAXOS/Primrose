#pragma once


class Core;
class Editor;

class DetailsWindow final {
public:
	DetailsWindow() = delete;
	DetailsWindow(Core& core, Editor& editor) noexcept;
	~DetailsWindow() = default;

	DetailsWindow(const DetailsWindow&) = delete;
	DetailsWindow& operator=(const DetailsWindow&) = delete;

	DetailsWindow(DetailsWindow&&) = delete;
	DetailsWindow& operator=(DetailsWindow&&) = delete;

public:
	void Update();
	void Render();

private:
	Core* m_Core{ nullptr };
	Editor* m_Editor{ nullptr };
};