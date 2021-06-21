#include "renderer.h"
#include "gameStructs.h"

template<class T>
T min(T a, T b, T c)
{
	return std::min(std::min(a, b), c);
}

template<class T>
T max(T a, T b, T c)
{
	return std::max(std::max(a, b), c);
}

template<class T>
T min(T a, T b, T c, T d)
{
	return std::min(min(a, b, c), d);
}

template<class T>
T max(T a, T b, T c, T d)
{
	return std::max(max(a, b, c), d);
}


void barycentric2(glm::vec2 p, glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, float& u, float& v)
{
	float x = p.x;
	float y = p.y;

	float a = p0.x - p1.x;
	float b = p0.x - p2.x;
	float c = p0.x - x;
	float d = p0.y - p1.y;
	float e = p0.y - p2.y;
	float f = p0.y - y;

	float div = (a * e - b * d);

	if (div == 0)
	{
		u = 0;
		v = 0;
	}
	else
	{
		u = (c * e - b * f) / div;
		v = (f * a - d * c) / div;
	}

};

void Renderer::updateWindowMetrics(int w, int h)
{
	if (this->w == w && this->h == h) { return; }

	this->w = w; this->h = h;
	zBuffer.clear();
	zBuffer.resize(w * h);


}

void Renderer::clearDepth()
{
	for (auto& i : zBuffer)
	{
		i = INFINITY;
	}
}

void Renderer::renderTriangleInClipSpace(glm::vec3 T0, glm::vec3 T1, glm::vec3 T2, glm::vec3 color)
{
	glm::fvec2 clipMinF = { min(T0.x, T1.x, T2.x), min(T0.y, T1.y, T2.y) };
	glm::fvec2 clipMaxF = { max(T0.x, T1.x, T2.x), max(T0.y, T1.y, T2.y) };

	glm::ivec2 clipMin = toScreenCoords(clipMinF);
	glm::ivec2 clipMax = toScreenCoords(clipMaxF);

	clipMin = glm::clamp(clipMin, { 0,0 }, { w - 1 , h - 1 });
	clipMax = glm::clamp(clipMax, { 0,0 }, { w - 1 , h - 1 });

	//this are in -1 1 space
	float z0 = T0.z;
	float z1 = T1.z;
	float z2 = T2.z;

	//this are in pixel screen space
	glm::vec2 t0 = toScreenCoordsFloat( glm::vec2{ T0.x, T0.y });
	glm::vec2 t1 = toScreenCoordsFloat( glm::vec2{ T1.x, T1.y });
	glm::vec2 t2 = toScreenCoordsFloat( glm::vec2{ T2.x, T2.y });

	//x, y are in pixel screen space
	for (int y = clipMin.y; y <= clipMax.y; y++)
	{
		for (int x = clipMin.x; x <= clipMax.x; x++)
		{
			float u = 0;
			float v = 0;
			barycentric2(glm::vec2(x, y), t0, t1, t2, u, v);

			if (u < 0 || v < 0 || u + v > 1)
			{
				continue;
			}

			//Vec2f uvs[3] = { textureUV0, textureUV1, textureUV2 };
			//float tu = (1 - u - v) * uvs[0].x + u * uvs[1].x + v * uvs[2].x;
			//float tv = (1 - u - v) * uvs[0].y + u * uvs[1].y + v * uvs[2].y;

			//tu *= mem->texture.w;
			//tv *= mem->texture.h;
			//int itu = floor(tu);
			//int itv = floor(tv);
			//unsigned char r = mem->texture.data[(itu + itv * mem->texture.w) * 3 + 0];
			//unsigned char g = mem->texture.data[(itu + itv * mem->texture.w) * 3 + 1];
			//unsigned char b = mem->texture.data[(itu + itv * mem->texture.w) * 3 + 2];

			unsigned char r = 255;
			unsigned char g = 255;
			unsigned char b = 255;

			float z = (1 - u - v) * z0 + u * z1 + v * z2; //trilinear interpolation
			//float depth = depthCalculation(z);
			float depth = z;
			//if (depth <= -1) { continue; }
			//todo clip behind camera

			//float light = color.r / 255.f;
			float light = 1;

			//if (depth < zBuffer[x + y * w]) ignore z buffer for now
			{
				//zBuffer[x + y * w] = depth;
				windowBuffer->drawAt(x, y, r * light, g * light, b * light);
				//windowBuffer->drawAt(x, y, color.r, color.g, color.b);
			};

		}
	}


}

glm::vec2 Renderer::toScreenCoordsFloat(glm::vec2 v)
{
	glm::vec2 sizes = { w, h };
	v += glm::vec2{ 1.f,1.f };
	v *= sizes;
	v /= 2.f;
	return v;
}


glm::ivec2 Renderer::toScreenCoords(glm::vec2 v)
{
	return toScreenCoordsFloat(v);
}

