#pragma once


class Core;


class ModelLoader final {
public:
	ModelLoader() = delete;
	ModelLoader(Core& core) noexcept;
	~ModelLoader() = default;

	ModelLoader(const ModelLoader&) = delete;
	ModelLoader& operator=(const ModelLoader&) = delete;

	ModelLoader(ModelLoader&&) = delete;
	ModelLoader& operator=(ModelLoader&&) = delete;


public:


private:
	Core* m_Core	{ nullptr };
};