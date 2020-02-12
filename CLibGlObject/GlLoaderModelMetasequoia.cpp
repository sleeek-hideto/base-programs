#include "StdAfx.h"
#include "GlLoaderModelMetasequoia.h"

/*!
	@brief �ގ��̉�͂��s��
	@param[in] file �t�@�C���E�|�C���^
	@param[inout] floorNow ����q�̊K�w
*/
bool CGlLoaderModelMetasequoia::RetrieveMaterial( FILE *file, int &floorNow )
{
	int floorStart = floorNow;

	int indexMtl = -1;
	char line[ 1024 ];
	while( fgets( line, 1024, file ) != NULL )
	{
		char *p = line;
		///// ���ʂ����邩�𒲂ׂ�
		while( *p!=0 ){ if( *p=='}' )floorNow--; if( *p=='{' )floorNow++; p++; }
		if( floorNow < floorStart )return true;

		char nameMat[ 64 ];
		//char strCol[ 64 ], strDif[ 64 ], strAmb[ 64 ], strEmi[ 64 ], strSpc[ 64 ], strPower[ 64 ], strTex[ 64 ], strApl[ 64 ];
		//strCol[0]=0; strDif[0]=0; strAmb[0]=0; strEmi[0]=0; strSpc[0]=0; strPower[0]=0; strTex[0]=0; strApl[0]=0;

		sscanf( line, "%s", nameMat );
		indexMtl++;/////����
		SetMtl2Index( PickupString( nameMat ), indexMtl );

		p = line;
		while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;

		double col[ 4 ], dif=0.0, amb=0.0, emi=0.0, spc=0.0, power=0.0;
		char tex[ 64 ]="", apl[ 64 ]="";
		char nameClass[ 64 ];
		double tmpCol[ 4 ];
		p = PickupClass( p, nameClass, col );
		p = PickupClass( p, nameClass, &dif );
		p = PickupClass( p, nameClass, &amb );
		p = PickupClass( p, nameClass, &emi );
		p = PickupClass( p, nameClass, &spc );
		p = PickupClass( p, nameClass, &power );
		p = PickupClass( p, nameClass, tex );
		p = PickupClass( p, nameClass, apl );

		///// Color Diff
		for( int c=0; c<3; c++ ){ tmpCol[c] = col[c]*dif; }
		tmpCol[3] = 1.0;
		m_DataMaterial.set( indexMtl, "colDiff", 4, tmpCol );

		///// Location Texture
		if( tex[ 0 ] == 0 )continue;
		m_UseTex = true;
		char *locTex = PickupString( tex );
			///// ����
		char tmpLoc[ 128 ];
		if( m_DirectoryNow[ 0 ] == 0 ){
			sprintf( tmpLoc, "%s", locTex );
		}else{
			sprintf( tmpLoc, "%s%s", m_DirectoryNow, locTex );
		}
		m_DataMaterial.set( indexMtl, "locTex", -1, tmpLoc );
	}
	return true;
}

/*!
	@brief �ގ��̉�͂��s��
	@param[in] file �t�@�C���E�|�C���^
	@param[in] index �X�L���E�C���f�b�N�X
	@param[inout] floorNow ����q�̊K�w
*/
bool CGlLoaderModelMetasequoia::RetrieveObject( FILE *file, int index, int &floorNow )
{
	if( index < 0 )return false;
	int floorStart = floorNow;

	char line[ 1024 ];
	char nameClass[ 64 ], namePrivate[ 64 ];
	int cntVer=0, cntInd=0;

	while( 1 )
	{
		int state = ScanFile( file, floorNow, nameClass, namePrivate );
		if( floorStart > floorNow )return true;/////�d�v
		if( state < 0 )return false;
		if( state < 2 )continue;

		if( strcmp( nameClass, "vertex" ) == 0 )
		{
			cntVer = atoi( namePrivate );
			if( cntVer < 1 )return false;
			m_ArrPos[ index ].Create( cntVer );
			if( m_UseTex )m_ArrTex[ index ].Create( cntVer );

			///// ���
			double *pPos = m_ArrPos[ index ].GetData();
			for( int c=0; c<cntVer; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%lf %lf %lf", pPos, pPos +1, pPos +2 );
				pPos[3] = 1.0;
				pPos += 4;
			}
			
		}else if( strcmp( nameClass, "face" ) == 0 ){
			/////���_���g���Ėʂ��\�z
			fpos_t posFaceFirst;
			fgetpos( file, &posFaceFirst );
			int cntFace = atoi( namePrivate );
			if( cntFace < 1 )return false;

			cntInd = 0;
			for( int c=0; c<cntFace; c++ ){
				int cntPoint;
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%d", &cntPoint );
				if( cntPoint == 3 )cntInd++;
				if( cntPoint == 4 )cntInd += 2;
			}
			if( cntInd < 1 )return false;
			m_ArrInd[ index ].Create( cntInd );

			fsetpos( file, &posFaceFirst );

			unsigned int *pInd = m_ArrInd[ index ].GetData();
			for( int c=0; c<cntFace; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				
				int cntPoint;
				sscanf( line, "%d", &cntPoint );
				char *p = line;
				while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;
				if( cntPoint == 3 )
				{
					int arrInd[ 3 ], numMtl;
					double arrTex[ 6 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 1 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;
				}else if( cntPoint == 4 ){
					int arrInd[ 4 ], numMtl;
					double arrTex[ 8 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 1 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;

					pInd[ 0 ] = (unsigned int)arrInd[ 3 ];
					pInd[ 1 ] = (unsigned int)arrInd[ 2 ];
					pInd[ 2 ] = (unsigned int)arrInd[ 0 ];
					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 2 ], rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 1 ], rowTex );
						rowTex[ 0 ] = arrTex[ 6 ];
						rowTex[ 1 ] = arrTex[ 7 ];
						m_ArrTex[ index ].SetDataRow( pInd[ 0 ], rowTex );
					}
					pInd += 3;
				}
			}
		}
	}
	return true;
}

