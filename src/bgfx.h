#pragma once

#include "app.h"

class TFinBgfx
{
public:
	TFinBgfx( int _argc, char** _argv, app::Application *app );
	void reset();
	void update( float dt );
    int shutdown();

private:
	app::Application *mApp;
	bgfx::ProgramHandle mProgram;
	bgfx::VertexBufferHandle mVbh;
	bgfx::IndexBufferHandle mIbh;
	float mTime;
};
