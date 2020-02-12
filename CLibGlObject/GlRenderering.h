#pragma once

/*
	@brief レンダリングを行うクラス
	@booking Texture0を普通のテクスチャに、Texture1をトゥーンのテクスチャに、
		Texture2をデプスのテクスチャに使う予定である
*/
class CGlRenderingImpl
{
public:
	char m_Log[ 1024 ];

protected:
	const static int LIGHT_MAX = 4;

	int m_nWindowId;	//!<OpenGL用のウインドウＩＤ
	int m_WindowSize[ 2 ];	//!<ウインドウの幅、高さ

	CGlCamera *m_Camera;
	CGlLight *m_ArrLight[ LIGHT_MAX ];

	float m_ColClear[4];

	clock_t m_TimeStart;

public:
	HDC m_WinDC;
	HGLRC m_HRC;

public:
	CGlRenderingImpl(){
		m_Log[ 0 ] = 0;

		m_nWindowId = 0;

		m_WindowSize[ 0 ] = 720;
		m_WindowSize[ 1 ] = 480;

		m_Camera = NULL;
		for( int n=0; n<LIGHT_MAX; n++ ){ m_ArrLight[ n ] = NULL; }

		m_ColClear[0] = 0.0f; m_ColClear[1] = 0.0f; m_ColClear[2] = 0.0f; m_ColClear[3] = 1.0f;
	}
	~CGlRenderingImpl(){
	}

///// 初期化・終了化・描写
	void CreateWindowGl(int width, int height, int x=0, int y=0, char *name=NULL);
	void StartRendering();
	int FinishRendering( int timeInterval=0 );
	int GetTimeRendering(){ glFinish(); return (int)(clock()-m_TimeStart); }

	void CreateContext( HDC h );
	void CreateContext( HWND h ){ CreateContext( GetDC( h ) ); }
	void StartContextRendering( HDC h=NULL );
	void FinishContextRendering();
	void DestroyContext( HDC h=NULL );

	void SetClearColor( float r, float g, float b, float a=1.0f ){
		m_ColClear[0] = r; m_ColClear[1] = g; m_ColClear[2] = b; m_ColClear[3] = a;
//		glClearColor( r,g,b,a );
	}
	void SetCamera( CGlCamera *cam ){ m_Camera = cam; }
	CGlCamera *GetCamera(){ return m_Camera; }
	void SetLight( unsigned int i, CGlLight *light ){ if(i<LIGHT_MAX)m_ArrLight[i]=light; }
	CGlLight *GetLight( unsigned int i=0 ){ if(i>=LIGHT_MAX)return NULL; return m_ArrLight[i]; }
	void SetWindowSize( int width, int height );

///// デバッグ
	inline void TestPrintCameraPos(){
		if( m_Camera==NULL )fprintf( stderr, "No Camera\n" );
		m_Camera->TestPrintPos();
	}
	inline void TestPrintCameraAngle(){
		if( m_Camera==NULL )fprintf( stderr, "No Camera\n" );
		m_Camera->TestPrintAngle();
	}

	void DrawDisplay( ChImgC3* img );
	void DrawDisplay( ChImgC4* img );
	void CaptureDisplay( ChImgC3* img, int left=0, int top=0 );
	void CaptureDepth( ChImgC1* img, int left=0, int top=0 );
	void CaptureDepth( ChImgF1* img, int left=0, int top=0 );

///// ぼかし
	bool InitEffectMotionBlur(){
		glClear( GLUT_ACCUM );///// バッファの掃除
	}
	bool CallEffectMotionBlur( float value=1.75 );
	bool CallEffectFog( float r, float g, float b, float zNear, float zFar );
	void RejectEffectFog(){ glDisable( GL_FOG ); }

///// シェーダ
protected:
	bool LoadSrcShader( char *loc, unsigned int indShader );
	void printShaderInfoLog( unsigned int indShader );
	void printProgramInfoLog( unsigned int program );
public:
	bool CheckShader(){ if( glCreateShader==NULL )return false; return true; }
	unsigned int InitEffectShader( char *locSrcVertex, char *locSrcFragment );
	bool CallEffectShader( unsigned int program, bool samplerTex0=false, bool samplerTex1=false, bool samplerTex2=false );
	void DetachEffectShader(){
		if( glUseProgram != NULL )glUseProgram( 0 );
	}

	unsigned int InitEffectShaderToon( char *locSrcVertex, char *locSrcFragment, char *locImgToon, int &texId );
	bool CallEffectShaderToon( unsigned int program, int texId );

	unsigned int InitEffectShaderShadow( char *locSrcVertex, char *locSrcFragment, int &texId );
	bool CallEffectShaderShadowPreCalcStart( unsigned int program, double depNear, double depFar, CVector3H *focus );
	bool CallEffectShaderShadowPreCalcFinish( int texId );
	bool CallEffectShaderShadow( unsigned int program, int texIdShadow );
};

/*
*	@brief レンダリング・シングルトン
*/
class CGlRendering
{
protected:
	const static int WINDOW_MAX = 4;	//!<ウインドウを作成できる数
	static CGlRenderingImpl **m_Rendering;	//!<インプリを格納

	static void Create(){
		m_Rendering = new CGlRenderingImpl* [ WINDOW_MAX ];
		for( int w=0; w<WINDOW_MAX; w++ ){
			m_Rendering[ w ] = NULL;
		}
	}

public:
	static CGlRenderingImpl *Get(int idWindow=0){
		if( idWindow >= WINDOW_MAX )return NULL;
		if( m_Rendering == NULL )Create();
		if( m_Rendering[ idWindow ] == NULL )m_Rendering[ idWindow ] = new CGlRenderingImpl;
		return m_Rendering[ idWindow ];
	}
	static void Destroy(){
		if( m_Rendering == NULL )return;
		for( int w=0; w<WINDOW_MAX; w++ ){
			delete m_Rendering[ w ];
		}
		delete [] m_Rendering;
		m_Rendering = NULL;
	}
};
