#pragma once

#include "ContentLoader.h"

class MeshLoader final : public ContentLoader<Mesh>
{
public:
	Mesh* Load(const std::initializer_list<std::wstring>& assetFiles) override;
	void Update(const std::initializer_list<std::wstring>& assetFiles, Mesh* content) override;
private:
	friend class SpringWind;
	MeshLoader(void) {};
	~MeshLoader(void) {};

	// -------------------------
	// Disabling default copy constructor and default 
	// assignment operator.
	// -------------------------
	MeshLoader(const MeshLoader &obj) = delete;
	MeshLoader& operator=(const MeshLoader& obj) = delete;
};

