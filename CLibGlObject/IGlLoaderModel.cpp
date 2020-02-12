#include "StdAfx.h"
#include "IGlLoaderModel.h"

/*
	@param index�̍ގ��̃X�L�������
	@param index �X�L���E�C���f�b�N�X
	@param skin �ΏۂƂ���X�L��
*/
bool IGlLoaderModel::MakeSkin( int index, CGlSkin *skin )
{
	if( MakeDataPolygon( index, skin ) == false )return false;

	if( m_EnableDebug )printf( "Loader::MakeSkin() | use grp[%d]\n", index );

	///// �F
	double colAmbi[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };
	double colDiff[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };
	double colSpec[ 4 ] = { 0.1, 0.1, 0.1, 1.0 };

	///// �}�e���A����\��t����
	int indMtl;
	if( m_DataSkin.get( index, "useMtl", &indMtl ) > 0 )
	{
		GetSkinColAmbi( indMtl, colAmbi );
		GetSkinColDiff( indMtl, colDiff );
		GetSkinColSpec( indMtl, colSpec );
	}

	skin->SetColor( colAmbi, colDiff, colSpec );

	///// �e�N�X�`��
	char locTex[ 128 ];
	if( indMtl >= 0 && GetSkinLocTex( indMtl, locTex ) == true ){
		if( skin->MakeTexture( locTex ) >= 0 ){
			if( m_EnableDebug )printf( "Loader::MakeSkin() | load texture %s\n", locTex );
		}
		if( GetDataSkinTex( index ) == NULL )skin->CalcTex_FromSphereNormal();
	}

	return true;
}

/*
	@param index�̍ގ��̃X�L�������
	@param index �X�L����
	@param skin �ΏۂƂ���X�L��
*/
bool IGlLoaderModel::MakeSkin( char *name, CGlSkin *skin )
{
	int index = Grp2Index( name );
	if( index >= GetAmtSkins() || index < 0 )return false;
	return MakeSkin( index, skin );
}