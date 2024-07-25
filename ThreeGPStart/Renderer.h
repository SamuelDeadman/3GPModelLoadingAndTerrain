#pragma once

#include "ExternalLibraryHeaders.h"

#include "Helper.h"
#include "Mesh.h"
#include "Camera.h"

//struct ModelMesh
//{
//	GLuint VAO{0};
//
//	GLuint numElements{0};
//
//	GLuint textureID{0};
//	//std::vector<Mesh> mesh;
//};
//
//struct Model
//{
//	std::string name;
//	std::vector<ModelMesh> mesh;
//};

class Renderer
{
private:
	// Program object - to host shaders
	GLuint m_program{ 0 };

	// Vertex Array Object to wrap all render settings
	GLuint m_VAO{ 0 };
	GLuint m_VAO_2{ 0 };
	GLuint m_VAO_3{ 0 };
	// Number of elments to use when rendering
	GLuint m_numElements{ 0 };
	GLuint m_numElements_2{ 0 };
	GLuint m_numElements_3{ 0 };
	GLuint m_textures;

	bool m_wireframe{ false };
	std::vector<glm::vec3> terrainvertices;
	std::vector<glm::vec3> terrainverticesoffset;
	bool CreateProgram();
public:
	Renderer();
	~Renderer();
	void DefineGUI();

	void CreateTerrain(int size);

	// Create and / or load geometry, this is like 'level load'
	bool InitialiseGeometry();
	bool LoadModel(const std::string& model, const std::string& tmodel, GLuint ElementNum, GLuint VAONum);
	// Render the scene
	void Render(const Helpers::Camera& camera, float deltaTime);
};

