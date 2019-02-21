#pragma once

#include <bgfx/bgfx.h>

class TFinBgfx
{
public:
	TFinBgfx( int _argc, char** _argv );
	void reset( uint32_t width, uint32_t height, uint32_t reset );
	void update( float dt );
    int shutdown();

private:
	bgfx::ProgramHandle mProgram;
	bgfx::VertexBufferHandle mVbh;
	bgfx::IndexBufferHandle mIbh;
	float mTime;
    uint32_t mWidth;
    uint32_t mHeight;
};
