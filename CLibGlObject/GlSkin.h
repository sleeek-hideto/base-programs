#pragma once

class CGlBone;

/*
*	@brief スケルトン・モデルのスキン部分のクラス
*/
class CGlSkin
{
public:
	char m_sType[ 16 ];	//!<スキンの種類名
	CGlBone *m_BoneParent;	//!<このスキンが貼り付けられるボーン
	CGlSkin *m_SkinSibling;	//!<このスキンの兄弟
	CArrayVector3H *m_ArrPos, *m_ArrNrm, *m_ArrCol, *m_ArrTex;	//!<頂点、法線、色、テクスチャ座標を格納する配列
	CUintArrayVector3 *m_ArrIndex;	//!<インデックスを格納
	GLenum m_Primitive;	//!<プリミティブの形
	CVector4 *m_ColAmbient, *m_ColDiffuse, *m_ColSpecular;	//!<スキンの色
	CVector4 m_ColFilter;//!<原色

	int m_nTexId;	//!<テクスチャID
	bool m_bAlphaBlend;	//!<アルファ・ブレンドを使うかどうか

public:
	CGlSkin(){
		m_sType[0] = 0;
		m_BoneParent = NULL;
		m_SkinSibling = NULL;
		m_ArrPos = NULL;	m_ArrNrm = NULL;	m_ArrCol = NULL;	m_ArrTex = NULL;
		m_ArrIndex = NULL;
		m_ColAmbient = NULL;	m_ColDiffuse = NULL;	m_ColSpecular = NULL;
		m_ColFilter.SetData( 1,1,1,1 );

		m_nTexId = -1;
		m_bAlphaBlend = false;
	}
	~CGlSkin(){
		if( m_ArrPos != NULL ){	delete m_ArrPos;	m_ArrPos = NULL;	}
		if( m_ArrNrm != NULL ){	delete m_ArrNrm;	m_ArrNrm = NULL;	}
		if( m_ArrCol != NULL ){	delete m_ArrCol;	m_ArrCol = NULL;	}
		if( m_ArrTex != NULL ){	delete m_ArrTex;	m_ArrTex = NULL;	}
		if( m_ArrIndex != NULL ){	delete m_ArrIndex;	m_ArrIndex = NULL;	}
		if( m_ColAmbient != NULL ){	delete m_ColAmbient;	m_ColAmbient = NULL;	}
		if( m_ColDiffuse != NULL ){	delete m_ColDiffuse;	m_ColDiffuse = NULL;	}
		if( m_ColSpecular != NULL ){	delete m_ColSpecular;	m_ColSpecular = NULL;	}
	}

	void Copy( CGlSkin *pSkin );
	void SetType( char* type ){
		strcpy( m_sType, type );
	}
	bool IsType( char* type ){
		if( strcmp( m_sType, type ) == 0 )return true;
		return false;
	}

	void MakeCube( double length=1.0 );
	void MakeSphere( double radius=1.0, int height=16, int width=32 );
	void MakePolygon( int posCnt, double *pPos, int indCnt, unsigned int *pInd, int nrmCnt=0, double *pNrm=NULL, int texCnt=0, double *pTex=NULL );
	bool CalcNormal();
	bool CalcTex_FromSphereNormal();

	bool LoadSkinUser( Ch2PlainData *data, int frame, bool &isNormalSet, bool &isTexSet );
	bool LoadSkinStorage( Ch3PlainData *data, char *nameSpace, bool &isNormalSet, bool &isTexSet );
	//bool LoadSkinMassWaveFront( char *locFile, bool &isNormalSet, bool &isTexSet );
	//bool LoadSkinMassWaveFront( Ch2PlainData *data, int frame, bool &isNormalSet, bool &isTexSet ){
	//	char locFile[ 128 ];
	//	if( data->get( frame, "locWaveFront", locFile ) == 0 )return false;
	//	return LoadSkinMassWaveFront( locFile, isNormalSet, isTexSet );
	//}

