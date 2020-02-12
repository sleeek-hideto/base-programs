#include "StdAfx.h"
#include ".\glrenderering.h"

CGlRenderingImpl **CGlRendering::m_Rendering = NULL;

/*!
	@brief ウインドウを新しく作る
	@param[in] height 高さ
	@param[in] width 幅
	@param[in] x Ｘ位置
	@param[in] y Ｙ位置
	@param[in] name ウインドウ名
*/
void CGlRenderingImpl::CreateWindowGl(int width, int height, int x, int y, char* name)
{
	//glClearColor( m_ColClear[0], m_ColClear[1], m_ColClear[2], m_ColClear[3] );
	glClearAccum( 0.0, 0.0, 0.0, 1.0 );
	glShadeModel( GL_SMOOTH );
	glPolygonMode( GL_FRONT, GL_FILL );
	//glPolygonMode(GL_BACK, GL_LINE);

	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_ACCUM );
	glutInitWindowSize( width, height );
	m_WindowSize[ 0 ] = width;
	m_WindowSize[ 1 ] = height;
	glutInitWindowPosition( x, y );
	if( name==NULL )
	{
		m_nWindowId = glutCreateWindow( "Window GL" );
	}else{
		m_nWindowId = glutCreateWindow( name );
	}

	///// GLEW初期化、この場所でよいか？
	GLenum err;
	err = glewInit();
	if (err != 0){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
}

/*!
	@brief レンダリングを始める
*/
void CGlRenderingImpl::StartRendering()
{
	m_TimeStart = clock();

	glViewport( 0, 0, m_WindowSize[ 0 ], m_WindowSize[ 1 ] );
	glShadeModel( GL_SMOOTH );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glEnable( GL_DEPTH_TEST );

	glDrawBuffer( GL_BACK );
	glClearColor( m_ColClear[0], m_ColClear[1], m_ColClear[2], m_ColClear[3] );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

	///// GL Model View
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	///// GL Projection
	if( m_Camera == NULL ){
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glOrtho( -1, 1, -1, 1, -1, 1 );
	}else{
		m_Camera->SendMatrixProjection();
	}
}

/*!
	@brief レンダリングを終わる
	@return レンダリングに使用した時間を返す(ms)
*/
int CGlRenderingImpl::FinishRendering( int timeInterval )
{
	glutSwapBuffers();

	int timeWork = (int)( clock() -m_TimeStart );
	if( timeInterval -timeWork > 0 ){
		Sleep( timeInterval -timeWork );
	}
	return timeWork;
}

