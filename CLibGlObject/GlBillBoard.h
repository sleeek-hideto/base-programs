#pragma once

class CGlBillBoard : public IGlObjectDraw
{
protected:
	int m_BoardMax;
	CGl2Bone *m_Bone;
	CGl2Skin *m_Skin;

public:
	CGlBillBoard(void){
		m_BoardMax = 0;
		m_Bone = NULL; m_Skin = NULL;
	}
	~CGlBillBoard(void){
		if( m_Bone != NULL ){ delete [] m_Bone; m_Bone=NULL; }
		if( m_Skin != NULL ){ delete [] m_Skin; m_Skin=NULL; }
	}

	void Create( int cntBoard );

	bool MakeObject( int id, char *locFile );
	bool MakeObject( int id, ChImgC3 *img );
	bool MakeObjectWithAlpha( int id, char *locFile, unsigned char r=0, unsigned char g=0, unsigned char b=0 );
	bool MakeObjectWithAlpha( int id, char *locFile, char *locAlpha );
	bool MakeObjectWithAlpha( int id, ChImgC4 *img );

	bool LoadObject( Ch3PlainData *data );
	bool LoadObject( char *locFile ){
		Ch3PlainData data;
		if( data.Load( locFile ) == false )return false;
		return LoadObject( &data );
	}
	bool LoadPosture( Ch3PlainData *data, int frame, char *name=NULL );

	//void SetScale( double x, double y ){ m_Skin.Scale( x,y,1.0 ); }
	//void SetScale( CVector3H *v ){ m_Skin.Scale( v ); }

	void SetPos( int id, double x, double y, double z ){ double tmp[3]={x,y,z}; m_Bone[id].SetPos(tmp); }
	void SetPos( int id, double *pos ){ m_Bone[id].SetPos(pos); }
	void SetPos( int id, CVector3H *v ){ m_Bone[id].SetPos(v); }

	void SetScale( int id, double height=1.0, double width=1.0 );
	void SetSize( int id, double height=2.0, double width=2.0 );
	void SetClipTex( int id, double left=0.0, double bottom=0.0, double right=1.0, double head=1.0 );

	void SetColor( int id, double r, double g, double b, double a ){ double col[4]={r,g,b,a}; m_Skin[id].SetColorFilter(col); }
	void SetColor( int id, double *col ){ m_Skin[id].SetColorFilter(col); }
	void SetColor( int id, CVector4 *col ){ m_Skin[id].SetColorFilter(col); }

	void Draw();

private:
	bool MakePolygon( int id );
};
