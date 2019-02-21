#pragma once

#include "TinySTL/string.h"

#include <Renderer/IRenderer.h>
#include "Input/InputSystem.h"
#include "Input/InputMappings.h"
//Math
#include "OS/Math/MathTypes.h"

class TF
{
public:
	TF( int _argc, char** _argv );
	void reset( uint32_t width, uint32_t height, WindowsDesc *desc );
	void update( float dt );
    int shutdown();

	bool load();
	void unload();
	void draw();
	tinystl::string getName() { return "01_Transformations"; }
	bool addSwapChain();
	bool addDepthBuffer();
	void recenterCameraView(float maxDistance, vec3 lookAt = vec3(0));
	static bool cameraInputEvent(const ButtonData* data);

private:

	float mTime;
    uint32_t mWidth;
    uint32_t mHeight;
	WindowsDesc *mWindowsDesc;
};
