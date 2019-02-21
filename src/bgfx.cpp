/*
 * Simple rotating cubes example, based off bgfx's example-01-cubes.
 * Does not mimic the original example completely because we're in a
 * different coordinate system.
 *
 * Copyright 2011-2017 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include "bgfx.h"
#include <bx/math.h>
#include <bx/string.h>

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
	static void init()
	{
		ms_decl
			.begin()
			.add( bgfx::Attrib::Position, 3, bgfx::AttribType::Float )
			.add( bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true )
			.end();
	}
	static bgfx::VertexDecl ms_decl;
};
bgfx::VertexDecl PosColorVertex::ms_decl;

static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};
static const uint16_t s_cubeTriList[] = { 2, 1, 0, 2, 3, 1, 5, 6, 4, 7, 6, 5, 4, 2, 0, 6, 2, 4, 3, 5, 1, 3, 7, 5, 1, 4, 0, 1, 5, 4, 6, 3, 2, 7, 3, 6 };

class TFinBgfx
{
	TFinBgfx( int _argc, char** _argv, app::Application *app ) : mApp( app )
	{
		PosColorVertex::init();

		char vsName[32];
		char fsName[32];

		const char* shaderPath = "???";

		switch ( bgfx::getRendererType() )
		{
			case bgfx::RendererType::Noop:
			case bgfx::RendererType::Direct3D9:  shaderPath = "shaders/dx9/";   break;
			case bgfx::RendererType::Direct3D11:
			case bgfx::RendererType::Direct3D12: shaderPath = "shaders/dx11/";  break;
			case bgfx::RendererType::Gnm:                                       break;
			case bgfx::RendererType::Metal:      shaderPath = "shaders/metal/"; break;
			case bgfx::RendererType::OpenGL:     shaderPath = "shaders/glsl/";  break;
			case bgfx::RendererType::OpenGLES:   shaderPath = "shaders/essl/";  break;
			case bgfx::RendererType::Vulkan:                                    break;
			case bgfx::RendererType::Count:                                     break;
		}

		bx::strCopy( vsName, BX_COUNTOF(vsName), shaderPath );
		bx::strCat( vsName, BX_COUNTOF(vsName), "vs_cubes.bin" );

		bx::strCopy( fsName, BX_COUNTOF(fsName), shaderPath );
		bx::strCat( fsName, BX_COUNTOF(fsName), "fs_cubes.bin" );

		mProgram = app::loadProgram( vsName, fsName );
		mVbh = bgfx::createVertexBuffer( bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) ), PosColorVertex::ms_decl );
		mIbh = bgfx::createIndexBuffer( bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList) ) );
		bgfx::setDebug( BGFX_DEBUG_TEXT );
		mTime = 0.0f;
	}

	void TFinBgfx::reset()
	{
		bgfx::reset( mApp->getWidth(), mApp->getHeight(), mApp->getReset() );
		bgfx::setViewClear( 0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0 );
		bgfx::setViewRect( 0, 0, 0, uint16_t( mApp->getWidth() ), uint16_t( mApp->getHeight() ) );
	}

	void TFinBgfx::update( float dt )
	{
		mTime += dt;
        const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };
        float view[16];
		bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(mApp->getWidth())/float(mApp->getHeight()), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);
		bgfx::setViewRect( 0, 0, 0, uint16_t( mApp->getWidth() ), uint16_t( mApp->getHeight() ) );
		bgfx::touch( 0 );
		for ( uint32_t yy = 0; yy < 11; ++yy )
		{
			for ( uint32_t xx = 0; xx < 11; ++xx )
			{
                float mtx[16];
                bx::mtxRotateXY(mtx, mTime + xx*0.21f, mTime + yy*0.37f);
                mtx[12] = -15.0f + float(xx)*3.0f;
                mtx[13] = -15.0f + float(yy)*3.0f;
                mtx[14] = 0.0f;
				bgfx::setTransform(mtx);
				bgfx::setVertexBuffer( 0, mVbh );
				bgfx::setIndexBuffer( mIbh );
				bgfx::setState( BGFX_STATE_DEFAULT );
				bgfx::submit( 0, mProgram );
			}
		}
	}

	int TFinBgfx::shutdown()
	{
		bgfx::destroy( mProgram );
		bgfx::destroy( mVbh );
		bgfx::destroy( mIbh );
		bgfx::shutdown();
		return 0;
	}
private:
	app::Application *mApp;
	bgfx::ProgramHandle mProgram;
	bgfx::VertexBufferHandle mVbh;
	bgfx::IndexBufferHandle mIbh;
	float mTime;
};

int main( int argc, char** argv )
{
	app::Application app;
	return app.run( argc, argv );
}