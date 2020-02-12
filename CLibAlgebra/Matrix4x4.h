#pragma once

/*
*	@brief ４ｘ４の行列を表すクラス
*/
class CMatrix4x4 : public CMatrix
{
private:
	double* m_dData[ 16 ];

	void Init(){
		for( int i=0; i<16; i++ ){ m_dData[ i ] = m_Matrix->data.db +i; }			
	}

public:
	CMatrix4x4():CMatrix( 4,4 ){
		Init();
	}
	CMatrix4x4( double *data ):CMatrix( 4,4,data ){
		Init();
	}

	void SetI(){
		SetScale( 1.0, 1.0, 1.0 );
	}
	void Copy( CMatrix4x4 *mat ){
		//SetData( mat->GetData() );
		*this = *mat;
	}

	void SetRow( int row, CVector3H *v ){
		memcpy( GetData( row ), v->GetData(), 4*sizeof(double) );
	}
	void SetCol( int col, CVector3H *v ){
		for( int r=0; r<GetRows(); r++ ){
			m_Matrix->data.db[ r * GetCols() + col ] = v->GetData()[ r ];
		}
	}
	void GetRow( int row, CVector3H *v ){
		memcpy( v->GetData(), GetData( row ), 4*sizeof(double) );
	}
	void GetCol( int col, CVector3H *v ){
		for( int r=0; r<GetRows(); r++ ){
			v->SetData( r, m_Matrix->data.db[ r * GetCols() + col ] );
		}
	}

	void SetScale(double scaleX, double scaleY, double scaleZ );
	void SetRotX(double angX);
	void SetRotY(double angY);
	void SetRotZ(double angZ);
	void SetTran(CVector3H *vt);
	void SetRotYXZTran(double angX, double angY, double angZ, CVector3H *vt=NULL);
	void SetRotYXZxTran(double angX, double angY, double angZ, CVector3H *vt);

	void Transpose( CMatrix *matIn ){
		CMatrix::Transpose( matIn );
	}
	void Transpose(){
		CMatrix4x4 matTmp; matTmp.Copy( this );
		Transpose( &matTmp );
	}
};
