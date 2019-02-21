/*
 * This is free and unencumbered software released into the public domain. 
 */

#include "app.h"

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#	define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#endif // BX_PLATFORM_
#include <cstring>
#include <bx/string.h>
#include <bx/math.h>
#include <bgfx/platform.h>
#include <GLFW/glfw3native.h>
#include <fstream>

// bgfx utils

const bgfx::Memory* app::loadMemory( const char* filename )
{
	std::ifstream file( filename, std::ios::binary | std::ios::ate );
	std::streamsize size = file.tellg();
	file.seekg( 0, std::ios::beg );
	const bgfx::Memory* mem = bgfx::alloc( uint32_t( size + 1 ) );
	if ( file.read( ( char* )mem->data, size ) )
	{
		mem->data[ mem->size - 1 ] = '\0';
		return mem;
	}
	return nullptr;
}

bgfx::ShaderHandle app::loadShader( const char* shader )
{
	return bgfx::createShader( loadMemory( shader ) );
}

bgfx::ProgramHandle app::loadProgram( const char* vsName, const char* fsName )
{
	bgfx::ShaderHandle vs = loadShader( vsName );
	bgfx::ShaderHandle fs = loadShader( fsName );
	return bgfx::createProgram( vs, fs, true );
}

// application

void app::Application::keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
#ifdef NO_IMGUI
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	ImGuiIO& io = ImGui::GetIO();
	if ( action == GLFW_PRESS )
	{
		io.KeysDown[ key ] = true;
	}
	if ( action == GLFW_RELEASE )
	{
		io.KeysDown[ key ] = false;
	}
	io.KeyCtrl = io.KeysDown[ GLFW_KEY_LEFT_CONTROL ] || io.KeysDown[ GLFW_KEY_RIGHT_CONTROL ];
	io.KeyShift = io.KeysDown[ GLFW_KEY_LEFT_SHIFT ] || io.KeysDown[ GLFW_KEY_RIGHT_SHIFT ];
	io.KeyAlt = io.KeysDown[ GLFW_KEY_LEFT_ALT ] || io.KeysDown[ GLFW_KEY_RIGHT_ALT ];
	io.KeySuper = io.KeysDown[ GLFW_KEY_LEFT_SUPER ] || io.KeysDown[ GLFW_KEY_RIGHT_SUPER ];
	if ( !io.WantCaptureKeyboard )
	{
		app->onKey( key, scancode, action, mods );
	}
#endif
}

void app::Application::charCallback( GLFWwindow* window, unsigned int codepoint )
{
#ifdef NO_IMGUI
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	ImGuiIO& io = ImGui::GetIO();
	if ( codepoint > 0 && codepoint < 0x10000 )
	{
		io.AddInputCharacter( ( unsigned short )codepoint );
	}
	app->onChar( codepoint );
#endif
}

void app::Application::charModsCallback( GLFWwindow* window, unsigned int codepoint, int mods )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	app->onCharMods( codepoint, mods );
}

void app::Application::mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
#ifdef NO_IMGUI
	ImGuiIO& io = ImGui::GetIO();
#endif
	if ( action == GLFW_PRESS && button >= 0 && button < 3 )
	{
		app->mMousePressed[button] = true;
	}
#ifdef NO_IMGUI
	if ( !io.WantCaptureMouse )
	{
		app->onMouseButton( button, action, mods );
	}
#endif
}

void app::Application::cursorPosCallback( GLFWwindow* window, double xpos, double ypos )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	app->onCursorPos( xpos, ypos );
}

void app::Application::cursorEnterCallback( GLFWwindow* window, int entered )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	app->onCursorEnter( entered );
}

void app::Application::scrollCallback( GLFWwindow* window, double xoffset, double yoffset )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	app->mMouseWheel += (float)yoffset;
	app->onScroll( xoffset, yoffset );
}

void app::Application::dropCallback( GLFWwindow* window, int count, const char** paths )
{
	app::Application* app = ( app::Application* )glfwGetWindowUserPointer( window );
	app->onDrop( count, paths );
}

void app::Application::imguiEvents( float dt )
{
#ifdef NO_IMGUI
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = dt;
	int w, h;
	int displayW, displayH;
	glfwGetWindowSize( mWindow, &w, &h );
	glfwGetFramebufferSize( mWindow, &displayW, &displayH );
	io.DisplaySize = ImVec2( ( float )w, ( float )h );
	io.DisplayFramebufferScale = ImVec2( w > 0 ? ( ( float )displayW / w ) : 0, h > 0 ? ( ( float )displayH / h ) : 0 );
	if ( glfwGetWindowAttrib( mWindow, GLFW_FOCUSED ) )
	{
		double mouse_x, mouse_y;
		glfwGetCursorPos( mWindow, &mouse_x, &mouse_y );
		io.MousePos = ImVec2( ( float )mouse_x, ( float )mouse_y );
	}
	else
	{
		io.MousePos = ImVec2( -1, -1 );
	}
	for (int i = 0; i < 3; i++)
	{
		io.MouseDown[ i ] = mMousePressed[ i ] || glfwGetMouseButton( mWindow, i ) != 0;
		mMousePressed[ i ] = false;
	}
	io.MouseWheel = mMouseWheel;
	mMouseWheel = 0.0f;
	glfwSetInputMode( mWindow, GLFW_CURSOR, io.MouseDrawCursor ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL );
	io.ClipboardUserData = mWindow;
#ifdef _WIN32
	io.ImeWindowHandle = glfwGetWin32Window( mWindow );
#endif
#endif
}