	void SetPosRow( int r, double *pos ){
		m_ArrPos->SetDataRow( r, pos );
	}
	void SetIndexRow( int r, unsigned int *index ){
		m_ArrIndex->SetDataRow( r, index );
	}
	void SetNormalRow( int r, double *nrm ){
		m_ArrNrm->SetDataRow( r, nrm );
	}
	void SetTexRow( int r, double *tex ){
		m_ArrTex->SetDataRow( r, tex );
	}
	double* GetPosRow( int r ){
		return m_ArrPos->GetData( r );
	}
	unsigned int* GetIndexRow( int r ){
		return m_ArrIndex->GetData( r );
	}
	double* GetNormalRow( int r ){
		return m_ArrNrm->GetData( r );
	}
	double* GetTexRow( int r ){
		return m_ArrTex->GetData( r );
	}

	void Shift( CVector3H *vec );
	void Shift( double x, double y, double z );
	void Scale( CVector3H *vec );
	void Scale( double x, double y, double z );
	void Rotate( CVector3H *vec );
	void Rotate( double x, double y, double z );
	void Project( CMatrix4x4 *mat );

	virtual void Draw( char *type=NULL );

	int MakeTexture( ChImgC3 *pImg, bool isClamped=false );
	int MakeTextureWithAlpha( ChImgC4 *pImg, bool isClamped=false );
	int MakeTexture( char *locFile, bool isClamped=false );
	int MakeTextureWithAlpha( char *locFile, bool isClamped=false, unsigned char r=0, unsigned char g=0, unsigned char b=0 );

	/*!
		@brief makeTextureによって作られたテクスチャを、makeTextureが返した値texIdをセットすることで別のスキンにも使用できる。
		@param[in] texId makeTextureが返した値。テクスチャID
		@param[in] alphaBlend アルファ・ブレンドするかどうか
	*/
	void SetTexture( int texId, bool alphaBlend=false ){
		m_nTexId=texId;	m_bAlphaBlend=alphaBlend;
	}
	int GetTexture(){	return m_nTexId;	}

	void SetColor( double *ambi, double *diff, double *spec );
	void SetColor( CVector4* ambi, CVector4 *diff, CVector4 *spec ){
		double *dataAmbi=NULL, *dataDiff=NULL, *dataSpec=NULL;
		if( ambi != NULL )dataAmbi=ambi->GetData();
		if( diff != NULL )dataDiff=diff->GetData();
		if( spec != NULL )dataSpec=spec->GetData();
		SetColor( dataAmbi, dataDiff, dataSpec );
	}
	void UnsetColor(){
		m_bAlphaBlend = false;
		if( m_ColAmbient != NULL ){ delete m_ColAmbient; m_ColAmbient=NULL; }
		if( m_ColDiffuse != NULL ){ delete m_ColDiffuse; m_ColDiffuse=NULL; }
		if( m_ColSpecular != NULL ){ delete m_ColSpecular; m_ColSpecular=NULL; }
	}
	void SetColorFilter( double *col ){
		m_ColFilter.SetData( col );
		if( col[3] < 1.0 ){ m_bAlphaBlend=true; }else{ m_bAlphaBlend=false; }
	}
	void SetColorFilter( CVector4 *col ){
		m_ColFilter.Copy( col );
		if( col->GetData()[3] < 1.0 ){ m_bAlphaBlend=true; }else{ m_bAlphaBlend=false; }
	}

	CGlSkin *GetSibling(){	return m_SkinSibling;	}
};

/*
class CGlSkinDeform : public CGlSkin
{
public:
	CGlSkinDeform *m_SkinWorld, *m_SkinDeformed;	//!<世界座標内でのスキン、デフォームされたスキン
	CMatrix4x4 m_MatDeformed;

	CGlSkinDeform(){
		m_SkinWorld = NULL;
		m_SkinDeformed = NULL;
		m_MatDeformed.SetI();
	}

	CGlSkinDeform *GetSibling()						{	return (CGlSkinDeform*)m_SkinSibling;	}

	void SetSkinWorld( CGlSkinDeform *skin )		{	m_SkinWorld = skin;	}
	CGlSkinDeform *GetSkinWorld()					{	return m_SkinWorld;	}
	void SetSkinDeformed( CGlSkinDeform *skin )		{	m_SkinDeformed = skin;	}
	CGlSkinDeform *GetSkinDeformed()				{	return m_SkinDeformed;	}

	void SetMatDeformed( double* data )				{	m_MatDeformed.SetData( data );	}
	void SetMatDeformed( CMatrix4x4* mat )			{	m_MatDeformed.Copy( mat );	}
	CMatrix4x4* GetMatDeformed()					{	return &m_MatDeformed;	}
};
*/