#include "StdAfx.h"
#include "Gl2Skin.h"

/*!
	@brief �X�L���Q���R�s�[����
	@param skin �R�s�[��
*/
void CGl2Skin::Copy( CGl2Skin *pSkin )
{
	CGlSkin::Copy( (CGlSkin*)pSkin );

	/* Tex */
	if( pSkin->m_ArrTex1 != NULL && m_ArrTex1 == NULL )
	{
		m_ArrTex1 = new CArrayVector3H( pSkin->m_ArrTex1->GetRows(), pSkin->m_ArrTex1->GetData() );
	}
	if( pSkin->m_ArrTex2 != NULL && m_ArrTex2 == NULL )
	{
		m_ArrTex2 = new CArrayVector3H( pSkin->m_ArrTex2->GetRows(), pSkin->m_ArrTex2->GetData() );
	}

	m_TexIds[ 0 ] = pSkin->m_TexIds[ 0 ];
	m_TexIds[ 1 ] = pSkin->m_TexIds[ 1 ];
	m_TexIds[ 2 ] = pSkin->m_TexIds[ 2 ];
	m_TexIds[ 3 ] = pSkin->m_TexIds[ 3 ];
}

/*!
	@brief �e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �e�N�X�`���摜
	@param[in] isClamped �e�N�X�`����[�ŌŒ肷�邩
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGl2Skin::MakeTexture( int unitInd, ChImgC3 *pImg, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	int tid = texturing->MakeTexture( pImg );
	if( tid < 0 )return -1;

	if( unitInd == 0 ){
		if( m_ArrTex == NULL )m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
		m_nTexId = tid;
		m_TexIds[ 0 ] = tid;
	}else if( unitInd == 1 ){
		if( m_ArrTex1 == NULL )m_ArrTex1 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 1 ] = tid;
	}else if( unitInd == 2 ){
		if( m_ArrTex2 == NULL )m_ArrTex2 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 2 ] = tid;
	}else if( unitInd == 3 ){
		if( m_ArrTex3 == NULL )m_ArrTex3 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 3 ] = tid;
	}

	return m_nTexId;
}

/*!
	@brief �A���t�@�t���e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �A���t�@�t���e�N�X�`���摜
	@param[in] isClamped �e�N�X�`����[�ŌŒ肷�邩
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGl2Skin::MakeTextureWithAlpha( int unitInd, ChImgC4 *pImg, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	int tid = texturing->MakeTextureWithAlpha( pImg );
	if( tid < 0 )return -1;

	if( unitInd == 0 ){
		if( m_ArrTex == NULL )m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
		m_nTexId = tid;
		m_TexIds[ 0 ] = tid;
	}else if( unitInd == 1 ){
		if( m_ArrTex1 == NULL )m_ArrTex1 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 1 ] = tid;
	}else if( unitInd == 2 ){
		if( m_ArrTex2 == NULL )m_ArrTex2 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 2 ] = tid;
	}else if( unitInd == 3 ){
		if( m_ArrTex3 == NULL )m_ArrTex3 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 3 ] = tid;
	}

	m_bAlphaBlend = true;	///// �A���t�@�E�u�����h�̐ݒ�

	return m_nTexId;
}

/*!
	@brief �e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] pImg �e�N�X�`���摜�̏ꏊ
	@param[in] isClamped �e�N�X�`����[�ŌŒ肷�邩
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGl2Skin::MakeTexture( int unitInd, char *locFile, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	int tid = texturing->MakeTexture( locFile );
	if( tid < 0 )return -1;

	if( unitInd == 0 ){
		if( m_ArrTex == NULL )m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
		m_nTexId = tid;
		m_TexIds[ 0 ] = tid;
	}else if( unitInd == 1 ){
		if( m_ArrTex1 == NULL )m_ArrTex1 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 1 ] = tid;
	}else if( unitInd == 2 ){
		if( m_ArrTex2 == NULL )m_ArrTex2 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 2 ] = tid;
	}else if( unitInd == 3 ){
		if( m_ArrTex3 == NULL )m_ArrTex3 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 3 ] = tid;
	}

	return m_nTexId;
}

int CGl2Skin::MakeTextureWithAlpha( int unitInd, char *locFile, bool isClamped, unsigned char r, unsigned char g, unsigned char b )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	int tid = texturing->MakeTextureWithAlpha( locFile, r,g,b );
	if( tid < 0 )return -1;

	if( unitInd == 0 ){
		if( m_ArrTex == NULL )m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
		m_nTexId = tid;
		m_TexIds[ 0 ] = tid;
	}else if( unitInd == 1 ){
		if( m_ArrTex1 == NULL )m_ArrTex1 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 1 ] = tid;
	}else if( unitInd == 2 ){
		if( m_ArrTex2 == NULL )m_ArrTex2 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 2 ] = tid;
	}else if( unitInd == 3 ){
		if( m_ArrTex3 == NULL )m_ArrTex3 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 3 ] = tid;
	}

	m_bAlphaBlend = true;	///// �A���t�@�E�u�����h�̐ݒ�

	return m_nTexId;
}

/*!
	@brief �e�N�X�`�����X�L���ɓ\��B�e�N�X�`���͂��ׂẴX�L���ɋ��L�����
	@param[in] locFile �e�N�X�`���摜�̏ꏊ
	@param[in] locAlpha �A���t�@�E�C���[�W�̏ꏊ
	@param[in] isClamped �e�N�X�`����[�ŌŒ肷�邩
	@return �e�N�X�`���ԍ���Ԃ�
*/
int CGl2Skin::MakeTextureWithAlpha( int unitInd, char *locFile, char *locAlpha, bool isClamped )
{
	CGlTextureImpl *texturing = CGlTexture::Get();
	texturing->EnableClamp( isClamped );
	int tid = texturing->MakeTextureWithAlpha( locFile, locAlpha );
	if( tid < 0 )return -1;

	if( unitInd == 0 ){
		if( m_ArrTex == NULL )m_ArrTex = new CArrayVector3H( m_ArrPos->GetRows() );
		m_nTexId = tid;
		m_TexIds[ 0 ] = tid;
	}else if( unitInd == 1 ){
		if( m_ArrTex1 == NULL )m_ArrTex1 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 1 ] = tid;
	}else if( unitInd == 2 ){
		if( m_ArrTex2 == NULL )m_ArrTex2 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 2 ] = tid;
	}else if( unitInd == 3 ){
		if( m_ArrTex3 == NULL )m_ArrTex3 = new CArrayVector3H( m_ArrPos->GetRows() );
		m_TexIds[ 3 ] = tid;
	}

	m_bAlphaBlend = true;	///// �A���t�@�E�u�����h�̐ݒ�

	return m_nTexId;
}