app::Application::Application()
{
	mWidth = 1280;
	mHeight = 768;
    mReset = 0;
	mMousePressed[ 0 ] = false;
	mMousePressed[ 1 ] = false;
	mMousePressed[ 2 ] = false;
	mMouseWheel = 0.0f;
    //mBgfxInTF = nullptr;
    mTFinBgfx = nullptr;
}

void app::Application::update( float dt )
{
    if( mTFinBgfx )
    {
        mTFinBgfx->update( dt );
    }
    //if( mBgfxInTF )
    //{
    //    mBgfxInTF->update( dt );
    //}
}

int app::Application::shutdown()
{
    if( mTFinBgfx )
    {
        delete mTFinBgfx;
    }
    return 0;
}

int app::Application::run( int argc, char** argv, bgfx::RendererType::Enum type, uint16_t vendorId, uint16_t deviceId, bgfx::CallbackI* callback, bx::AllocatorI* allocator )
{
	// Initialize the glfw
	if ( !glfwInit() )
	{
		return -1;
	}

    bool bgfxInTF = false;
    if ( argc > 1 )
    {
        if (0 == bx::strCmp(argv[1], "--bgfxintf") )
        {
            bgfxInTF = true;
        }
    }

	// Create a window
	glfwWindowHint( GLFW_CLIENT_API, GLFW_NO_API );
	mWindow = glfwCreateWindow( getWidth(), getHeight(), "", NULL, NULL );
	if ( !mWindow )
	{
		glfwTerminate();
		return -1;
	}

	// Setup input callbacks
	glfwSetWindowUserPointer( mWindow, this );
	glfwSetKeyCallback( mWindow, keyCallback );
	glfwSetMouseButtonCallback( mWindow, mouseButtonCallback );
	glfwSetScrollCallback( mWindow, scrollCallback );
	glfwSetCharCallback( mWindow, charCallback );

	glfwSetKeyCallback( mWindow, keyCallback );
	glfwSetCharCallback( mWindow, charCallback );
	glfwSetCharModsCallback( mWindow, charModsCallback );
	glfwSetMouseButtonCallback( mWindow, mouseButtonCallback );
	glfwSetCursorPosCallback( mWindow, cursorPosCallback );
	glfwSetCursorEnterCallback( mWindow, cursorEnterCallback );
	glfwSetScrollCallback( mWindow, scrollCallback );
	glfwSetDropCallback( mWindow, dropCallback );

	// Setup bgfx
	bgfx::PlatformData platformData;
	memset( &platformData, 0, sizeof( platformData ) );
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(mWindow);
	platformData.ndt = glfwGetX11Display();
#elif BX_PLATFORM_OSX
	platformData.nwh = glfwGetCocoaWindow(mWindow);
#elif BX_PLATFORM_WINDOWS
	platformData.nwh = glfwGetWin32Window(mWindow);
#endif // BX_PLATFORM_
	bgfx::setPlatformData( platformData );

	// Init bgfx
	bgfx::Init init;
	init.type = type;
	init.vendorId = vendorId;
	init.deviceId = deviceId;
	init.callback = callback;
	init.allocator = allocator;
	bgfx::init(init);

#ifdef NO_IMGUI
	// Setup ImGui
	imguiInit();
#endif

	// Initialize the application
	reset();
    if( !bgfxInTF )
    {
        mTFinBgfx = new TFinBgfx( argc, argv, this );
    }
    

	// Loop until the user closes the window
	float lastTime = 0;
	float dt;
	float time;
	while ( !glfwWindowShouldClose( mWindow ) )
	{
		time = ( float )glfwGetTime();
		dt = time - lastTime;
		lastTime = time;

		glfwPollEvents();
#ifdef NO_IMGUI
		imguiEvents( dt );
		ImGui::NewFrame();
		update( dt );
        ImGui::Render();
#else
		update( dt );
#endif
		bgfx::frame();

		int w, h;
		glfwGetWindowSize( mWindow, &w, &h );
		if ( w != mWidth || h != mHeight )
		{
			mWidth = w;
			mHeight = h;
		}
	}

	// Shutdown application and glfw
	int ret = shutdown();
#ifdef NO_IMGUI
	imguiShutdown();
#endif
	glfwTerminate();
	return ret;
}

void app::Application::reset()
{
#ifdef NO_IMGUI
	imguiReset( uint16_t( getWidth() ), uint16_t( getHeight() ) );
#endif
    if( mTFinBgfx )
        mTFinBgfx->reset();
}
