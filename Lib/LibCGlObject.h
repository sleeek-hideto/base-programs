#pragma once

//#include <gl/gl.h>
#include <time.h>
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>

#include "LibChData.h"
#include "LibChImage.h"
#include "LibCAlgebra.h"

/*!
	@brief 抽象クラス・ファクトリー
*/
class IGlFactory
{
public:
	virtual void Create()=0;
	virtual void Destroy()=0;

	virtual void Draw()=0;
	virtual void MotionMouse( int x, int y )=0;
	virtual void ButtonMouse( int btn, int state, int x, int y )=0;
	virtual void Idle()=0;
	virtual void Reshape( int w, int h )=0;
	virtual void Keyboard( unsigned char key, int x, int y )=0;
};

/*!
	@brief 抽象クラス・オブジェクト
*/
class IGlObject
{
public:
	virtual bool LoadObject( char *locFile )=0;
	virtual bool LoadPosture( Ch3PlainData *data, int frame, char *name )=0;

	//virtual void SetPos(){};
};

/*!
	@brief 抽象 Draw クラス・オブジェクト
*/
class IGlObjectDraw
{
public:
	virtual bool MakeObject(){ return false; }
	virtual bool MakeObjectWithAlpha(){ return false; }
	virtual bool LoadObject( char *locFile )=0;
	virtual bool LoadPosture( Ch3PlainData *data, int frame, char *name )=0;

	virtual void SetPos(){}
	virtual void SetAngle(){}
	virtual void SetColor(){}

	virtual void Draw()=0;
};

#include "../CLibGlObject/IGlSolver.h"

#include "../CLibGlObject/GlTexture.h"
#include "../CLibGlObject/GlSkin.h"
#include "../CLibGlObject/GlBone.h"
#include "../CLibGlObject/Gl2Skin.h"
#include "../CLibGlObject/Gl2Bone.h"
#include "../CLibGlObject/IGlLoaderModel.h"
#include "../CLibGlObject/GlLoaderModelWaveFront.h"
#include "../CLibGlObject/GlLoaderModelXFile.h"
#include "../CLibGlObject/GlLoaderModelMetasequoia.h"
#include "../CLibGlObject/GlCamera.h"
#include "../CLibGlObject/GlLight.h"
#include "../CLibGlObject/GlArthro.h"
#include "../CLibGlObject/GlClay.h"
#include "../CLibGlObject/GlBillBoard.h"
#include "../CLibGlObject/GlWindowBoard.h"
#include "../CLibGlObject/GlRenderering.h"

#include "../CLibGlObject/GlEvent.h"
#include "../CLibGlObject/GlSolverPendulum.h"
#include "../CLibGlObject/GlSolverIK.h"

#pragma comment(lib , "glew32.lib")