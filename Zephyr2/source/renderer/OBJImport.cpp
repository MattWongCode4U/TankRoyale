#include "OBJImport.h"

std::vector<GLfloat> OBJImport::importObjInfo(std::string string)
{
	std::stringstream ss(string);
	std::string lines, lines2, lines3;
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> textArray;
	std::vector<GLfloat> combined;

	while (std::getline(ss, lines, '\n'))
	{

		std::stringstream ss2(lines);
		while (std::getline(ss2, lines2, ' '))
		{
			//sSDL_Log("%s", lines2.c_str());
			if (strcmp(lines2.c_str(), "v") == 0)
			{
				
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					vertexArray.push_back(strtof(lines2.c_str(), NULL));
				}
				
			}
			if (strcmp(lines2.c_str(), "vt") == 0)
			{
				for (int i = 0; i < 2; i++)
				{
					std::getline(ss2, lines2, ' ');
					textArray.push_back(strtof(lines2.c_str(), NULL));
				}
			}
			if (strcmp(lines2.c_str(), "vn") == 0)
			{
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					normalArray.push_back(strtof(lines2.c_str(), NULL));
				}
			}
			if (strcmp(lines2.c_str(), "f") == 0)
			{
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					std::stringstream ss3(lines2);
					for (int j = 0; j < 3; j++) {
						std::getline(ss3, lines3, '/');
						switch (j) {
						case 0:
							for (int k = 0; k < 3; k++) {
								combined.emplace_back(vertexArray[(atoi(lines3.c_str()) - 1) * 3 + k]);
							}
							break;
						case 1:
							for (int k = 0; k < 2; k++) {
								combined.emplace_back(textArray[(atoi(lines3.c_str()) - 1) * 2 + k]);
							}
							break;
						case 2:
							for (int k = 0; k < 3; k++) {
								combined.emplace_back(normalArray[(atoi(lines3.c_str()) - 1) * 3 + k]);
							}
							break;

						}
					}
				}
			}
		}
	}
	vertexArray.clear();
	textArray.clear();
	normalArray.clear();
	return combined;
}
std::vector<GLfloat> OBJImport::importObjInfoVertices(std::string string)
{
	std::stringstream ss(string);
	std::string lines, lines2, lines3;
	std::vector<GLfloat> vertexArray;
	std::vector<GLfloat> normalArray;
	std::vector<GLfloat> textArray;
	std::vector<GLfloat> combined;

	while (std::getline(ss, lines, '\n'))
	{

		std::stringstream ss2(lines);
		while (std::getline(ss2, lines2, ' '))
		{
			//sSDL_Log("%s", lines2.c_str());
			if (strcmp(lines2.c_str(), "v") == 0)
			{
				
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					vertexArray.push_back(strtof(lines2.c_str(), NULL));
				}
				
			}
			if (strcmp(lines2.c_str(), "vt") == 0)
			{
				for (int i = 0; i < 2; i++)
				{
					std::getline(ss2, lines2, ' ');
					textArray.push_back(strtof(lines2.c_str(), NULL));
				}
			}
			if (strcmp(lines2.c_str(), "vn") == 0)
			{
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					normalArray.push_back(strtof(lines2.c_str(), NULL));
				}
			}
			if (strcmp(lines2.c_str(), "f") == 0)
			{
				for (int i = 0; i < 3; i++)
				{
					std::getline(ss2, lines2, ' ');
					std::stringstream ss3(lines2);
					for (int j = 0; j < 3; j++) {
						std::getline(ss3, lines3, '/');
						switch (j) {
						case 0:
							for (int k = 0; k < 3; k++) {
								combined.emplace_back(vertexArray[(atoi(lines3.c_str()) - 1) * 3 + k]);
							}
							break;
						case 1:
							for (int k = 0; k < 2; k++) {
								combined.emplace_back(textArray[(atoi(lines3.c_str()) - 1) * 2 + k]);
							}
							break;
						case 2:
							for (int k = 0; k < 3; k++) {
								combined.emplace_back(normalArray[(atoi(lines3.c_str()) - 1) * 3 + k]);
							}
							break;

						}
					}
				}
			}
		}
	}
	textArray.clear();
	normalArray.clear();
	combined.clear();
	return vertexArray;
}