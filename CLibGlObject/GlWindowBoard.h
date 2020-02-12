#pragma once

class CGlWindowBoard : public IGlObjectDraw
{
protected:
	int m_BoardMax;
	CGl2Bone *m_Bone;
	CGl2Skin *m_Skin;

public:
	CGlWindowBoard(void){
		m_BoardMax = 0;
		m_Bone = NULL; m_Skin = NULL;
	}
	~CGlWindowBoard(void){
		if( m_Bone != NULL )delete [] m_Bone;
		if( m_Skin != NULL )delete [] m_Skin;
	}

	void Create( int cntBoard );

	bool MakeObject( int ind, char *locFile );
	bool MakeObject( int ind, ChImgC3 *img );
	bool MakeObjectWithAlpha( int ind, char *locFile, unsigned char r=0, unsigned char g=0, unsigned char b=0 );
	bool MakeObjectWithAlpha( int ind, char *locFile, char *locAlpha );
	bool MakeObjectWithAlpha( int ind, ChImgC4 *img );

	bool LoadObject( Ch2PlainData *data );
	bool LoadObject( char *locFile ){
		Ch2PlainData data;
		if( data.load( locFile )==false ) return false;
		return LoadObject( &data );
	}
	bool LoadObject( Ch3PlainData *data );
	bool LoadPosture( Ch3PlainData *data, int frame, char *name=NULL );

	void SetPos( int ind, double x, double y, double z ){ if(ind>=m_BoardMax)return; double p[3]={x,y,z}; m_Bone[ind].SetPos(p); }
	void SetPos( int ind, double *pos ){ if(ind>=m_BoardMax)return; m_Bone[ind].SetPos(pos); }
	void SetPos( int ind, CVector3H *p ){ if(ind>=m_BoardMax)return; m_Bone[ind].SetPos(p); }

	void SetAngle( int ind, double x, double y, double z ){ if(ind>=m_BoardMax)return; double a[3]={x,y,z}; m_Bone[ind].SetAngle(a); }
	void SetAngle( int ind, double *angle ){ if(ind>=m_BoardMax)return; m_Bone[ind].SetAngle(angle); }
	void SetAngle( int ind, CVector3H *angle ){ if(ind>=m_BoardMax)return; m_Bone[ind].SetAngle(angle); }

	void SetScale( int ind, double x, double y );
	void SetSize( int ind, double height=2.0, double width=2.0 );
	void SetClipTex( int ind, double left=0.0, double bottom=0.0, double right=1.0, double head=1.0 );

	void SetColor( int ind, double r, double g, double b, double a ){ if(ind>=m_BoardMax)return; double col[4]={r,g,b,a}; m_Skin[ind].SetColorFilter(col); }
	void SetColor( int ind, double *col ){ if(ind>=m_BoardMax)return; m_Skin[ind].SetColorFilter(col); }
	void SetColor( int ind, CVector4 *col ){ if(ind>=m_BoardMax)return; m_Skin[ind].SetColorFilter(col); }

	void Draw();

private:
	bool MakePolygon( int ind );
};
