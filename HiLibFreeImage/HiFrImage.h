#pragma once

class ChImgC1;
class ChImgC3;
class ChImgC4;
class ChImgF1;

class ChImageInterface /* ちゅーしょうクラス */
{
public:
	typedef unsigned char uchar;
	IplImage *my_Ipl;
	CvSize my_CvSize;

	ChImageInterface(void)				{ my_Ipl=NULL; }
	~ChImageInterface(void)				{ destroy(); }					
	virtual void destroy()				{ if(my_Ipl != NULL){ cvReleaseImage(&my_Ipl); my_Ipl=NULL; } }

	inline bool _isInROI(int h)			{ if(0<= h && h<my_Ipl->height){ return true; } return false; }
	inline bool _isInROI(int h, int w)	{ if(0<= h && h<my_Ipl->height && 0<=w && w<my_Ipl->width){ return true; } return false; }

	inline IplImage* getIplImage(void)	{ return my_Ipl; }
	inline int getHeight()				{ return my_Ipl->height; }
	inline int getWidth()				{ return my_Ipl->width; }
	inline int getChannel()				{ return my_Ipl->nChannels; }
	inline int getStep(void)			{ return my_Ipl->widthStep; }
	inline int getPixelSize(void)		{ return my_Ipl->height*my_Ipl->width; }
	inline int getByteSize(void)		{ return my_Ipl->imageSize; }
	inline CvSize* getCvSize(void)		{ return &my_CvSize; }

	//inline setPseudoChannel1()			{ my_Ipl->nChannels=1; my_Ipl->width *= 3; }
	//inline resetPseudoChannel1()		{ my_Ipl->nChannels=3; my_Ipl->width /= 3; }

	inline uchar* getOrigin()			{ return (unsigned char*)my_Ipl->imageData; }		
};

class ChImageChar : public ChImageInterface /* ちゅーしょうクラス */
{
protected:
	void _createUchar(int height, int width, int channel){
		my_CvSize.height = height; my_CvSize.width = width;
		my_Ipl = cvCreateImage(my_CvSize, IPL_DEPTH_8U, channel);
	}

public:
	virtual void create(int height, int width){}
	virtual bool load(char* locFile){ return false; }
	virtual bool save(char* locFile){ return false; }

	void clone(ChImageChar* img){
		destroy();
		_createUchar( img->getHeight(), img->getWidth(), img->getChannel() );
		memcpy( getOrigin(), img->getOrigin(), img->getByteSize() );
	}
	void clone(IplImage* img){
		destroy();
		_createUchar( img->height, img->width, img->nChannels );
		memcpy( getOrigin(), img->imageData, img->imageSize );
	}
	void cloneData( ChImageChar* img ){
		if( img->getChannel() != getChannel() || img->getByteSize() > getByteSize() )return;
		memcpy( getOrigin(), img->getOrigin(), img->getByteSize() );
	}
	void cloneData( IplImage* img ){
		if( img->nChannels != getChannel() || img->imageSize > getByteSize() )return;
		memcpy( getOrigin(), img->imageData, img->imageSize );
	}

	void convertUPtoDOWN(ChImageChar* img){
		for(int h=0; h<getHeight(); h++)
		{
			memcpy( this->getPixel(h), img->getPixel(getHeight()-h-1), getStep() );
		}
	}
	void convertUPtoDOWN(){
		ChImageChar tmpImg;	tmpImg.clone( this );
		convertUPtoDOWN( &tmpImg );
	}

	/*!
		@brief 指定された範囲を切り出す
		@param[in] img 元のイメージ
		@param[in] startHeight, startWidth 切り出し始めの高さ、幅（左上の位置）
		@param[in] endHeight, endWidth 切り出し終わりの高さ、幅（右下の位置）
	*/
	void trim( ChImageChar *img, int startHeight, int startWidth, int endHeight, int endWidth );

	/*!
		@brief 拡大・縮小する
		@param[in] img 元のイメージ
		@param[in] percent 縮尺・拡大率のパーセント
	*/
	void scale( ChImageChar *img, int percent );

	/*!
		@brief イメージimgを、自分に貼り付ける
		@param[in] img 貼り付けるイメージ
		@param[in] startHeight,startEnd 貼り付け始める高さ、幅（左上の位置）
	*/
	void paste( ChImageChar *img, int startHeight, int startWidth );

	uchar *getPixel(int height=0, int width=0){
		if(_isInROI(height, width)){ return (uchar*)my_Ipl->imageData + height*my_Ipl->widthStep + width*my_Ipl->nChannels; }
		return NULL;
	}
	void getPixel(int height, int width, uchar *out){
		if(_isInROI(height, width)){ memcpy(out, my_Ipl->imageData + height*my_Ipl->widthStep + width*my_Ipl->nChannels, sizeof(uchar)*my_Ipl->nChannels); }
	}
	void setPixel(int height, int width, uchar *in){
		if(_isInROI(height, width)){ memcpy(my_Ipl->imageData + height*my_Ipl->widthStep + width*my_Ipl->nChannels, in, sizeof(uchar)*my_Ipl->nChannels); }
	}
};

class ChImgC1 : public ChImageChar
{
public:
	void create(ChImageChar* img)		{ _createUchar(img->getHeight(), img->getWidth(), 1); }
	void create(int height, int width)	{ _createUchar(height, width, 1); }

	bool load(char* locFile);
	bool save(char* locFile);
	void setAll(uchar c);

