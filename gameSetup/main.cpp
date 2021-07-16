#include <Windows.h>
#include "gameStructs.h"
#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,
	DWORD     fdwReason,
	LPVOID    lpvReserved
)
{

	if(fdwReason == DLL_PROCESS_ATTACH)
	{
	//	OutputDebugString("gameLayer: dll attached");
	}

	return true;
}

/*
	auto triangle = [windowBuffer, line](Vec2i t0, Vec2i t1, Vec2i t2, glm::ivec3 color)
	{
		if (t0.y > t1.y) std::swap(t0, t1);
		if (t0.y > t2.y) std::swap(t0, t2);
		if (t1.y > t2.y) std::swap(t1, t2);

		int totalHeight = t2.y - t0.y;

		for(int y=t0.y; y <= t1.y; y++)
		{
			int segmentHeight = t1.y - t0.y + 1;
			float alpha = (float)(y - t0.y) / totalHeight;
			float beta = (float)(y - t0.y) / segmentHeight;

			Vec2i a = t0 + (t2 - t0) * alpha;
			Vec2i b = t0 + (t1 - t0) * beta;

			if (b.x < a.x) { std::swap(a.x, b.x); }
			for (int x = a.x; x <= b.x; x++)
			{
				windowBuffer->drawAt(x, y, 255, 255, 255);
			}

			windowBuffer->drawAt(a.x, y, red.r, red.g, red.b);
			windowBuffer->drawAt(b.x, y, green.r, green.g, green.b);
		}

		for(int y=t1.y+1; y<=t2.y; y++)
		{
			int segmentHeight = t2.y - t1.y + 1;
			float alpha = (float)(y - t0.y) / totalHeight;
			float beta = (float)(y - t1.y) / segmentHeight;

			Vec2i a = t0 + (t2 - t0) * alpha;
			Vec2i b = t1 + (t2 - t1) * beta;

			if (b.x < a.x) { std::swap(a.x, b.x); }
			for (int x = a.x; x <= b.x; x++)
			{
				windowBuffer->drawAt(x, y, 255, 255, 255);
			}

			windowBuffer->drawAt(a.x, y, red.r, red.g, red.b);
			windowBuffer->drawAt(b.x, y, green.r, green.g, green.b);
		}

		//line(t0, t1, green);
		//line(t1, t2, green);
		//line(t2, t0, red);

	};
*/




FreeListAllocator* allocator = nullptr;
Console* console = nullptr;

#pragma region allocator

void* operator new  (std::size_t count)
{
	auto a = allocator->threadSafeAllocate(count);


	return a;
}

void* operator new[](std::size_t count)
{
	auto a = allocator->threadSafeAllocate(count);


	return a;
}

void operator delete  (void* ptr)
{

	allocator->threadSafeFree(ptr);
}

void operator delete[](void* ptr)
{

	allocator->threadSafeFree(ptr);
}

#pragma endregion

extern "C" __declspec(dllexport) void onCreate(GameMemory* mem, HeapMemory * heapMemory,
	WindowSettings *windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	new(mem) GameMemory; // *mem = GameMemory();
	console = &platformFunctions->console;

	platformFunctions->makeContext();
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
	}

	gl2d::setErrorFuncCallback([](const char* c) {console->elog(c); });
	gl2d::init();
#pragma endregion


	//set the size of the window
	windowSettings->w = 200;
	windowSettings->h = 200;
	windowSettings->drawWithOpenGl = false;
	windowSettings->lockTo60fps = true;


	mem->model.LoadFile("resources/obj/african_head.obj");

	mem->renderer2D.create();

	stbi_set_flip_vertically_on_load(1);
	mem->renderer.texture.data = stbi_load("resources/african_head_diffuse.tga",
		&mem->renderer.texture.w, &mem->renderer.texture.h, nullptr, 3);

}

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

inline void barycentric2(Vec2f p, Vec2f p0, Vec2f p1, Vec2f p2, float &u, float &v)
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


glm::ivec3 red{255, 0, 0};
glm::ivec3 white{255, 255, 255};
glm::ivec3 green{0, 255, 0};

//this might be usefull to change variables on runtime
extern "C" __declspec(dllexport) void onReload(GameMemory * mem, HeapMemory * heapMemory,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	console = &platformFunctions->console;

	platformFunctions->makeContext();
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
	{
		MessageBoxA(0, "glewInit", "Error from glew", MB_ICONERROR);
	}

	gl2d::setErrorFuncCallback([](const char* c) {console->elog(c); });
	gl2d::init();
#pragma endregion

	platformFunctions->console.log("reloaded...");


}


