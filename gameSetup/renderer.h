#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <ivec.h>

struct GameWindowBuffer;

struct Renderer
{
	float w = 0;  //todo remove
	float h = 0;
	GameWindowBuffer *windowBuffer;
	std::vector<float> zBuffer;

	Renderer() {};

	void updateWindowMetrics(int w, int h);
	void updateWindowBuffer(GameWindowBuffer* b) { windowBuffer = b; }
	void clearDepth();

	void renderTriangleInClipSpace(glm::vec3 T0, glm::vec3 T1,
		glm::vec3 T2, glm::vec3 color);

	glm::ivec2 toScreenCoords(glm::vec2 v);
	glm::vec2 toScreenCoordsFloat(glm::vec2 v);

};