	void convertFloatToUchar( ChImgF1 *imgF1, float low=0.0, float high=1.0 );
};


class ChImgC3 : public ChImageChar
{
public:
	void create(ChImageChar* img)			{ _createUchar(img->getHeight(), img->getWidth(), 3); }
	void create(int height, int width)		{ _createUchar(height, width, 3); }
	bool load(char* locFile);
	bool save(char* locFile);

	void convertChannel4to3( ChImgC4 *imgC4 );

	/*!
		@brief RGB系イメージから、HSV系イメージを作る
		@param[in] pImg RGB系イメージ
	*/
	void convertRGBtoHSV(ChImageChar* img);

	/*!
		@brief HSV系イメージから、RGB系イメージを作る
		@param[in] pImg HSV系イメージ
	*/
	void convertHSVtoRGB(ChImageChar* img);

	/*!
		@brief RGB系イメージから、HLS系イメージを作る
		@param[in] pImg RGB系イメージ
	*/
	void convertRGBtoHLS(ChImageChar* img);

	/*!
		@brief HLS系イメージから、RGB系イメージを作る
		@param[in] pImg HLS系イメージ
	*/
	void convertHLStoRGB(ChImageChar* img);
	
	/*!
		@brief イメージのRGBの順番を、逆に、BGRにする
		@param[in] pImg イメージ
	*/
	void convertRGBtoBGR(ChImageChar* img);
	void convertRGBtoBGR(){
		convertRGBtoBGR( this );
	}

	/*!
		@brief すべてのピクセルにrgbの値を代入する
		@param[in] rgb 赤緑青の値
	*/
	void setAll(uchar rgb[3]);

	/*!
		@brief メディアンフィルタを実行する
		@param[in] pImg イメージ
	*/
	void filterMedian(ChImageChar* pImg);
	void filterMedian(){
		ChImgC3 tmpImg; tmpImg.clone( this );
		filterMedian( &tmpImg );
	}
};

class ChImgC4 : public ChImageChar
{
public:
	void create(ChImageChar* img)			{ _createUchar(img->getHeight(), img->getWidth(), 4); }
	void create(int height, int width)		{ _createUchar(height, width, 4); }
	bool load(char* locFile);
	bool save(char* locFile);

	void convertChannel3to4( ChImgC3 *imgC3 );
	void convertRGBAtoBGRA( ChImgC4* img );
	void convertRGBAtoBGRA();

	/*!
		@brief 透過色を決める
		@param[in] r,g,b 透過色の赤緑青
	*/
	void setTransparent( uchar r, uchar g, uchar b );
	void setAll(uchar rgba[4]);
};

class ChImgArrC3 : public ChImageChar
{
private:
	int my_Count;
	int my_OneHeight;

	int getHeight()						{	return ChImageChar::getHeight();	}

public:
	void create(int cnt, int height, int width){
		my_Count = cnt;
		my_OneHeight = height;
		_createUchar(height*cnt, width, 3);
	}
	int getOneHeight()					{	return my_OneHeight;	}
	int getAllHeight()					{	return my_Ipl->height;	}
	bool load(int num, char* locFile);

	void setAll(uchar rgb[3]);

	bool setArrPixel(int num, int height, int width, unsigned char *in){ if(_isInROI(num*my_OneHeight+height, width)){ memcpy(getPixel(num*my_OneHeight+height, width),in,3*sizeof(uchar)); return true; } return false; }
	bool getArrPixel(int num, int height, int width, unsigned char *out){ if(_isInROI(num*my_OneHeight+height, width)){ memcpy(out,getPixel(num*my_OneHeight+height, width),3*sizeof(uchar)); return true; } return false; }
	uchar *getArrPixel(int num, int height=0, int width=0){
		int arrHeight = num*my_OneHeight+height;
		if(_isInROI(arrHeight, width)){ return getPixel(arrHeight, width); } return NULL;
	}
};

class ChImgF1 : public ChImageInterface
{
protected:
	void _createFloat( int height, int width, int channel ){
		my_CvSize.height = height; my_CvSize.width = width;
		my_Ipl = cvCreateImage(my_CvSize, IPL_DEPTH_32F, channel);
	}

public:
	void create(int height, int width)					{ _createFloat( height, width, 1 ); }
	void clone( ChImgF1 *img );
	void load(char* locFile, float low=0.0, float high=1.0);
	void save(char* locFile, float low=0.0, float high=1.0);

	inline float* getOrigin()			{ return (float*)my_Ipl->imageData; }		

	void setAll( float in );
	void setAllZero(){
		cvSetZero( getIplImage() );
	}
	void setPixel( int height, int width, float in )	{ memcpy( my_Ipl->imageData + height*my_Ipl->widthStep +width*sizeof(float), &in, sizeof(float) );	}
	float* getPixel( int height=0, int width=0 )		{ return (float*)(my_Ipl->imageData + height*my_Ipl->widthStep + width*sizeof(float)); }

	void convertUcharToFloat( ChImgC1 *imgC1, float low=0.0, float high=1.0 );
	void convertUPtoDOWN( ChImgF1* img ){
		for(int h=0; h<getHeight(); h++)
		{
			memcpy( this->getPixel(h), img->getPixel(getHeight()-h-1), getStep() );
		}
	}
	void convertUPtoDOWN(){
		ChImgF1 tmpImg;	tmpImg.clone( this );
		convertUPtoDOWN( &tmpImg );
	}
};