extern "C" __declspec(dllexport) void gameLogic(GameInput * input, GameMemory * mem,
	HeapMemory * heapMemory, VolatileMemory * volatileMemory, GameWindowBuffer * windowBuffer,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{

#pragma region per frame setup
	allocator = &heapMemory->allocator;
	float deltaTime = input->deltaTime;
	console = &platformFunctions->console;

	if(windowSettings->fullScreen)
	{
	}else
	{
	}
	//mem->renderer.currentCamera.zoom = 0.5;
	//mem->renderer.currentCamera.target = { mem->posX, mem->posY };

	float w = windowBuffer->w;
	float h = windowBuffer->h;

	w = std::max(1.f, w);
	h = std::max(1.f, h);
	//todo normalize dir;

	windowBuffer->clear();
#pragma endregion
	
	mem->renderer.updateWindowMetrics(w, h);
	mem->renderer.updateWindowBuffer(windowBuffer);
	mem->renderer.clearDepth();

	mem->zBuffer.resize(w * h);
	for (auto &i : mem->zBuffer)
	{
		i = INFINITY;
	}

#pragma region camera

	float speed = 4 * deltaTime;

	glm::vec3 dir = {};
	if (input->keyBoard[Button::W].held || input->anyController.Up.held)
	{
		dir.z -= speed;
	}
	if (input->keyBoard[Button::S].held || input->anyController.Down.held)
	{
		dir.z += speed;
	}
	if (input->keyBoard[Button::A].held || input->anyController.Left.held)
	{
		dir.x -= speed;
	}
	if (input->keyBoard[Button::D].held || input->anyController.Right.held)
	{
		dir.x += speed;
	}

	if (input->keyBoard[Button::Q].held)
	{
		dir.y -= speed;
	}
	if (input->keyBoard[Button::E].held)
	{
		dir.y += speed;
	}

	mem->camera.moveFPS(dir);

	{
		static glm::dvec2 lastMousePos = {};
		if (input->rightMouse.held)
		{
			glm::dvec2 currentMousePos = {};
			currentMousePos.x = input->mouseX;
			currentMousePos.y = input->mouseY;

			float speed = 0.8f;

			glm::vec2 delta = lastMousePos - currentMousePos;
			delta *= speed * deltaTime;

			mem->camera.rotateCamera(delta);

			lastMousePos = currentMousePos;
		}
		else
		{
			lastMousePos.x = input->mouseX;
			lastMousePos.y = input->mouseY;

		}
	}

	mem->camera.aspectRatio = (float)w / h;

#pragma endregion


	auto depthCalculation = [](float z)->float
	{
		float closePlane = -10; float farPlane = -40;
		float val = ((1.f / z - 1.f / closePlane) / (1.f / farPlane - 1.f / closePlane));
		//float val = (z - closePlane) / (farPlane - closePlane);
		
		if(val < 0 || val > 1)
		{
			return -1.f;
		}
		
		val = glm::clamp(val, 0.f, 1.f);
		return val;
	};

	auto line = [windowBuffer](Vec2i p0, Vec2i p1, glm::ivec3 color) 
	{
		int x0 = p0.x;
		int y0 = p0.y;
		int x1 = p1.x;
		int y1 = p1.y;

		bool transpose = false;
		if (std::abs(x1 - x0) < std::abs(y1 - y0))
		{
			std::swap(x0, y0);
			std::swap(x1, y1);
			transpose = true;
		}

		if (x0 > x1)
		{
			std::swap(x0, x1);
			std::swap(y0, y1);
		}

		int dx = x1 - x0;
		int dy = y1 - y0;
		float derror = std::abs(dy / (float)dx);
		float error = 0;
		int y = y0;

		if(transpose)
		{
			for (int x = x0; x < x1; x++)
			{
				windowBuffer->drawAt(y, x, color.r, color.g, color.b);
				error += derror;
				if (error > .5f)
				{
					y += (y1 > y0 ? 1 : -1);
					error -= 1.f;
				}
			}
		}else
		{
			for (int x = x0; x < x1; x++)
			{
				windowBuffer->drawAt(x, y, color.r, color.g, color.b);
				error += derror;
				if (error > .5f)
				{
					y += (y1 > y0 ? 1 : -1);
					error -= 1.f;
				}
			}
		}

	};

	auto barycentric = [](Vec2f p, Vec2f p0, Vec2f p1, Vec2f p2, float &u, float &v)
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
		}else
		{
			u = (c * e - b * f) / div;
			v = (f * a - d * c) / div;
		}

		u = glm::clamp(u, 0.f, 1.f);
		v = glm::clamp(v, 0.f, 1.f);

		if(u+v > 1)
		{
			float sub = (u + v) / 2.f;
			u -= sub;
			v -= sub;
		}

		u = glm::clamp(u, 0.f, 1.f);
		v = glm::clamp(v, 0.f, 1.f);

	};

	auto triangle2 = [windowBuffer, line, mem, depthCalculation, w, h]
	(Vec3f T0, Vec3f T1, Vec3f T2, glm::ivec3 color, Vec2f textureUV0, Vec2f textureUV1, Vec2f textureUV2)
	{
		glm::ivec2 clipMin = { min(T0.x, T1.x, T2.x), min(T0.y, T1.y, T2.y) };
		glm::ivec2 clipMax = { max(T0.x, T1.x, T2.x), max(T0.y, T1.y, T2.y) };
		
		clipMin = glm::clamp(clipMin, { 0,0 }, { w - 1 , h - 1 });
		clipMax = glm::clamp(clipMax, { 0,0 }, { w - 1 , h - 1 });

		float z0 = T0.z;
		float z1 = T1.z;
		float z2 = T2.z;
		Vec2f t0 = { T0.x, T0.y };
		Vec2f t1 = { T1.x, T1.y };
		Vec2f t2 = { T2.x, T2.y };

		for (int y = clipMin.y; y <= clipMax.y; y++)
		{
			for (int x = clipMin.x; x <= clipMax.x; x++)
			{
				float u = 0;
				float v = 0;
				barycentric2(Vec2f(x, y), t0, t1, t2, u, v);
				
				if (u < 0 || v < 0 || u + v > 1)
				{
					continue;
				}

				Vec2f uvs[3] = { textureUV0, textureUV1, textureUV2 };
				float tu = (1 - u - v) * uvs[0].x + u * uvs[1].x + v * uvs[2].x;
				float tv = (1 - u - v) * uvs[0].y + u * uvs[1].y + v * uvs[2].y;

				tu *= mem->renderer.texture.w;
				tv *= mem->renderer.texture.h;
				int itu = floor(tu);
				int itv = floor(tv);
				unsigned char r = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 0];
				unsigned char g = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 1];
				unsigned char b = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 2];

				float z = (1 - u - v) * z0 + u * z1 + v * z2;
				float depth = depthCalculation(z);
				if (depth == -1) { continue; }

				float light = color.r / 255.f;

				if (depth < mem->zBuffer[x + y * w])
				{
					mem->zBuffer[x + y * w] = depth;
					windowBuffer->drawAt(x, y, r * light, g * light, b * light);
					//windowBuffer->drawAt(x, y, color.r, color.g, color.b);
				};

			}
		}
	
	};

	auto triangle = [windowBuffer, line, mem, depthCalculation, w, h, barycentric]
	(Vec3f T0, Vec3f T1, Vec3f T2, glm::ivec3 color, Vec2f textureUV0, Vec2f textureUV1, Vec2f textureUV2)
	{
		float z0 = T0.z;
		float z1 = T1.z;
		float z2 = T2.z;
		Vec2f t0 = { T0.x, T0.y };
		Vec2f t1 = { T1.x, T1.y };
		Vec2f t2 = { T2.x, T2.y };

		if (t0.y > t1.y) { std::swap(t0, t1); std::swap(textureUV0, textureUV1);}
		if (t0.y > t2.y) { std::swap(t0, t2); std::swap(textureUV0, textureUV2);}
		if (t1.y > t2.y) { std::swap(t1, t2); std::swap(textureUV1, textureUV2);}
		
		int totalHeight = t2.y - t0.y;
		int segmentHeight = t1.y - t0.y + 1;

		if (totalHeight == 0) { return; }

		if (segmentHeight)
		for(int y= floor(t0.y); y <= t1.y; y++)
		{
			float alpha = (float)(y - t0.y) / totalHeight;
			float beta = (float)(y - t0.y) / segmentHeight;

			Vec2f a = t0 + (t2 - t0) * alpha;
			Vec2f b = t0 + (t1 - t0) * beta;


			if (b.x < a.x) { std::swap(a.x, b.x); }
			for (int x = floor(a.x); x <= b.x; x++)
			{
				if (x < 0 || y < 0 || x >= w || y >= h) { continue; }

				//float ty = (y - t0.y) / (t1.y - t0.y);
				//float tx = (x - a.x) / (b.x - a.x);
				float u = 0;
				float v = 0;
				barycentric(Vec2f(x, y), t0, t1, t2, u, v);

				Vec2f uvs[3] = { textureUV0, textureUV1, textureUV2 };
				float tu = (1 - u - v) * uvs[0].x + u * uvs[1].x + v * uvs[2].x;
				float tv = (1 - u - v) * uvs[0].y + u * uvs[1].y + v * uvs[2].y;
				tu *= mem->renderer.texture.w;
				tv *= mem->renderer.texture.h;
				int itu = floor(tu);
				int itv = floor(tv);
				unsigned char r = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 0];
				unsigned char g = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 1];
				unsigned char b = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 2];

				float z = (1 - u - v) * z0 + u * z1 + v * z2;
				float depth = depthCalculation(z);
				if (depth == -1) { continue; }

				float light = color.r / 255.f;

				if (depth < mem->zBuffer[x + y * w])
				{
					mem->zBuffer[x + y * w] = depth;
					windowBuffer->drawAt(x, y, r*light, g*light, b*light);
					//windowBuffer->drawAt(x, y, color.r, color.g, color.b);
				};

				//float c = glm::clamp(depth * 255.f, 0.f, 255.f);

				//float zPrime = t0.y * (1.f - t) + t1.y * t;
				//float z = 

				//windowBuffer->drawAt(x, y, c, c, c);

			}

			//windowBuffer->drawAt(a.x, y, red.r, red.g, red.b);
			//windowBuffer->drawAt(b.x, y, green.r, green.g, green.b);
		}

		segmentHeight = t2.y - t1.y + 1;
		
		if(segmentHeight)
		for(int y= floor(t1.y+1); y<=t2.y; y++)
		{
			float alpha = (float)(y - t0.y) / totalHeight;
			float beta = (float)(y - t1.y) / segmentHeight;

			Vec2f a = t0 + (t2 - t0) * alpha;
			Vec2f b = t1 + (t2 - t1) * beta;

			if (b.x < a.x) { std::swap(a.x, b.x); }
			for (int x = floor(a.x); x <= b.x; x++)
			{
				if (x < 0 || y < 0 || x >= w || y >= h) { continue; }

				float u = 0;
				float v = 0;

				barycentric(Vec2f(x, y), t0, t1, t2, u, v);


				Vec2f uvs[3] = { textureUV0, textureUV1, textureUV2 };
				float tu = (1 - u - v) * uvs[0].x + u * uvs[1].x + v * uvs[2].x;
				float tv = (1 - u - v) * uvs[0].y + u * uvs[1].y + v * uvs[2].y;
				tu *= mem->renderer.texture.w;
				tv *= mem->renderer.texture.h;
				int itu = floor(tu);
				int itv = floor(tv);
				unsigned char r = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 0];
				unsigned char g = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 1];
				unsigned char b = mem->renderer.texture.data[(itu + itv * mem->renderer.texture.w) * 3 + 2];

				float z = (1 - u - v) * z0 + u * z1 + v * z2;
				float depth = depthCalculation(z);

				float light = color.r / 255.f;

				if (depth == -1) { continue; }
				if (depth < mem->zBuffer[x + y * w])
				{
					mem->zBuffer[x + y * w] = depth;
					windowBuffer->drawAt(x, y, r * light, g * light, b * light);
					//windowBuffer->drawAt(x, y, color.r, color.g, color.b);
				};

				//float c = glm::clamp(depth * 255.f, 0.f, 255.f);

				//windowBuffer->drawAt(x, y, c, c, c);
			}

			//windowBuffer->drawAt(a.x, y, red.r, red.g, red.b);
			//windowBuffer->drawAt(b.x, y, green.r, green.g, green.b);
		}
		
		//line(t0, t1, green);
		//line(t1, t2, green);
		//line(t2, t0, red);

	};


	//Vec2i t0[3] = { Vec2i(10, 10),   Vec2i(50, 10),  Vec2i(10, 50) };
	//Vec2i t1[3] = { Vec2i(50, 50),  Vec2i(50, 10),   Vec2i(10, 50) };
	//Vec2i t2[3] = { Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180) };
	//triangle(t0[0], t0[1], t0[2], glm::ivec3(255, 0, 0));
	//triangle(t1[0], t1[1], t1[2], glm::ivec3(255, 0, 0));
	//triangle(t2[0], t2[1], t2[2], glm::ivec3(0, 255, 0));

	//for (int i = 0; i < mem->model.nfaces(); i++)
	//{
	//	std::vector<int> face = mem->model.face(i);
	//	for (int j = 0; j < 3; j++)
	//	{
	//		Vec3f v0 = mem->model.vert(face[j]);
	//		Vec3f v1 = mem->model.vert(face[(j + 1) % 3]);
	//		int x0 = (v0.x + 1.) * w / 2.;
	//		int y0 = (v0.y + 1.) * h / 2.;
	//		int x1 = (v1.x + 1.) * w / 2.;
	//		int y1 = (v1.y + 1.) * h / 2.;
	//		line(x0, y0, x1, y1, glm::ivec3{ 255,255,255 });
	//	}
	//}
	srand(0);

	glm::vec3 light_dir(0, 0, -1);

	//mem->renderer.renderTriangleInClipSpace(
	//	glm::vec3(-0.5, 0.5, 1),
	//	glm::vec3(-0.2, -0.2, 1),
	//	glm::vec3(0.5, 0.6, 1), 
	//	glm::vec3(1, 1, 1));

	//glm::mat4 projMat = glm::perspective(glm::radians(90.f), (float)w / h, 0.1f, 30.f);
	glm::mat4 projMat = mem->camera.getProjectionMatrix() * mem->camera.getWorldToViewMatrix();


	//mem->renderer.renderLineClipSpace(glm::vec2(-0.5, -0.5), glm::vec2(0.7,0.7), glm::vec3(1.f));

	//mem->renderer.clipAndRenderTriangleInClipSpace({0,1,-0.5,1}, { -1,-1,-0.5,1 }, { 1,-1,-0.5,1 },
	//	{ 0,0 }, { 1,0 }, { 0,1 },
	//		glm::vec3(1.f));


	for (int i = 0; i < mem->model.LoadedIndices.size()/3; i++)
	{
		std::vector<int> face = { (int)mem->model.LoadedIndices[i * 3], (int)mem->model.LoadedIndices[i * 3 + 1], (int)mem->model.LoadedIndices[i * 3] + 2 };
		Vec3f screen_coords[3];
		glm::vec3 world_coords[3];
		glm::vec4 projected_coords[3];
		glm::vec2 textureUVs[3];

		for (int j = 0; j < 3; j++)
		{
			auto v = mem->model.LoadedVertices[face[j]];
			v.Position.Z -= 1.5;

			screen_coords[j] = Vec3f( (v.Position.X + 1.f) * w/ 2.f, (v.Position.Y + 1.f) * h / 2.f, v.Position.Z);
			screen_coords[j].x = floor(screen_coords[j].x);
			screen_coords[j].y = floor(screen_coords[j].y);
			world_coords[j] = { v.Position.X, v.Position.Y, v.Position.Z };

			textureUVs[j].x = v.TextureCoordinate.X;
			textureUVs[j].y = v.TextureCoordinate.Y;

			projected_coords[j] = glm::vec4(world_coords[j], 1);

			projected_coords[j] = projMat * projected_coords[j];

		}

	
		//if (intensity > 0)
		//{
		//	triangle2(screen_coords[0], screen_coords[1], screen_coords[2],
		//		glm::vec3(intensity * 255, intensity * 255, intensity * 255),
		//		textureUVs[0], textureUVs[1], textureUVs[2] );
		//
		//}else
		//{
		//	//triangle(screen_coords[0], screen_coords[1], screen_coords[2], glm::vec3(0));
		//}

		glm::vec3 n = glm::cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
		//Vec3f n = (world_coords[1] - world_coords[0]) ^ (world_coords[2] - world_coords[0]);
		n = glm::normalize(n);
		float intensity = glm::clamp(glm::dot(n, light_dir), 0.f, 1.f);
		//if(intensity > 0)
		{

			mem->renderer.clipAndRenderTriangleInClipSpace(projected_coords[0], projected_coords[1], projected_coords[2],
				textureUVs[0], textureUVs[1], textureUVs[2],
				glm::vec3(intensity));

			//mem->renderer.renderLineClipSpace(projected_coords[0], projected_coords[1], glm::vec3(1.f));
			//mem->renderer.renderLineClipSpace(projected_coords[1], projected_coords[2], glm::vec3(1.f));
			//mem->renderer.renderLineClipSpace(projected_coords[2], projected_coords[0], glm::vec3(1.f));

		}


		//for(int i=0;i<3;i++)
		//line(Vec2i(screen_coords[i].x, screen_coords[i].y), Vec2i(screen_coords[(i+1)%3].x, screen_coords[(i + 1) % 3].y), {255,255,255});
	}



}

extern "C" __declspec(dllexport) void onClose(GameMemory * mem, HeapMemory * heapMemory,
	WindowSettings * windowSettings, PlatformFunctions * platformFunctions)
{
#pragma region necesary setup
	allocator = &heapMemory->allocator;
	console = &platformFunctions->console;


#pragma endregion




}