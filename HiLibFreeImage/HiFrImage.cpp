#include "StdAfx.h"
#include ".\HiFrImage.h"

///// ///// interface ///// /////
void ChImageChar::trim( ChImageChar *img, int startHeight, int startWidth, int endHeight, int endWidth )
{
	destroy();
	int height = endHeight - startHeight;
	int width = endWidth - startWidth;
	int channel = img->getChannel();
	if( height < 1 || width < 1 )return;
	_createUchar( height, width, channel );

	for( int h=0; h<height; h++ )
	{
		memcpy( getPixel( h ), img->getPixel( startHeight +h, startWidth ), width * channel * sizeof(uchar) ); 
	}
}

void ChImageChar::scale( ChImageChar *img, int percent )
{
	destroy();
	int height = img->getHeight() * percent / 100;
	int width = img->getWidth() * percent / 100;
	int channel = img->getChannel();
	if( height < 1 || width < 1 )return;
	_createUchar( height, width, channel );

	cvResize( img->getIplImage(), getIplImage(), CV_INTER_LINEAR );
}

void ChImageChar::paste( ChImageChar *img, int startHeight, int startWidth )
{
	if( my_Ipl == NULL )return;

	int channel = img->getChannel();
	int srcStartHeight = startHeight, srcStartWidth = startWidth;
	int dstStartHeight = 0, dstStartWidth = 0;
	int rangeHeight = img->getHeight(), rangeWidth = img->getWidth();

	if( srcStartHeight < 0 )
	{
		srcStartHeight = 0;
		rangeHeight += startHeight;
		dstStartHeight = -startHeight;
	}
	if( srcStartWidth < 0 )
	{
		srcStartWidth = 0;
		rangeWidth += startWidth;
		dstStartWidth = -startWidth;
	}
	if( srcStartHeight + rangeHeight > this->getHeight() )
	{
		rangeHeight = this->getHeight() - srcStartHeight;
	}
	if( srcStartWidth + rangeWidth > this->getWidth() )
	{
		rangeWidth = this->getWidth() - srcStartWidth;
	}

	for( int h=0; h<rangeHeight; h++ )
	{
		memcpy( getPixel( srcStartHeight +h, srcStartWidth ), 
			img->getPixel( dstStartHeight +h, dstStartWidth ),
			rangeWidth * channel * sizeof(uchar) );
	}
}

///// ///// channel 1 ///// /////
bool ChImgC1::load(char* locFile)
{
	destroy();

	if((my_Ipl = cvLoadImage(locFile)) == NULL)return false;
	my_CvSize.height = my_Ipl->height; my_CvSize.width = my_Ipl->width;
	if( getChannel() != 1 )return false;
	return true;
}

bool ChImgC1::save(char* locFile)
{
	cvSaveImage(locFile, my_Ipl);
	return true;
}

void ChImgC1::setAll(uchar c)
{
	memset( getOrigin(), c, getByteSize() );
}

void ChImgC1::convertFloatToUchar( ChImgF1 *imgF1, float low, float high )
{
	unsigned char *c;
	float *f, range;
	range = high - low;
	for( int h=0; h<getHeight(); h++ )
	{
		c = getPixel( h );
		f = imgF1->getPixel( h );
		for( int w=0; w<getWidth(); w++ )
		{
			if( high <= *f ){
				*c = 255;
			}else if( low >= *f ){
				*c = 0;
			}else{
				*c = (unsigned char)( (*f -low) * 255.0 / range );
			}
			c++;
			f++;
		}
	}
}

///// ///// channel 3 ///// /////
bool ChImgC3::load(char* locFile)
{
	destroy();

	if((my_Ipl = cvLoadImage(locFile)) == NULL)return false;
	my_CvSize.height = my_Ipl->height; my_CvSize.width = my_Ipl->width;
	if( getChannel() != 3 )return false;
	return true;
}

bool ChImgC3::save(char* locFile)
{
	cvSaveImage(locFile, my_Ipl);
	return true;
}

void ChImgC3::convertChannel4to3( ChImgC4 *imgC4 )
{
	uchar *c3, *c4;
	for(int h=0; h<getHeight(); h++)
	{
		c3 = this->getPixel( h );
		c4 = imgC4->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			memcpy( c3, c4, 3*sizeof(uchar) );
			c4 += 4;	c3 += 3;
		}
	}
}

void ChImgC3::convertRGBtoHSV(ChImageChar* img)
{
	cvCvtColor( img->getIplImage(), getIplImage(), CV_RGB2HSV );
}

void ChImgC3::convertHSVtoRGB(ChImageChar* img)
{
	cvCvtColor( img->getIplImage(), getIplImage(), CV_HSV2RGB );
}

void ChImgC3::convertRGBtoHLS(ChImageChar* img)
{
	cvCvtColor( img->getIplImage(), getIplImage(), CV_RGB2HLS );
}

void ChImgC3::convertHLStoRGB(ChImageChar* img)
{
	cvCvtColor( img->getIplImage(), getIplImage(), CV_HLS2RGB );
}

