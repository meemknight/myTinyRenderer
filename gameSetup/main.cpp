#include <Windows.h>
#include "gameStructs.h"
#include <GL/glew.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <fstream>

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
	windowSettings->w = 600;
	windowSettings->h = 600;
	windowSettings->drawWithOpenGl = false;
	windowSettings->lockTo60fps = false;


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

//this might be useful to change variables on runtime
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



	glm::vec3 light_dir(0, 0, -1);

	
	glm::mat4 projMat = mem->camera.getProjectionMatrix() * mem->camera.getWorldToViewMatrix();

	static int counter = 500;
	mem->profiler.start();

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

	mem->profiler.end();
	counter--;

	if (counter<=0)
	{
		auto rez = mem->profiler.getAverageAndResetData();

		std::ofstream of("rezults.txt", std::ios_base::app);

		of << rez.timeSeconds << " s,    " << rez.cpuClocks << " clocks  " << " no floor" << "\n\n";
		
		of.close();

		exit(0);
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