/*!
	@brief デバイス・コンテキストによる描写の初期化をする
	@param[in] h ウインドウ・ハンドラ
*/
void CGlRenderingImpl::CreateContext( HDC h )
{
	PIXELFORMATDESCRIPTOR pfd = { 
		sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd 
		1,                     // version number 
		PFD_DRAW_TO_WINDOW |   // support window 
		PFD_SUPPORT_OPENGL |   // support OpenGL 
		PFD_DOUBLEBUFFER,      // double buffered 
		PFD_TYPE_RGBA,         // RGBA type 
		32,                    // 24-bit color depth 
		0, 0, 0, 0, 0, 0,      // color bits ignored 
		0,                     // no alpha buffer 
		0,                     // shift bit ignored 
		0,                     // no accumulation buffer 
		0, 0, 0, 0,            // accum bits ignored 
		32,                    // 32-bit z-buffer 
		0,                     // no stencil buffer 
		0,                     // no auxiliary buffer 
		PFD_MAIN_PLANE,        // main layer 
		0,                     // reserved 
		0, 0, 0                // layer masks ignored 
	};

	m_WinDC = h;

	int pixelFormat = ChoosePixelFormat( m_WinDC, &pfd );
	SetPixelFormat( m_WinDC, pixelFormat, &pfd );

	m_HRC = wglCreateContext( m_WinDC );
	wglMakeCurrent( m_WinDC, m_HRC );

	///// GLEW初期化、この場所でよいか？
	GLenum err;
	err = glewInit();
	if (err != 0){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	return;
}

/*!
	@brief デバイス・コンテキストによる１フレーム描写をはじめる
	@param[in] h ウインドウ・ハンドラ
*/
void CGlRenderingImpl::StartContextRendering( HDC h )
{
	if( h != NULL ) m_WinDC = h;
	wglMakeCurrent( m_WinDC, m_HRC );
	StartRendering();
}
/*!
	@brief デバイス・コンテキストによる１フレーム描写を終わる
	@param[in] h ウインドウ・ハンドラ
*/
void CGlRenderingImpl::FinishContextRendering()
{
	glFinish();
	SwapBuffers( m_WinDC );
}

/*!
	@brief デバイス・コンテキストによる描写の終了化をする
*/
void CGlRenderingImpl::DestroyContext( HDC h )
{
	if( h != NULL ) m_WinDC = h;
	wglMakeCurrent( m_WinDC, 0 );
	wglDeleteContext( m_HRC );
}

/*!
	@brief ウインドウサイズを調整する
	@param[in] height 高さ
	@param[in] width 幅
*/
void CGlRenderingImpl::SetWindowSize( int width, int height )
{
	m_WindowSize[0] = width;
	m_WindowSize[1] = height;
//	glViewport( 0, 0, width, height );
}

/*!
	@brief ライト０の太陽光を設定する
void CGlRenderingImpl::SetLight0( CVector3H *pos, CVector4 *ambi, CVector4 *diff, CVector4 *spec )
{
	if( pos != NULL )
	{
		m_Light0Pos[ 0 ] = (float)pos->GetData()[0];
		m_Light0Pos[ 1 ] = (float)pos->GetData()[1];
		m_Light0Pos[ 2 ] = (float)pos->GetData()[2];
	}
	if( ambi != NULL )
	{
		m_Light0ColAmbi[ 0 ] = (float)ambi->GetData()[0];
		m_Light0ColAmbi[ 1 ] = (float)ambi->GetData()[1];
		m_Light0ColAmbi[ 2 ] = (float)ambi->GetData()[2];
		m_Light0ColAmbi[ 3 ] = (float)ambi->GetData()[3];
	}
	if( diff != NULL )
	{
		m_Light0ColDiff[ 0 ] = (float)diff->GetData()[0];
		m_Light0ColDiff[ 1 ] = (float)diff->GetData()[1];
		m_Light0ColDiff[ 2 ] = (float)diff->GetData()[2];
		m_Light0ColDiff[ 3 ] = (float)diff->GetData()[3];
	}
	if( spec != NULL )
	{
		m_Light0ColSpec[ 0 ] = (float)spec->GetData()[0];
		m_Light0ColSpec[ 1 ] = (float)spec->GetData()[1];
		m_Light0ColSpec[ 2 ] = (float)spec->GetData()[2];
		m_Light0ColSpec[ 3 ] = (float)spec->GetData()[3];
	}

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, m_Light0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, m_Light0ColAmbi );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, m_Light0ColDiff);
	glLightfv(GL_LIGHT0, GL_SPECULAR, m_Light0ColSpec);
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
}
*/

/*!
	@brief ウインドウ・イメージをイメージimgに書き込む
	@param[in] イメージ
*/
void CGlRenderingImpl::DrawDisplay( ChImgC3* img )
{
	if( img->getIplImage() == NULL )return;

	ChImgC3 iBuffer;
	iBuffer.clone(img);
	iBuffer.convertRGBtoBGR();
	iBuffer.convertUPtoDOWN();

	//glDrawBuffer(GL_BACK);
	glDepthMask( GL_FALSE );
	glDrawPixels( iBuffer.getWidth(), iBuffer.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, iBuffer.getOrigin() );
	glDepthMask( GL_TRUE );
}

/*!
	@brief ウインドウ・イメージをイメージimgに書き込む
	@param[in] イメージ
*/
void CGlRenderingImpl::DrawDisplay( ChImgC4* img )
{
	if( img->getIplImage() == NULL )return;

	ChImgC4 iBuffer;
	iBuffer.clone(img);
	iBuffer.convertRGBAtoBGRA();
	iBuffer.convertUPtoDOWN();

	//glDrawBuffer(GL_BACK);
	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawPixels( iBuffer.getWidth(), iBuffer.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, iBuffer.getOrigin() );

	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

/*!
	@brief ウインドウ・イメージをキャプチャする
	@param[out] img キャプチャされたイメージ
*/
void CGlRenderingImpl::CaptureDisplay( ChImgC3* img, int left, int top )
{
//	if( img->getWidth() != m_WindowSize[0] || img->getHeight() != m_WindowSize[1] )return;

	glReadBuffer(GL_BACK);
	glReadPixels( left, top, img->getWidth(), img->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, img->getOrigin() );
	img->convertRGBtoBGR();
	img->convertUPtoDOWN();
}

/*!
	@brief ウインドウ・デプスをイメージimgに書き込む
	@param[in] イメージ
*/
void CGlRenderingImpl::CaptureDepth( ChImgC1 *img, int left, int top )
{
//	if( img->getWidth() != m_WindowSize[0] || img->getHeight() != m_WindowSize[1] )return;

	//glutSetWindow( m_nWindowId );
	glReadBuffer(GL_BACK);
	glReadPixels( left, top, img->getWidth(), img->getHeight(), GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, img->getOrigin() );
	img->convertUPtoDOWN();
}

/*!
	@brief ウインドウ・デプスをイメージimgに書き込む。値は0から1までの間
	@param[in] イメージ
*/
void CGlRenderingImpl::CaptureDepth( ChImgF1 *img, int left, int top )
{
//	if( img->getWidth() != m_WindowSize[0] || img->getHeight() != m_WindowSize[1] )return;

	//glutSetWindow( m_nWindowId );
	glReadBuffer(GL_BACK);
	glReadPixels( left, top, img->getWidth(), img->getHeight(), GL_DEPTH_COMPONENT, GL_FLOAT, img->getOrigin() );
	img->convertUPtoDOWN();
}

/*!
	@brief 視覚効果のモーション・ブラーを行う。レンダリング中に毎回呼び出すこと
	@param[in] value 値は1.0より大きいものを入れる、値が「小さい」ほどぶれる
*/
bool CGlRenderingImpl::CallEffectMotionBlur( float value )
{
	if( value < 1.0 )return false;

	glAccum( GL_ACCUM, (value - 1.0) / value );
	//glAccum( GL_ACCUM, 1.0 / value );
	glAccum( GL_RETURN, value );
	glAccum( GL_MULT, 1.0 / value );
	//int e = glGetError();
	return true;
}

/*!
	@brief 視覚効果のフォグを起こす
	@param[in] r,g,b フォグの色の設定
*/
bool CGlRenderingImpl::CallEffectFog( float r, float g, float b, float zNear, float zFar )
{
	if( zFar <= 0.0f || zNear >= zFar ){
		glDisable( GL_FOG );
		return false;
	}

	float colFog[4] = { r,g,b,1.0 };
	glEnable( GL_FOG );
	glFogfv( GL_FOG_COLOR, colFog );

	//glFogi( GL_FOG_MODE, GL_EXP );
	//glFogi( GL_FOG_MODE, GL_EXP2 );
	//glFogf( GL_FOG_DENSITY, level );

	glFogi( GL_FOG_MODE, GL_LINEAR );
	glFogf( GL_FOG_START, zNear );
	glFogf( GL_FOG_END, zFar );

	glHint( GL_FOG_HINT, GL_DONT_CARE );
	return true;
}

/*
	@brief 与えられた位置locのシェーダを読み込みコンパイルする
	@param[in] loc シェーダ位置
	@param[in] indShader シェーダ番号
*/
bool CGlRenderingImpl::LoadSrcShader( char *loc, unsigned int indShader )
{
	errno_t err;
	FILE *fp;
	void *buf;
	int size;
	GLint compiled;

	if( ( err = fopen_s(&fp, loc, "rb") ) != 0 )
	{
		fprintf(stderr, "%s is not found!!\n", loc);
		return false;
	}
  
	/* ファイルサイズの取得 */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	/* シェーダプログラムの読み込み領域を確保 */
	if( ( buf = (void *)malloc(size) ) == NULL )
	{
		fprintf(stderr, "Memory is not enough for %s\n", loc);
		fclose(fp);
		return false;
	}
  
	/* ファイルを先頭から読み込む */
	fseek( fp, 0, SEEK_SET );
	fread( buf, 1, size, fp );

	/* シェーダオブジェクトにプログラムをセット */
	glShaderSource( indShader, 1, (const GLchar **)&buf, &size );
  
	/* シェーダ読み込み領域の解放 */
	free( buf );
	fclose( fp );

	/* シェーダのコンパイル */
	glCompileShader( indShader );
	glGetShaderiv( indShader, GL_COMPILE_STATUS, &compiled );
	printShaderInfoLog( indShader );		/* コンパイルログの出力 */
	if( compiled == GL_FALSE )
	{
		fprintf( stderr, "Compile error in %s!!\n", loc );
		return false;
	}

	return true;
}

/*
	@brief シェーダのコンパイル結果を表示
	@param[in] program シェーダ番号
*/
void CGlRenderingImpl::printShaderInfoLog( unsigned int shader )
{
	int logSize = 0;
	int len;

	/* ログの長さは、最後のNULL文字も含む */
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logSize);

	if (logSize > 1)
	{
		glGetShaderInfoLog(shader, 1024, &len, m_Log);
		fprintf(stderr, "Shader Info Log\n%s\n", m_Log);
	}
}

/*
	@brief シェーダ・プログラムのリンク結果を表示
	@param[in] program シェーダ・プログラム
*/
void CGlRenderingImpl::printProgramInfoLog( unsigned int program )
{
	int logSize;
	int len;

	/* ログの長さは、最後のNULL文字も含む */
	glGetProgramiv(program, GL_INFO_LOG_LENGTH , &logSize);

	if (logSize > 1)
	{
		glGetProgramInfoLog(program, 1024, &len, m_Log);
		fprintf(stderr, "Program Info Log\n%s\n", m_Log);
	}else{
		m_Log[ 0 ] = 0;
	}
}

/*
	@brief 渡されたソースからシェーダを初期化する
	@param[in] locSrcVertex 頂点シェーダのソース位置
	@param[in] locSrcFragment フラグメントシェーダのソース位置
	@return シェーダ・プログラムの番号、－１だとエラー
*/
unsigned int CGlRenderingImpl::InitEffectShader( char *locSrcVertex, char *locSrcFragment )
{
	if( locSrcVertex == NULL || locSrcFragment == NULL )return 0;
	if( glCreateShader == NULL )return 0;

	/* ベンダーとGPU */
	char gpuStr[256];
	sprintf_s( gpuStr, sizeof(gpuStr), "%s: OpenGL %s", glGetString(GL_RENDERER), glGetString(GL_VERSION) );

	unsigned int vtxShader=0, frgShader=0, prog=0;
	int linked;

	/* シェーダオブジェクトの作成 */
	vtxShader = glCreateShader( GL_VERTEX_SHADER );
	frgShader = glCreateShader( GL_FRAGMENT_SHADER );

	/* バーテックスシェーダのロードとコンパイル */
	if( LoadSrcShader( locSrcVertex, vtxShader ) == false ){
		return 0;
	}

	/* フラグメントシェーダのロードとコンパイル */
	if( LoadSrcShader( locSrcFragment, frgShader ) == false ){
		return 0;
	}

	/* プログラムオブジェクトの作成 */
	prog = glCreateProgram();
	//printProgramInfoLog( prog );

	/* シェーダオブジェクトのシェーダプログラムへの登録 */
	glAttachShader(prog, vtxShader);
	glAttachShader(prog, frgShader);
	//printProgramInfoLog( prog );

	/* シェーダオブジェクトの削除 */
	glDeleteShader(vtxShader);
	glDeleteShader(frgShader);

	/* シェーダプログラムのリンク */
	glLinkProgram( prog );
	glGetProgramiv( prog, GL_LINK_STATUS, &linked );
	printProgramInfoLog( prog );
	if ( linked == GL_FALSE )
	{
		fprintf(stderr, "Link error of %s & %s!!\n", locSrcVertex, locSrcFragment);
		return 0;
	}

	return prog;
}

/*
	@brief シェーダ・プログラムを使用する
	@param[in] プログラム番号
*/
bool CGlRenderingImpl::CallEffectShader( unsigned int program, bool samplerTex0, bool samplerTex1, bool samplerTex2 )
{	///// sample sample sample
	if( glUseProgram == NULL )return false;
	glUseProgram( program );
	if( program == 0 )return false;

	if( samplerTex0 ){
		int texLoc0 = glGetUniformLocation( program, "sampler0" );
		glUniform1i( texLoc0, 0 );
	}
	if( samplerTex1 ){
		int texLoc1 = glGetUniformLocation( program, "sampler1" );
		glUniform1i( texLoc1, 1 );
	}
	if( samplerTex2 ){
		int texLoc2 = glGetUniformLocation( program, "sampler2" );
		glUniform1i( texLoc2, 2 );
	}

	return true;
}

/*
	@brief トゥーン・レンダリングを初期化する
	@param[in] locSrcVertex 頂点シェーダのソース位置
	@param[in] locSrcFragment フラグメントシェーダのソース位置
	@param[in] locImgToon トゥーン・イメージの場所
	@param[out] nameTex テクスチャ名
	@return program プログラム・インデックス
*/
unsigned int CGlRenderingImpl::InitEffectShaderToon( char *locSrcVertex, char *locSrcFragment, char *locImgToon, int &texId )
{
	texId = -1;

	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( true );
	texId = texturing->MakeTexture( locImgToon );
	if( texId < 0 )return false;

	return InitEffectShader( locSrcVertex, locSrcFragment );
}

/*
	@param トゥーン・レンダリングを実行する
	@param プログラム・インデックス
*/
bool CGlRenderingImpl::CallEffectShaderToon( unsigned int program, int texId ){
	if( glUseProgram == 0 )return false;
	if( program == 0 || texId < 0 )return false;

	glUseProgram( program );
	glUniform1i( glGetUniformLocation( program, "sampler0" ), 0 );
	glUniform1i( glGetUniformLocation( program, "sampler1" ), 1 );

	///// カメラ位置の計算
	float fpos[ 3 ] ={ 0,0,0 };
	if( m_Camera != NULL )m_Camera->GetPosCamera()->GetData( fpos );
	glUniform3fv( glGetUniformLocation( program, "posCamera" ), 1, fpos );

	///// テクスチャの設定
	CGlTexture::Get()->BindUnit1( texId );

	return true;
}

/*!
	@brief カラー・テーブルを使い、色を減色させることで、視覚効果のトゥーン・レンダリングを行う。
	@param[in] program プログラム・インデックス
	@param[in] border 何レベルに分けるか
bool CGlRenderingImpl::CallEffectShaderToonSubtractiveColor( unsigned int program, unsigned char level )
{
	if( glUseProgram == 0 )return false;
	if( level < 2 )return false;

	///// GL の拡張が使える場合
	if( glColorTable != NULL )
	{
		unsigned char border = 256 / level;
		unsigned char tableGray[ 256 ];
		for( int v=0; v<256; v++ ){
			tableGray[ v ] = (v / border) * border;
		}
		glColorTable( GL_COLOR_TABLE, GL_LUMINANCE8, 256, GL_LUMINANCE, GL_UNSIGNED_BYTE, tableGray );
		glEnable( GL_COLOR_TABLE );
		return true;
	}

	glUseProgram( program );
	if( program == 0 )return false;
	glUniform1f( glGetUniformLocation( program, "uniLevel" ), (float)level );

	return true;
}
*/

/*!
	@brief シャドウの初期化
	@param[in] locSrcVertex 頂点シェーダのソース位置
	@param[in] locSrcFragment フラグメント・シェーダのソース位置
	@param[out] 確保されたシャドウ・テクスチャのID
*/
unsigned int CGlRenderingImpl::InitEffectShaderShadow( char *locSrcVertex, char *locSrcFragment, int &texId )
{
	const int width = 512;
	const int height = 512;
	texId = -1;

	CGlTextureImpl *texturing = CGlTexture::Get();
	texId = texturing->MakeTextureDepth( width, height );
	if( texId < 0 )return false;

	return InitEffectShader( locSrcVertex, locSrcFragment );
}

/*!
	@brief シャドウの前処理を始める。この後に、オブジェクトを描写する
	@param[in] program シェーダ・プログラム
	@param[in] depNear,depFar 描写する対象の最小距離と最大距離
	@param[in] focus カメラが向く位置
*/
bool CGlRenderingImpl::CallEffectShaderShadowPreCalcStart( unsigned int program, double depNear, double depFar, CVector3H *focus )
{
	const int width = 512;
	const int height = 512;

	///// エフェクトの除去
	RejectEffectFog();
	CallEffectShader( program );

	glDrawBuffer( GL_BACK );
	glViewport( 0,0, width, height );

	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );

	glClearColor( 1.0, 1.0, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glMatrixMode( GL_PROJECTION );
	//glPushMatrix();
	glLoadIdentity();
	gluPerspective( 60.0, 1.0, depNear, depFar );
	//glGetFloatv( GL_PROJECTION_MATRIX, GetLight( 0 )->GetMatPro() );
	float *posLight = GetLight( 0 )->GetPos();
	double *posFocus = focus->GetData();
	gluLookAt( posLight[0], posLight[1], posLight[2],
		posFocus[0], posFocus[1], posFocus[2],
		0, 1.0, 0
		);
	//glRotatef( 45.0, 1.0, 0.0, 0.0 );
	//glTranslatef( -posLight[0], -posLight[1], -posLight[2] );
	glGetFloatv( GL_PROJECTION_MATRIX, GetLight( 0 )->GetMatPro() );

	//glMatrixMode( GL_MODELVIEW );
	//glPushMatrix();
	//glLoadIdentity();

//	glDisable( GL_LIGHTING );/////陰影付けも不要なのでライティングをオフにする
//	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );/////色を付けない

//	glCullFace( GL_FRONT );/////デプスバッファには背面のポリゴンの奥行きを記録するようにする
	return true;
}

/*!
	@brief シャドウの前処理を終わる。
*/
bool CGlRenderingImpl::CallEffectShaderShadowPreCalcFinish( int texId )
{
	if( texId < 0 )return false;

	glFinish();

	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->OverDrawSubImageDepth( texId );

	return true;
}

bool CGlRenderingImpl::CallEffectShaderShadow( unsigned int program, int texIdShadow )
{
	if( glUseProgram == 0 )return false;
	if( program == 0 || texIdShadow < 0 )return false;

	glUseProgram( program );
	glUniform1i( glGetUniformLocation( program, "sampler0" ), 0 );
	glUniform1i( glGetUniformLocation( program, "shadow3" ), 3 );

	///// ライト行列
	if( GetLight( 0 ) != NULL ){
		//int mem = glGetUniformLocation( program, "proLight" );
		//glUniformMatrix4fv( glGetUniformLocation( program, "proLight" ), 1, GL_TRUE/*転地行列でない->TRUE*/, GetLight( 0 )->GetMatPro() );
		glUniformMatrix4fv( glGetUniformLocation( program, "proLight" ), 1, GL_FALSE/*転地行列を->FALSE*/, GetLight( 0 )->GetMatPro() );
	}

	///// テクスチャの設定
	CGlTexture::Get()->BindUnit3( texIdShadow );

	return true;
}
