#include "Renderer.h"
#include "Camera.h"
#include "Helper.h"
#include "ImageLoader.h"
#include <string>
Renderer::Renderer() 
{

}

// On exit must clean up any OpenGL resources e.g. the program, the buffers
Renderer::~Renderer()
{
	// TODO: clean up any memory used including OpenGL objects via glDelete* calls
	glDeleteProgram(m_program);
	glDeleteBuffers(1, &m_VAO);
	glDeleteBuffers(1, &m_VAO_2);
}


// Use IMGUI for a simple on screen GUI
void Renderer::DefineGUI()
{
	// Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	{
		ImGui::Begin("3GP");                    // Create a window called "3GP" and append into it.

		ImGui::Text("Visibility.");             // Display some text (you can use a format strings too)	

		ImGui::Checkbox("Wireframe", &m_wireframe);	// A checkbox linked to a member variable

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		
		ImGui::End();
	}
}

void Renderer::CreateTerrain(int size) 
{
	int numCellsX{ 64 };
	int numCellsZ{ 64 };

	int numVertsX = numCellsX + 1;
	int numVertsZ = numCellsZ + 1;

	int numTrisX = numCellsX * 4;
	int numTrisZ = numCellsZ * 4;

	float cellSizeX = size / (float)numCellsX;
	float cellSizeZ = size / (float)numCellsZ;

	int posOffset1{ 0 }; //change the position of the terrain (can't update live i think) X
	int posOffset2{ -2500 }; //Z - value to place the terrain behind the model
	int posOffset3{ -200 }; //Y
	
	Helpers::ImageLoader heightMap;
	heightMap.Load("Data\\Heightmaps\\curvy.gif"); //terrain heightmap 
	
	std::vector<glm::vec3> vertices;

	unsigned char* texels = (unsigned char*)heightMap.GetData();

	glm::vec3 startPos{ (-size / 2.0f) + posOffset1, 0 , (size / 2.0f) + posOffset2 };
	
	for (int z = 0; z < numVertsZ; z++)
	{
		for (int x = 0; x < numVertsX; x++)
		{
			glm::vec3 pos{ startPos };

			pos.x += cellSizeX * x;
			pos.z += cellSizeZ * -z;
			int aRadnomValue = (float)(rand() % 2000) + 1;
			int bRadnomValue = (float)(rand() % 2000) + 1;
			int cRadnomValue = (float)(rand() % 2000) + 1;

			float u = (float)x / (cellSizeX - 1);
			float v = (float)z / (cellSizeX - 1);

			int heightMapX = (int)(u * (heightMap.Width() - 1));
			int heightMapY = (int)(v * (heightMap.Height() - 1));

			//  //double Noise(int i, int x, int y) {
			//	int n = heightMapX + heightMapY * 57;
			//	n = (n << 13) ^ n;
			//	int a = aRadnomValue, b = aRadnomValue, c = cRadnomValue;
			//	int t = (n * (n * n * a + b) + c) & 0x7fffffff;
			//	int perlinval = 1.0 - (double)(t) / 1073741824.0;
			
			int offset = (heightMapX + heightMapY * heightMap.Width()) * 4;
			pos.y = texels[offset] + posOffset3; //terrain height map y position 
			//TERRAIN BEST SEEN IN WIREFRAME MODE
			terrainvertices.push_back(pos);
		}
	}

	std::vector<GLuint> elements;
	bool toggle{false};
	for (GLuint z = 0; z < numCellsZ; z++)
	{
		for (GLuint x = 0; x < numCellsX; x++)
		{
			GLuint startVertex = z * numVertsX + x;
			if (toggle)
			{
				elements.push_back(startVertex);
				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + 1 + numVertsX);

				elements.push_back(startVertex);
				elements.push_back(startVertex + 1 + numVertsX);
				elements.push_back(startVertex + numVertsX);
			}
			else
			{
				elements.push_back(startVertex);
				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + numVertsX);

				elements.push_back(startVertex + 1);
				elements.push_back(startVertex + 1 + numVertsX);
				elements.push_back(startVertex + numVertsX);
			}
			toggle = !toggle;
		}
		toggle = !toggle;
	}

	//std::vector<glm::vec3> normals(vertices.size());
	//std::fill(normals.begin(), normals.end(), glm::vec3(0, 0, 0));

	//for (size_t index =0; index < elements.size(); index += 3)//loops though all the triangles in the terrain
	//{
	//	double index1 = elements[index+0];//could loop this logic to reduce space
	//	double index2 = elements[index+1];
	//	double index3 = elements[index+2];


	//	glm::vec3 v0 = vertices[index1];
	//	glm::vec3 v1 = vertices[index2];
	//	glm::vec3 v2 = vertices[index3];

	//	glm::vec3 edge1 = v1 - v0;
	//	glm::vec3 edge2 = v2 - v0;

	//	glm::vec3 normal = glm::normalize(glm::cross(edge1, edge2));

	//	normals[index1] += normal;
	//	normals[index2] += normal;
	//	normals[index3] += normal;
	//}

	//for (glm::vec3& n : normals)
	//	n = glm::normalize(n);

	m_numElements_2 = elements.size();

	GLuint positionsVBO;
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainvertices.size(), terrainvertices.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0); 

	/*GLuint normalsVBO;
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/

	GLuint elementsVBO;
	glGenBuffers(1, &elementsVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenVertexArrays(1, &m_VAO_2); 
	glBindVertexArray(m_VAO_2);
	 
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,                  // attribute 0
		3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
		GL_FLOAT,           // type of the item
		GL_FALSE,           // normalized or not (advanced)
		0,                  // stride (advanced)
		(void*)0            // array buffer offset (advanced)
	);

	//glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(
	//	1,                  // attribute 0
	//	3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
	//	GL_FLOAT,           // type of the item
	//	GL_FALSE,           // normalized or not (advanced)
	//	0,                  // stride (advanced)
	//	(void*)0            // array buffer offset (advanced)
	//);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
	glBindVertexArray(0);
}

bool Renderer::LoadModel(const std::string& model, const std::string& tmodel, GLuint ElementNum, GLuint VAONum)
{
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile(model))//jeep object
		return false;

	Helpers::ImageLoader tloader;
	if (!tloader.Load(tmodel)) //jeep texture
		return false;

	glGenTextures(1, &m_textures);//this is defined you stupid software :(
	glBindTexture(GL_TEXTURE_2D, m_textures);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tloader.Width(), tloader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tloader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	int modeloffset1{ 2000 };
	int modeloffset2{ 2000 };
	glm::vec3 modeloffset{ modeloffset1, 0, modeloffset2 };
	terrainverticesoffset.push_back(modeloffset);

	std::vector<Helpers::Mesh> meshvector = loader.GetMeshVector();

	for (auto& mesh : meshvector)
	{
		GLuint verticesVBO;
		glGenBuffers(1, &verticesVBO);
		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * mesh.vertices.size()), mesh.vertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint normalsVBO;
		glGenBuffers(1, &normalsVBO);
		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * mesh.normals.size()), mesh.normals.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint textcoordVBO;
		glGenBuffers(1, &textcoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, textcoordVBO);
		glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec2) * mesh.uvCoords.size()), mesh.uvCoords.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLuint elementsVBO;
		glGenBuffers(1, &elementsVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		GLuint modeloffVBO;
		glGenBuffers(1, &modeloffVBO);
		glBindBuffer(GL_ARRAY_BUFFER, modeloffVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainverticesoffset.size(), terrainverticesoffset.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		ElementNum = mesh.elements.size();

		glGenVertexArrays(1, &VAONum);
		glBindVertexArray(VAONum);

		glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(
			0,                  // attribute 0
			3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(
			1,                  // attribute 0
			3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, textcoordVBO);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(
			2,                  // attribute 0
			2,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);

		glBindBuffer(GL_ARRAY_BUFFER, modeloffVBO);
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(
			3,                  // attribute 0
			3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
			GL_FLOAT,           // type of the item
			GL_FALSE,           // normalized or not (advanced)
			0,                  // stride (advanced)
			(void*)0            // array buffer offset (advanced)
		);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
		glBindVertexArray(0);
	}
	// Good idea to check for an error now:	
	Helpers::CheckForGLError();
	return true;
}

// Load, compile and link the shaders and create a program object to host them
bool Renderer::CreateProgram()
{
	// Create a new program (returns a unqiue id)
	m_program = glCreateProgram();

	// Load and create vertex and fragment shaders
	GLuint vertex_shader{ Helpers::LoadAndCompileShader(GL_VERTEX_SHADER, "Data/Shaders/vertex_shader.glsl") };
	GLuint fragment_shader{ Helpers::LoadAndCompileShader(GL_FRAGMENT_SHADER, "Data/Shaders/fragment_shader.glsl") };
	if (vertex_shader == 0 || fragment_shader == 0)
		return false;

	// Attach the vertex shader to this program (copies it)
	glAttachShader(m_program, vertex_shader);//lol DON'T accidently delete these :(

	// The attibute 0 maps to the input stream "vertex_position" in the vertex shader
	// Not needed if you use (location=0) in the vertex shader itself
	//glBindAttribLocation(m_program, 0, "vertex_position");

	// Attach the fragment shader (copies it)
	glAttachShader(m_program, fragment_shader); //lol DON'T accidently delete these :(
	// Done with the originals of these as we have made copies
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader); 

	// Link the shaders, checking for errors
	if (!Helpers::LinkProgramShaders(m_program))
		return false;

	return !Helpers::CheckForGLError();
}

// Load / create geometry into OpenGL buffers	
bool Renderer::InitialiseGeometry()
{
	CreateTerrain(4000);

	// Load and compile shaders into m_program
	if (!CreateProgram())
		return false;

	// Helpers has an object for loading 3D geometry, supports most types
	
	// Load in the jeep
	Helpers::ModelLoader loader;
	if (!loader.LoadFromFile("Data\\Models\\Jeep\\jeep.obj"))//jeep object
		return false;

	//Helpers::ModelLoader loadera;
	//if (!loadera.LoadFromFile("Data\\Models\\Apple\\apple.obj"))//jeep object
	//	return false;

	Helpers::ImageLoader tloader;
	if (!tloader.Load("Data\\Models\\Jeep\\jeep_army.jpg")) //jeep texture
		return false;

	glGenTextures(1, &m_textures);
	glBindTexture(GL_TEXTURE_2D, m_textures);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tloader.Width(), tloader.Height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tloader.GetData());
	glGenerateMipmap(GL_TEXTURE_2D);

	int modeloffset1{ 2000 };
	int modeloffset2{ 2000 };
	glm::vec3 modeloffset{ modeloffset1, 0, modeloffset2 };
	terrainverticesoffset.push_back(modeloffset);

		std::vector<Helpers::Mesh> meshvector = loader.GetMeshVector();

		for (auto& mesh : meshvector)
		{
			GLuint verticesVBO;
			glGenBuffers(1, &verticesVBO);
			glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
			glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * mesh.vertices.size()), mesh.vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLuint normalsVBO;
			glGenBuffers(1, &normalsVBO);
			glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
			glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec3) * mesh.normals.size()), mesh.normals.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLuint textcoordVBO;
			glGenBuffers(1, &textcoordVBO);
			glBindBuffer(GL_ARRAY_BUFFER, textcoordVBO);
			glBufferData(GL_ARRAY_BUFFER, (sizeof(glm::vec2) * mesh.uvCoords.size()), mesh.uvCoords.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			GLuint elementsVBO;
			glGenBuffers(1, &elementsVBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mesh.elements.size(), mesh.elements.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			GLuint modeloffVBO;
			glGenBuffers(1, &modeloffVBO);
			glBindBuffer(GL_ARRAY_BUFFER, modeloffVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * terrainverticesoffset.size(), terrainverticesoffset.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			m_numElements = mesh.elements.size();

			glGenVertexArrays(1, &m_VAO);
			glBindVertexArray(m_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(
				0,                  // attribute 0
				3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
				GL_FLOAT,           // type of the item
				GL_FALSE,           // normalized or not (advanced)
				0,                  // stride (advanced)
				(void*)0            // array buffer offset (advanced)
			);

			glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(
				1,                  // attribute 0
				3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
				GL_FLOAT,           // type of the item
				GL_FALSE,           // normalized or not (advanced)
				0,                  // stride (advanced)
				(void*)0            // array buffer offset (advanced)
			);

			glBindBuffer(GL_ARRAY_BUFFER, textcoordVBO);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(
				2,                  // attribute 0
				2,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
				GL_FLOAT,           // type of the item
				GL_FALSE,           // normalized or not (advanced)
				0,                  // stride (advanced)
				(void*)0            // array buffer offset (advanced)
			);

			glBindBuffer(GL_ARRAY_BUFFER, modeloffVBO);
			glEnableVertexAttribArray(3);
			glVertexAttribPointer(
				3,                  // attribute 0
				3,                  // size in components of each item in the stream e.g. a position has 3 components x,y and z
				GL_FLOAT,           // type of the item
				GL_FALSE,           // normalized or not (advanced)
				0,                  // stride (advanced)
				(void*)0            // array buffer offset (advanced)
			);


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementsVBO);
			glBindVertexArray(0);
		}

		


	// Good idea to check for an error now:	
	Helpers::CheckForGLError();

	return true;
}



// Render the scene. Passed the delta time since last called.
void Renderer::Render(const Helpers::Camera& camera, float deltaTime)
{		
	// Configure pipeline settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Wireframe mode controlled by ImGui
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Clear buffers from previous frame
	glClearColor(0.0f, 0.0f, 0.0f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// TODO: Compute viewport and projection matrix
	GLint viewportSize[4];
	glGetIntegerv(GL_VIEWPORT, viewportSize);
	const float aspect_ratio = viewportSize[2] / (float)viewportSize[3];
	glm::mat4 projection_xform = glm::perspective(glm::radians(45.0f), aspect_ratio, 1.0f, 4000.0f);
	// TODO: Compute camera view matrix and combine with projection matrix for passing to shader
	glm::mat4 view_xform = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetLookVector(), camera.GetUpVector());
	glm::mat4 combined_xform = projection_xform * view_xform;
	// TODO: Send the combined matrix to the shader in a uniform
	glUseProgram(m_program);

	GLuint combined_xform_id = glGetUniformLocation(m_program, "combined_xform");
	glUniformMatrix4fv(combined_xform_id, 1, GL_FALSE, glm::value_ptr(combined_xform));
	// TODO: render each mesh. Send the correct model matrix to the shader in a uniform
	glm::mat4 model_xform = glm::mat4(1);

	// Send the model matrix to the shader in a uniform
	GLuint model_xform_id = glGetUniformLocation(m_program, "model_xform");
	glUniformMatrix4fv(model_xform_id, 1, GL_FALSE, glm::value_ptr(model_xform));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_textures);
	glUniform1i(glGetUniformLocation(m_program, "sampler_tex"), 0);

	

	// Bind our VAO and render
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_numElements, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(m_VAO_2);
	glDrawElements(GL_TRIANGLES, m_numElements_2, GL_UNSIGNED_INT, (void*)0);

	glBindVertexArray(m_VAO_3);
	glDrawElements(GL_TRIANGLES, m_numElements_3, GL_UNSIGNED_INT, (void*)0);

	// Always a good idea, when debugging at least, to check for GL errors
	Helpers::CheckForGLError();
	// Always a good idea, when debugging at least, to check for GL errors each frame

	Helpers::CheckForGLError();
}