/*!
	@brief �e�N�X�`�����W�����ƂɁA�ގ��̉�͂��s��
	@param[in] file �t�@�C���E�|�C���^
	@param[in] index �X�L���E�C���f�b�N�X
	@param[inout] floorNow ����q�̊K�w
*/
bool CGlLoaderModelMetasequoia::RetrieveObject_TextureNonUnified( FILE *file, int index, int &floorNow )
{
	if( index < 0 )return false;
	int floorStart = floorNow;

	char line[ 1024 ];
	char nameClass[ 64 ], namePrivate[ 64 ];
	int cntVer=0, cntInd=0;
	
	CArrayVector3H tmpArrPos;

	while( 1 )
	{
		int state = ScanFile( file, floorNow, nameClass, namePrivate );
		if( floorStart > floorNow )return true;/////�d�v
		if( state < 0 )return false;
		if( state < 2 )continue;

		if( strcmp( nameClass, "vertex" ) == 0 )
		{
			cntVer = atoi( namePrivate );
			if( cntVer < 1 )return false;
			tmpArrPos.Create( cntVer );

			///// ���
			double *pPos = tmpArrPos.GetData();
			for( int c=0; c<cntVer; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%lf %lf %lf", pPos, pPos +1, pPos +2 );
				pPos[3] = 1.0;
				pPos += 4;
			}
			
		}else if( strcmp( nameClass, "face" ) == 0 ){
			/////�܂��A�C���f�b�N�X�𒲂ׂāA�O�p�ʂ������K�v���𐔂���
			fpos_t posFaceFirst;
			fgetpos( file, &posFaceFirst );
			int cntFace = atoi( namePrivate );
			if( cntFace < 1 )return false;

			cntInd = 0;
			for( int c=0; c<cntFace; c++ ){
				int cntPoint;
				if( fgets( line, 1024, file ) == NULL )return false;
				sscanf( line, "%d", &cntPoint );
				if( cntPoint == 3 )cntInd++;
				if( cntPoint == 4 )cntInd += 2;
			}
			if( cntInd < 1 )return false;
			m_ArrInd[ index ].Create( cntInd );/////�C���f�b�N�X�z�񂾂��łȂ��A
			m_ArrPos[ index ].Create( cntInd*3 );/////���_�z��
			if( m_UseTex )m_ArrTex[ index ].Create( cntInd*3 );/////�e�N�X�`�����W�z��

			fsetpos( file, &posFaceFirst );

			int numInd = 0;
			unsigned int *pInd = m_ArrInd[ index ].GetData();
			for( int c=0; c<cntFace; c++ )
			{
				if( fgets( line, 1024, file ) == NULL )return false;
				
				int cntPoint;
				sscanf( line, "%d", &cntPoint );
				char *p = line;
				while( *p==' ' || *p=='\t' )p++; while( *p!=' ' && *p!='\t' && *p!=0 )p++; while( *p==' ' || *p=='\t' )p++;
				if( cntPoint == 3 )
				{
					int arrInd[ 3 ], numMtl;
					double arrTex[ 6 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[1] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[2] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}

					numInd += 3;
					pInd += 3;
				}else if( cntPoint == 4 ){
					int arrInd[ 4 ], numMtl;
					double arrTex[ 8 ], rowTex[ 3 ] = { 0,0,0 };
					char nameType[ 64 ];
					p = PickupClass( p, nameType, arrInd );
					p = PickupClass( p, nameType, &numMtl );
					p = PickupClass( p, nameType, arrTex );

					if( c == 0 )m_DataSkin.set( index, "useMtl", 1, &numMtl );
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[1] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[2] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 2 ];
						rowTex[ 1 ] = arrTex[ 3 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}

					numInd += 3;
					pInd += 3;

					/////��ڂ̎O�p
					pInd[ 0 ] = (unsigned int)numInd +2;
					pInd[ 1 ] = (unsigned int)numInd +1;
					pInd[ 2 ] = (unsigned int)numInd;
					m_ArrPos[ index ].SetDataRow( numInd, tmpArrPos.GetData( arrInd[0] ) );
					m_ArrPos[ index ].SetDataRow( numInd +1, tmpArrPos.GetData( arrInd[2] ) );
					m_ArrPos[ index ].SetDataRow( numInd +2, tmpArrPos.GetData( arrInd[3] ) );

					if( m_UseTex )
					{
						rowTex[ 0 ] = arrTex[ 0 ];
						rowTex[ 1 ] = arrTex[ 1 ];
						m_ArrTex[ index ].SetDataRow( numInd, rowTex );
						rowTex[ 0 ] = arrTex[ 4 ];
						rowTex[ 1 ] = arrTex[ 5 ];
						m_ArrTex[ index ].SetDataRow( numInd +1, rowTex );
						rowTex[ 0 ] = arrTex[ 6 ];
						rowTex[ 1 ] = arrTex[ 7 ];
						m_ArrTex[ index ].SetDataRow( numInd +2, rowTex );
					}
					pInd += 3;
				}
			}
		}
	}
	return true;
}

