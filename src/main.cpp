#include "app.h"

int main( int argc, char** argv )
{
	app::Application app;
	return app.run( argc, argv, bgfx::RendererType::OpenGL );
}
