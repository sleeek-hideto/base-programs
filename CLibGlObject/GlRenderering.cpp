#include "StdAfx.h"
#include ".\glrenderering.h"

CGlRenderingImpl **CGlRendering::m_Rendering = NULL;

/*!
	@brief �E�C���h�E��V�������
	@param[in] height ����
	@param[in] width ��
	@param[in] x �w�ʒu
	@param[in] y �x�ʒu
	@param[in] name �E�C���h�E��
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

	///// GLEW�������A���̏ꏊ�ł悢���H
	GLenum err;
	err = glewInit();
	if (err != 0){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
}

/*!
	@brief �����_�����O���n�߂�
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
	@brief �����_�����O���I���
	@return �����_�����O�Ɏg�p�������Ԃ�Ԃ�(ms)
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
	@brief �f�o�C�X�E�R���e�L�X�g�ɂ��`�ʂ̏�����������
	@param[in] h �E�C���h�E�E�n���h��
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

	///// GLEW�������A���̏ꏊ�ł悢���H
	GLenum err;
	err = glewInit();
	if (err != 0){
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	return;
}

/*!
	@brief �f�o�C�X�E�R���e�L�X�g�ɂ��P�t���[���`�ʂ��͂��߂�
	@param[in] h �E�C���h�E�E�n���h��
*/
void CGlRenderingImpl::StartContextRendering( HDC h )
{
	if( h != NULL ) m_WinDC = h;
	wglMakeCurrent( m_WinDC, m_HRC );
	StartRendering();
}
/*!
	@brief �f�o�C�X�E�R���e�L�X�g�ɂ��P�t���[���`�ʂ��I���
	@param[in] h �E�C���h�E�E�n���h��
*/
void CGlRenderingImpl::FinishContextRendering()
{
	glFinish();
	SwapBuffers( m_WinDC );
}

/*!
	@brief �f�o�C�X�E�R���e�L�X�g�ɂ��`�ʂ̏I����������
*/
void CGlRenderingImpl::DestroyContext( HDC h )
{
	if( h != NULL ) m_WinDC = h;
	wglMakeCurrent( m_WinDC, 0 );
	wglDeleteContext( m_HRC );
}

/*!
	@brief �E�C���h�E�T�C�Y�𒲐�����
	@param[in] height ����
	@param[in] width ��
*/
void CGlRenderingImpl::SetWindowSize( int width, int height )
{
	m_WindowSize[0] = width;
	m_WindowSize[1] = height;
//	glViewport( 0, 0, width, height );
}

/*!
	@brief ���C�g�O�̑��z����ݒ肷��
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
	@brief �E�C���h�E�E�C���[�W���C���[�Wimg�ɏ�������
	@param[in] �C���[�W
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
	@brief �E�C���h�E�E�C���[�W���C���[�Wimg�ɏ�������
	@param[in] �C���[�W
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
	@brief �E�C���h�E�E�C���[�W���L���v�`������
	@param[out] img �L���v�`�����ꂽ�C���[�W
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
	@brief �E�C���h�E�E�f�v�X���C���[�Wimg�ɏ�������
	@param[in] �C���[�W
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
	@brief �E�C���h�E�E�f�v�X���C���[�Wimg�ɏ������ށB�l��0����1�܂ł̊�
	@param[in] �C���[�W
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
	@brief ���o���ʂ̃��[�V�����E�u���[���s���B�����_�����O���ɖ���Ăяo������
	@param[in] value �l��1.0���傫�����̂�����A�l���u�������v�قǂԂ��
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
	@brief ���o���ʂ̃t�H�O���N����
	@param[in] r,g,b �t�H�O�̐F�̐ݒ�
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
	@brief �^����ꂽ�ʒuloc�̃V�F�[�_��ǂݍ��݃R���p�C������
	@param[in] loc �V�F�[�_�ʒu
	@param[in] indShader �V�F�[�_�ԍ�
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
  
	/* �t�@�C���T�C�Y�̎擾 */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	/* �V�F�[�_�v���O�����̓ǂݍ��ݗ̈���m�� */
	if( ( buf = (void *)malloc(size) ) == NULL )
	{
		fprintf(stderr, "Memory is not enough for %s\n", loc);
		fclose(fp);
		return false;
	}
  
	/* �t�@�C����擪����ǂݍ��� */
	fseek( fp, 0, SEEK_SET );
	fread( buf, 1, size, fp );

	/* �V�F�[�_�I�u�W�F�N�g�Ƀv���O�������Z�b�g */
	glShaderSource( indShader, 1, (const GLchar **)&buf, &size );
  
	/* �V�F�[�_�ǂݍ��ݗ̈�̉�� */
	free( buf );
	fclose( fp );

	/* �V�F�[�_�̃R���p�C�� */
	glCompileShader( indShader );
	glGetShaderiv( indShader, GL_COMPILE_STATUS, &compiled );
	printShaderInfoLog( indShader );		/* �R���p�C�����O�̏o�� */
	if( compiled == GL_FALSE )
	{
		fprintf( stderr, "Compile error in %s!!\n", loc );
		return false;
	}

	return true;
}