void ChImgC3::convertRGBtoBGR(ChImageChar* img)
{
	//uchar *c, tmp;
	//for(int h=0; h<getHeight(); h++)
	//{
	//	c = this->getPixel( h );
	//	for(int w=0; w<getWidth(); w++)
	//	{
	//		tmp=c[0]; c[0]=c[2]; c[2]=tmp;
	//		c += 3;
	//	}
	//}

	cvConvertImage( img->getIplImage(), getIplImage(), CV_CVTIMG_SWAP_RB );
}

void ChImgC3::setAll(uchar rgb[3])
{
	uchar *c;
	for(int h=0; h<getHeight(); h++)
	{
		c = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			memcpy( c, rgb, 3*sizeof(uchar) );
			c += 3;
		}
	}
}

void ChImgC3::filterMedian(ChImageChar* img)
{
	cvSmooth( img->getIplImage(), getIplImage(), CV_MEDIAN );
}

///// ///// channel 4 ///// /////
bool ChImgC4::load(char* locFile)
{
	ChImgC3 imgC3;
	if( imgC3.load(locFile) == false )return false;
	this->create( &imgC3 );
	this->convertChannel3to4( &imgC3 );
	return true;
}

bool ChImgC4::save(char* locFile)
{
	ChImgC3 imgC3; imgC3.create( this );
	imgC3.convertChannel4to3( this );
	return imgC3.save(locFile);
}

void ChImgC4::convertChannel3to4( ChImgC3 *imgC3 )
{
	uchar *c3, *c4;
	for(int h=0; h<getHeight(); h++)
	{
		c3 = imgC3->getPixel( h );
		c4 = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			memcpy( c4, c3, 3*sizeof(uchar) );
			c4[3] = 255;
			c4 += 4;	c3 += 3;
		}
	}
}

void ChImgC4::convertRGBAtoBGRA()
{
	uchar *ct, tmp;
	for(int h=0; h<getHeight(); h++)
	{
		ct = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			tmp = ct[0];
			ct[0] = ct[2];
			ct[2] = tmp;
			ct += 4;
		}
	}
}

void ChImgC4::convertRGBAtoBGRA( ChImgC4* img )
{
	uchar *ci, *ct;
	for(int h=0; h<getHeight(); h++)
	{
		ci = img->getPixel( h );
		ct = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			ct[0] = ci[2];
			ct[1] = ci[1];
			ct[2] = ci[0];
			ct[3] = ci[3];
			ci += 4;
			ct += 4;
		}
	}
}

void ChImgC4::setTransparent( uchar r, uchar g, uchar b )
{
	uchar *c;
	for(int h=0; h<getHeight(); h++)
	{
		c = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			if( c[0]==r && c[1]==g && c[2]==b ){	c[3]=0;	}else{	c[3]=255;	}
			c += 4;
		}
	}
}

void ChImgC4::setAll(uchar rgba[4])
{
	uchar *c;
	for(int h=0; h<getHeight(); h++)
	{
		c = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			memcpy( c, rgba, 4*sizeof(uchar) );
			c += 4;
		}
	}
}

///// ///// image array ///// /////
bool ChImgArrC3::load(int num, char* locFile)
{
	ChImgC3 imgC3;
	if( imgC3.load(locFile) == false )return false;
	memcpy(getArrPixel(num), imgC3.getOrigin(), imgC3.getByteSize());
	return true;
}

void ChImgArrC3::setAll(uchar rgb[3])
{
	uchar *c;
	for(int h=0; h<getHeight(); h++)
	{
		c = this->getPixel( h );
		for(int w=0; w<getWidth(); w++)
		{
			memcpy( c, rgb, 3*sizeof(uchar) );
			c += 3;
		}
	}
}

void ChImgF1::clone( ChImgF1 *img )
{
	_createFloat( img->getHeight(), img->getWidth(), 1 );
	memcpy( getOrigin(), img->getOrigin(), img->getByteSize() );
}

void ChImgF1::load(char* locFile, float low, float high)
{
	ChImgC1 imgC1;
	imgC1.load( locFile );
	create( imgC1.getHeight(), imgC1.getWidth() );
	convertUcharToFloat( &imgC1, low, high );
}

void ChImgF1::save(char* locFile, float low, float high)
{
	ChImgC1 imgC1;
	imgC1.create( getHeight(), getWidth() );
	imgC1.convertFloatToUchar( this, low, high );
	imgC1.save( locFile );
}

void ChImgF1::convertUcharToFloat( ChImgC1 *imgC1, float low, float high )
{
	unsigned char *c;
	float *f, range;
	range = high - low;
	for( int h=0; h<getHeight(); h++ )
	{
		f = getPixel( h );
		c = imgC1->getPixel( h );
		for( int w=0; w<getWidth(); w++ )
		{
			*f = range * (*c) / 255 + low;
			c++;
			f++;
		}
	}
}
void ChImgF1::setAll( float in )
{
	cvSet( getIplImage(), cvScalar( in ) );

	//float *f;
	//for( int h=0; h<getHeight(); h++ )
	//{
	//	f = getPixel( h );
	//	for( int w=0; w<getWidth(); w++ )
	//	{
	//		*f = in;
	//		f++;
	//	}
	//}
}
