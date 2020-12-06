#include "Geometry.h"

Geometry::Geometry(std::string objFilename, glm::vec3 amb, glm::vec3 diff, glm::vec3 spec, glm::vec3 scale) : 
	kAmbient(amb), kDiffuse(diff), kSpecular(spec) {
	// parsing vertex, vertex normal and faces
	std::ifstream objFile(objFilename);

	if (objFile.is_open()) {
		std::string line;
		std::vector<glm::vec3> temp_normals;
		std::vector<int> vertexIdx;
		std::vector<int> normalIdx;
		bool special = false;
		while (std::getline(objFile, line)) {
			std::stringstream ss;
			ss << line;

			// get the label
			std::string label;
			ss >> label;

			// line is vertex
			if (label == "v") {
				// write position to a vec3 and add to points vector
				glm::vec3 vertex;
				ss >> vertex.x >> vertex.y >> vertex.z;
				points.push_back(vertex);
			}
			// line is vertex normal
			else if (label == "vn") {
				// write normal data to a vec3 and push to temp normal vector
				glm::vec3 normal;
				ss >> normal.x >> normal.y >> normal.z;
				temp_normals.push_back(normal);
			}
			// lien is face
			else if (label == "f") {
				glm::ivec3 face;
				std::string index1;
				std::string index2;
				std::string index3;
				ss >> index1 >> index2 >> index3;
				if (index1.substr(index1.find('/'), 2) == "//") {
					index1 = index1.substr(0, index1.find('/'));
					index2 = index2.substr(0, index2.find('/'));
					index3 = index3.substr(0, index3.find('/'));
					face.x = std::stoi(index1) - 1;
					face.y = std::stoi(index2) - 1;
					face.z = std::stoi(index3) - 1;
					faces.push_back(face);
				}
				else {
					special = true;
					face.x = std::stoi(index1.substr(0, index1.find('/'))) - 1;
					face.y = std::stoi(index2.substr(0, index2.find('/'))) - 1;
					face.z = std::stoi(index3.substr(0, index3.find('/'))) - 1;
					faces.push_back(face);
					vertexIdx.push_back(face.x);
					vertexIdx.push_back(face.y);
					vertexIdx.push_back(face.z);
					normalIdx.push_back(std::stoi(index1.substr(index1.find('/', index1.find('/') + 1) + 1, std::string::npos)) - 1);
					normalIdx.push_back(std::stoi(index2.substr(index2.find('/', index2.find('/') + 1) + 1, std::string::npos)) - 1);
					normalIdx.push_back(std::stoi(index3.substr(index3.find('/', index3.find('/') + 1) + 1, std::string::npos)) - 1);
				}
			}
		}
		objFile.close();
		if (special) {
			std::cerr << "special!" << std::endl;
			normals = std::vector<glm::vec3>(vertexIdx.size());
			for (int i = 0; i < vertexIdx.size(); ++i) {
				normals[vertexIdx[i]] = temp_normals[normalIdx[i]];
			}
		}
		else {
			normals = temp_normals;
		}
	}
	else {
		std::cerr << "Can't open the file: " << objFilename << std::endl;
	}

	// record max and min on 3 dimensions
	GLfloat xMax = points[0].x;
	GLfloat xMin = points[0].x;
	GLfloat yMax = points[0].y;
	GLfloat yMin = points[0].y;
	GLfloat zMax = points[0].z;
	GLfloat zMin = points[0].z;

	// iterate through all points, record max and min of 3 dimensions
	for (const auto& vertex : points) {
		if (vertex.x > xMax) {
			xMax = vertex.x;
		}
		if (vertex.x < xMin) {
			xMin = vertex.x;
		}
		if (vertex.y > yMax) {
			yMax = vertex.y;
		}
		if (vertex.y < yMin) {
			yMin = vertex.y;
		}
		if (vertex.z > zMax) {
			zMax = vertex.z;
		}
		if (vertex.z < zMin) {
			zMin = vertex.z;
		}
	}

	// calculate center coordinate
	auto center = glm::vec3((xMin + xMax) / 2, (yMin + yMax) / 2, (zMin + zMax) / 2);

	// record the maximum distance from the center
	GLfloat distMax = 0;
	for (const auto& vertex : points) {
		GLfloat dist = glm::length(vertex - center);
		if (dist > distMax) {
			distMax = dist;
		}
	}
	// calculate the scale factor, normalizing maximum distance to 9


	// Translate to center
	model = glm::translate(glm::mat4(1), -center);
	model = glm::scale(scale) * model;


	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);

	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Bind NBO to the bound VAO, and store normal data
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), normals.data(), GL_STATIC_DRAW);

	// Enable Vertex Attreibute 1 to pass normal data through to shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * faces.size(), faces.data(), GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	std::cerr << "Finish loading " << objFilename << std::endl;
}

Geometry::~Geometry() {
	// Delete the VBO and the VAO.
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);

      for (auto child : children) {
            delete child;
      }
}


void Geometry::draw(const glm::mat4& C, GLuint shader) {
	// Actiavte the shader program 
	glUseProgram(shader);

	// Get back correct culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "transform"), 1, GL_FALSE, glm::value_ptr(C));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "kAmbient"), 1, glm::value_ptr(kAmbient));
	glUniform3fv(glGetUniformLocation(shader, "kDiffuse"), 1, glm::value_ptr(kDiffuse));
	glUniform3fv(glGetUniformLocation(shader, "kSpecular"), 1, glm::value_ptr(kSpecular));

	// Bind the VAO
	glBindVertexArray(VAO);

	// Draw triangles
	glDrawElements(GL_TRIANGLES, 3 * faces.size(), GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);

	for (auto child : children) {
		child->draw(C, shader);
	}
}

void Geometry::update() {
	for (auto child : children) {
		child->update();
	}
}

void Geometry::addChild(Node* child) {
      children.push_back(child);
}