/*
	@brief �V�F�[�_�̃R���p�C�����ʂ�\��
	@param[in] program �V�F�[�_�ԍ�
*/
void CGlRenderingImpl::printShaderInfoLog( unsigned int shader )
{
	int logSize = 0;
	int len;

	/* ���O�̒����́A�Ō��NULL�������܂� */
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &logSize);

	if (logSize > 1)
	{
		glGetShaderInfoLog(shader, 1024, &len, m_Log);
		fprintf(stderr, "Shader Info Log\n%s\n", m_Log);
	}
}

/*
	@brief �V�F�[�_�E�v���O�����̃����N���ʂ�\��
	@param[in] program �V�F�[�_�E�v���O����
*/
void CGlRenderingImpl::printProgramInfoLog( unsigned int program )
{
	int logSize;
	int len;

	/* ���O�̒����́A�Ō��NULL�������܂� */
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
	@brief �n���ꂽ�\�[�X����V�F�[�_������������
	@param[in] locSrcVertex ���_�V�F�[�_�̃\�[�X�ʒu
	@param[in] locSrcFragment �t���O�����g�V�F�[�_�̃\�[�X�ʒu
	@return �V�F�[�_�E�v���O�����̔ԍ��A�|�P���ƃG���[
*/
unsigned int CGlRenderingImpl::InitEffectShader( char *locSrcVertex, char *locSrcFragment )
{
	if( locSrcVertex == NULL || locSrcFragment == NULL )return 0;
	if( glCreateShader == NULL )return 0;

	/* �x���_�[��GPU */
	char gpuStr[256];
	sprintf_s( gpuStr, sizeof(gpuStr), "%s: OpenGL %s", glGetString(GL_RENDERER), glGetString(GL_VERSION) );

	unsigned int vtxShader=0, frgShader=0, prog=0;
	int linked;

	/* �V�F�[�_�I�u�W�F�N�g�̍쐬 */
	vtxShader = glCreateShader( GL_VERTEX_SHADER );
	frgShader = glCreateShader( GL_FRAGMENT_SHADER );

	/* �o�[�e�b�N�X�V�F�[�_�̃��[�h�ƃR���p�C�� */
	if( LoadSrcShader( locSrcVertex, vtxShader ) == false ){
		return 0;
	}

	/* �t���O�����g�V�F�[�_�̃��[�h�ƃR���p�C�� */
	if( LoadSrcShader( locSrcFragment, frgShader ) == false ){
		return 0;
	}

	/* �v���O�����I�u�W�F�N�g�̍쐬 */
	prog = glCreateProgram();
	//printProgramInfoLog( prog );

	/* �V�F�[�_�I�u�W�F�N�g�̃V�F�[�_�v���O�����ւ̓o�^ */
	glAttachShader(prog, vtxShader);
	glAttachShader(prog, frgShader);
	//printProgramInfoLog( prog );

	/* �V�F�[�_�I�u�W�F�N�g�̍폜 */
	glDeleteShader(vtxShader);
	glDeleteShader(frgShader);

	/* �V�F�[�_�v���O�����̃����N */
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
	@brief �V�F�[�_�E�v���O�������g�p����
	@param[in] �v���O�����ԍ�
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
	@brief �g�D�[���E�����_�����O������������
	@param[in] locSrcVertex ���_�V�F�[�_�̃\�[�X�ʒu
	@param[in] locSrcFragment �t���O�����g�V�F�[�_�̃\�[�X�ʒu
	@param[in] locImgToon �g�D�[���E�C���[�W�̏ꏊ
	@param[out] nameTex �e�N�X�`����
	@return program �v���O�����E�C���f�b�N�X
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
	@param �g�D�[���E�����_�����O�����s����
	@param �v���O�����E�C���f�b�N�X
*/
bool CGlRenderingImpl::CallEffectShaderToon( unsigned int program, int texId ){
	if( glUseProgram == 0 )return false;
	if( program == 0 || texId < 0 )return false;

	glUseProgram( program );
	glUniform1i( glGetUniformLocation( program, "sampler0" ), 0 );
	glUniform1i( glGetUniformLocation( program, "sampler1" ), 1 );

	///// �J�����ʒu�̌v�Z
	float fpos[ 3 ] ={ 0,0,0 };
	if( m_Camera != NULL )m_Camera->GetPosCamera()->GetData( fpos );
	glUniform3fv( glGetUniformLocation( program, "posCamera" ), 1, fpos );

	///// �e�N�X�`���̐ݒ�
	CGlTexture::Get()->BindUnit1( texId );

	return true;
}

/*!
	@brief �J���[�E�e�[�u�����g���A�F�����F�����邱�ƂŁA���o���ʂ̃g�D�[���E�����_�����O���s���B
	@param[in] program �v���O�����E�C���f�b�N�X
	@param[in] border �����x���ɕ����邩
bool CGlRenderingImpl::CallEffectShaderToonSubtractiveColor( unsigned int program, unsigned char level )
{
	if( glUseProgram == 0 )return false;
	if( level < 2 )return false;

	///// GL �̊g�����g����ꍇ
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
	@brief �V���h�E�̏�����
	@param[in] locSrcVertex ���_�V�F�[�_�̃\�[�X�ʒu
	@param[in] locSrcFragment �t���O�����g�E�V�F�[�_�̃\�[�X�ʒu
	@param[out] �m�ۂ��ꂽ�V���h�E�E�e�N�X�`����ID
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
	@brief �V���h�E�̑O�������n�߂�B���̌�ɁA�I�u�W�F�N�g��`�ʂ���
	@param[in] program �V�F�[�_�E�v���O����
	@param[in] depNear,depFar �`�ʂ���Ώۂ̍ŏ������ƍő勗��
	@param[in] focus �J�����������ʒu
*/
bool CGlRenderingImpl::CallEffectShaderShadowPreCalcStart( unsigned int program, double depNear, double depFar, CVector3H *focus )
{
	const int width = 512;
	const int height = 512;

	///// �G�t�F�N�g�̏���
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

//	glDisable( GL_LIGHTING );/////�A�e�t�����s�v�Ȃ̂Ń��C�e�B���O���I�t�ɂ���
//	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );/////�F��t���Ȃ�

//	glCullFace( GL_FRONT );/////�f�v�X�o�b�t�@�ɂ͔w�ʂ̃|���S���̉��s�����L�^����悤�ɂ���
	return true;
}

/*!
	@brief �V���h�E�̑O�������I���B
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

	///// ���C�g�s��
	if( GetLight( 0 ) != NULL ){
		//int mem = glGetUniformLocation( program, "proLight" );
		//glUniformMatrix4fv( glGetUniformLocation( program, "proLight" ), 1, GL_TRUE/*�]�n�s��łȂ�->TRUE*/, GetLight( 0 )->GetMatPro() );
		glUniformMatrix4fv( glGetUniformLocation( program, "proLight" ), 1, GL_FALSE/*�]�n�s���->FALSE*/, GetLight( 0 )->GetMatPro() );
	}

	///// �e�N�X�`���̐ݒ�
	CGlTexture::Get()->BindUnit3( texIdShadow );

	return true;
}