/*!
	@brief �e�N�X�`�����W�����g�̖@�����狁�߂�B
	@param[in] texInd �쐬����e�N�X�`�����W�̃C���f�b�N�X
*/
bool CGl2Skin::CalcTex_FromSphereNormal( int texInd )
{
	if( m_ArrNrm == NULL || m_ArrPos == NULL )return false;
	CArrayVector3H *targetTex = GetTargetTex( texInd );
	if( targetTex == NULL )return false;

	int cntRow = m_ArrNrm->GetRows();

	double *pNrm = m_ArrNrm->GetData();
	double *pTex = targetTex->GetData();
	for( int t=0; t<cntRow; t++ )
	{
		pTex[1] = 1.0 - acos( pNrm[1] ) / PI;
		if( pNrm[0] == 0.0 && pNrm[2] == 0.0 ){
			pTex[0] = 0.5;
		}else{
			pTex[0] = -atan2( pNrm[0], -pNrm[2] ) / (2.0*PI) + 0.5;
		}
		pNrm += 4;	pTex += 4;
	}
	return true;
}

/*!
	@brief �X�L����`�悷��
	@param[in] type �ǂ̃X�L���E�^�C�v��`�悷�邩
*/
void CGl2Skin::Draw( char *type )
{
	if( type != NULL && strcmp( m_sType, type ) != 0 )return;
	if( m_ArrIndex == NULL )return;

	bool isLightOff = false;

	CGlTextureImpl *texturing = CGlTexture::Get();
	if( m_TexIds[ 0 ] >= 0 ){
		texturing->BindUnit0( m_TexIds[ 0 ] );
	}
	if( m_TexIds[ 1 ] >= 0 ){
		texturing->BindUnit1( m_TexIds[ 1 ] );
	}
	if( m_TexIds[ 2 ] >= 0 ){
		texturing->BindUnit2( m_TexIds[ 2 ] );
	}

	if( m_bAlphaBlend == true )
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	if(m_ColAmbient == NULL && m_ColDiffuse == NULL && m_ColSpecular == NULL)
	{
		glDisable( GL_LIGHTING );
		glDisable( GL_LIGHT0 );
		isLightOff = true;
	}
	if( m_ColAmbient != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColAmbient->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_AMBIENT, tmpCol );
	}
	if( m_ColDiffuse != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColDiffuse->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmpCol );
	}
	if( m_ColSpecular != NULL )
	{
		float tmpCol[4];
		double *pCol = m_ColSpecular->GetData();
		tmpCol[0] = (float)pCol[0];	tmpCol[1] = (float)pCol[1];	tmpCol[2] = (float)pCol[2];	tmpCol[3] = (float)pCol[3];
		glMaterialfv(GL_FRONT, GL_SPECULAR, tmpCol );
		glMaterialf(GL_FRONT, GL_SHININESS, 20.0f);
	}
	///// �J���[�E�t�B���^�[
	glColor4dv( m_ColFilter.GetData() );

	if( m_ArrPos != NULL )
	{
		glEnableClientState(GL_VERTEX_ARRAY); glVertexPointer( 4, GL_DOUBLE, 0, m_ArrPos->GetData() );
	}
	if( m_ArrNrm != NULL )
	{
		glEnableClientState(GL_NORMAL_ARRAY); glNormalPointer( GL_DOUBLE, 32, m_ArrNrm->GetData() );
	}
	if( m_ArrCol != NULL )
	{
		glEnableClientState(GL_COLOR_ARRAY); glColorPointer( 4, GL_DOUBLE, 0, m_ArrCol->GetData() );
	}

	if( m_ArrTex != NULL && m_TexIds[ 0 ] >= 0 )
	{
		glClientActiveTexture( GL_TEXTURE0 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_DOUBLE, 32, m_ArrTex->GetData() );
	}
	if( m_ArrTex1 != NULL && m_TexIds[ 1 ] >= 0 )
	{
		glClientActiveTexture( GL_TEXTURE1 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_DOUBLE, 32, m_ArrTex1->GetData() );
	}
	if( m_ArrTex2 != NULL && m_TexIds[ 2 ] >= 0 )
	{
		glClientActiveTexture( GL_TEXTURE2 );
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_DOUBLE, 32, m_ArrTex2->GetData() );
	}

	if( m_ArrIndex != NULL )
	{
		//m_ArrIndex->TestPrint( "Index" );
		glDrawElements( m_Primitive, m_ArrIndex->GetRowsCols(), GL_UNSIGNED_INT, m_ArrIndex->GetData() );
	}

	if( m_TexIds[ 0 ] >= 0 ){
		texturing->UnbindUnit();
	}
	if( m_TexIds[ 1 ] >= 0 ){
		texturing->UnbindUnit();
	}
	if( m_TexIds[ 2 ] >= 0 ){
		texturing->UnbindUnit();
	}

	if( m_bAlphaBlend == true )
	{
		glEnable(GL_CULL_FACE);
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	if( isLightOff == true )
	{
		glEnable( GL_LIGHTING );
	}
}