/*!
	@brief �X�L��skin�ɑ΂��āA�C���f�b�N�X�̃X�L��index�̒��_�f�[�^�݂̂��\�z����
	@param[in] index �C���f�b�N�X
	@param[inout] skin �ΏۂƂȂ�X�L��
*/
bool CGlLoaderModelMetasequoia::MakeDataPolygon( int index, CGlSkin *skin )
{
	if( index < -1 || index >= m_nAmtSkins )return false;
	if( skin == NULL )return false;

	///// ���_�f�[�^�����͊m��
	skin->MakePolygon(
		GetCntSkinPos( index ), GetDataSkinPos( index ),
		GetCntSkinInd( index ), GetDataSkinInd( index ),
		GetCntSkinNrm( index ), GetDataSkinNrm( index ),
		GetCntSkinTex( index ), GetDataSkinTex( index )
		);

	if( GetDataSkinNrm( index ) == NULL )skin->CalcNormal();

	return true;
}

/*!
	@brief ���^�Z�R�C�A�̃f�[�^��ǂݍ���
	@param[in] locFile �t�@�C���̈ʒu
*/
bool CGlLoaderModelMetasequoia::Load( char *locFile )
{
	///// �J�����g�E�f�B���N�g���̐ݒ�
	strcpy( m_DirectoryNow, locFile );
	int lenLoc = (int)strlen( m_DirectoryNow );
	char *pC = m_DirectoryNow +lenLoc -1;
	for( int c=0; c<lenLoc; c++ ){
		if( *pC == '/' || *pC == '\\' ){
			pC[ 1 ] = 0;
			break;
		}
		pC--;
	}

	FILE *file = fopen( locFile, "r" );
	setvbuf( file, NULL, _IONBF, 0 );
	if( file == NULL )return false;

	char line[ 1024 ];
	fgets( line, 1024, file );
	Trim( line );
	if( strcmp( line, "Metasequoia Document" ) != 0 )return false;

	fpos_t posFileFirst;
	fgetpos( file, &posFileFirst );

	char nameClass[ 64 ], namePrivate[ 64 ];
	///// �f�[�^�́u���v�̉��
	int floorNow = 0;
	m_nAmtSkins = 0;
	while( ScanFile( file, floorNow, nameClass, namePrivate ) >= 0 )
	{
		if( strcmp( nameClass, "Material" ) == 0 ){
			RetrieveMaterial( file, floorNow );
		}else if( strcmp( nameClass, "Object" ) == 0 ){
			SetGrp2Index( PickupString( namePrivate ), m_nAmtSkins );
			m_nAmtSkins++;
		}
	}
	if( m_nAmtSkins == 0 )return false;
	m_ArrPos = new CArrayVector3H[ m_nAmtSkins ];
	m_ArrInd = new CUintArrayVector3[ m_nAmtSkins ];
	if( m_UseTex )m_ArrTex = new CArrayVector3H[ m_nAmtSkins ];
	//m_ArrNrm = new CArrayVector3H[ m_nAmtSkins ];

	///// �f�[�^�́u���g�v�̉��
	fsetpos( file, &posFileFirst );
	floorNow = 0;
	while( ScanFile( file, floorNow, nameClass, namePrivate ) >= 0 )
	{
		if( strcmp( nameClass, "Object" ) == 0 ){
			if( m_IsTextureUnified ){
				RetrieveObject( file, Grp2Index( PickupString( namePrivate ) ), floorNow );
			}else{
				RetrieveObject_TextureNonUnified( file, Grp2Index( PickupString( namePrivate ) ), floorNow );
			}
		}
	}

	fclose( file );
	return true;
}

