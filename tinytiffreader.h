/*
    Copyright (c) 2008-2015 Jan W. Krieger (<jan@jkrieger.de>, <j.krieger@dkfz.de>), German Cancer Research Center (DKFZ) & IWR, University of Heidelberg

    last modification: $LastChangedDate: 2015-07-07 12:07:58 +0200 (Di, 07 Jul 2015) $  (revision $Rev: 4005 $)

    This software is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License (LGPL) as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.


*/
/*
   Further modifications by John Robinson starting December 2019  Thos changes are also
   covered by the GNU General Public License.
*/

#ifndef TINYTIFFREADER_H
#define TINYTIFFREADER_H

#ifndef TINYTIFFREADER_LIB_EXPORT
#    define TINYTIFFREADER_LIB_EXPORT
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <string>

using namespace std;

/*! \defgroup tinytiffreader Tiny TIFF reader library
   \ingroup tools

   The methods in this file allow to read TIFF files with limited capabilites,
   but very fast (comapred to libtiff) and also more frames from a multi-frame
   TIFF than libtiff (which is currently limited to 65535 frames due to internal
   data sizes!).
   
   This library currently support TIFF files, which meet the following criteria:
     * TIFF-only (no BigTIFF), i.e. max. 4GB
	 * uncompressed frames
	 * one, or more samples per frame
	 * data types: UINT, INT, FLOAT
   .	 
   
   This example reads all frames from a TIFF file:
   \code
   TinyTIFFReaderFile* tiffr=NULL;
   tiffr=TinyTIFFReader_open(filename); 
    if (!tiffr) { 
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; 
    } else { 
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; 
        std::cout<<"    ImageDescription:\n"<< TinyTIFFReader_getImageDescription(tiffr) <<"\n"; 
        uint32_t frames=TinyTIFFReader_countFrames(tiffr); 
        std::cout<<"    frames: "<<frames<<"\n"; 
        if (TinyTIFFReader_wasError(tiffr)) std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; 
        uint32_t frame=0; 
        do { 
            uint32_t width=TinyTIFFReader_getWidth(tiffr); 
            uint32_t height=TinyTIFFReader_getHeight(tiffr); 
			bool ok=true;
            if (width>0 && height>0) std::cout<<"    size of frame "<<frame<<": "<<width<<"x"<<height<<"\n"; 
            else { std::cout<<"    ERROR IN FRAME "<<frame<<": size too small "<<width<<"x"<<height<<"\n"; ok=false; } 
            if (ok) { 
                frame++; 
                uint16_t* image=(uint16_t*)calloc(width*height, sizeof(uint16_t));  
                TinyTIFFReader_getSampleData(tiffr, image, 0); 
                if (TinyTIFFReader_wasError(tiffr)) { ok=false; std::cout<<"   ERROR:"<<TinyTIFFReader_getLastError(tiffr)<<"\n"; } 
				
                ///////////////////////////////////////////////////////////////////
				// HERE WE CAN DO SOMETHING WITH THE IMAGE IN image (ROW-MAJOR!)
                ///////////////////////////////////////////////////////////////////
				
                free(image); 
            } 
        } while (TinyTIFFReader_readNext(tiffr)); // iterate over all frames
        std::cout<<"    read "<<frame<<" frames\n"; 
    } 
    TinyTIFFReader_close(tiffr); 
   \endcode
   
   This example reads the first frame in a TIFF file:
   \code
   TinyTIFFReaderFile* tiffr=NULL;
   tiffr=TinyTIFFReader_open(filename); 
   if (!tiffr) { 
        std::cout<<"    ERROR reading (not existent, not accessible or no TIFF file)\n"; 
   } else { 
		uint32_t width=TinyTIFFReader_getWidth(tiffr); 
        uint32_t height=TinyTIFFReader_getHeight(tiffr); 
		uint16_t* image=(uint16_t*)calloc(width*height, sizeof(uint16_t));  
        TinyTIFFReader_getSampleData(tiffr, image, 0); 
				
                ///////////////////////////////////////////////////////////////////
				// HERE WE CAN DO SOMETHING WITH THE IMAGE IN image (ROW-MAJOR!)
                ///////////////////////////////////////////////////////////////////
				
		free(image); 
	} 
    TinyTIFFReader_close(tiffr); 
   \endcode


 */

/** \brief struct used to describe a TIFF file
  * \ingroup tinytiffreader
  */
struct TinyTIFFReaderFile; // forward


/*! \brief open TIFF file for reading
    \ingroup TinyTIFFReader

    \param filename name of the new TIFF file
    \return a new TinyTIFFReaderFile pointer on success, or NULL on errors, Note that you can not use TinyTIFFReader_getLastError() if
            \c NULL is returned! In this case the C method open() retrned an error, so the fiel didn't exist, or you do not have
            permission to read it. Also if no TIFF file was detected (first twi bytes were neither 'II' nor 'MM') \c NULL is returned.

  */
TINYTIFFREADER_LIB_EXPORT TinyTIFFReaderFile* TinyTIFFReader_open(const char* filename);


/*! \brief close a given TIFF file
    \ingroup TinyTIFFReader

    \param tiff TIFF file to close

    This function also releases memory allocated in TinyTIFFReader_open() in \a tiff.
 */
TINYTIFFREADER_LIB_EXPORT void TinyTIFFReader_close(TinyTIFFReaderFile* tiff);

/*! \brief returns a pointer to the last error message
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

    \note the pointer is accessible as long as the TIFF file has not been closed using TinyTIFFReader_close()
 */
TINYTIFFREADER_LIB_EXPORT const char* TinyTIFFReader_getLastError(TinyTIFFReaderFile* tiff);

/*! \brief returns TRUE (non-zero) when there was an error in the last function call, or FALSE (zero) if there was no error
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT int TinyTIFFReader_wasError(TinyTIFFReaderFile* tiff);

/*! \brief returns TRUE (non-zero) when there was no error in the last function call, or FALSE (zero) if there was an error
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT int TinyTIFFReader_success(TinyTIFFReaderFile* tiff);

/*! \brief returns TRUE (non-zero) if another frame exists in the TIFF file
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT int TinyTIFFReader_hasNext(TinyTIFFReaderFile* tiff);

/*! \brief reads the next frame from a multi-frame TIFF
    \ingroup TinyTIFFReader

    \param tiff TIFF file 
    \return TRUE (non-zero) if another frame exists in the TIFF file

 */
TINYTIFFREADER_LIB_EXPORT int TinyTIFFReader_readNext(TinyTIFFReaderFile* tiff);


/*! \brief return the width of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT uint32_t TinyTIFFReader_getWidth(TinyTIFFReaderFile* tiff);

/*! \brief return the height of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT uint32_t TinyTIFFReader_getHeight(TinyTIFFReaderFile* tiff);

/*! \brief return the image description of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT std::string TinyTIFFReader_getImageDescription(TinyTIFFReaderFile* tiff);


#define TINYTIFFREADER_SAMPLEFORMAT_UINT 1
#define TINYTIFFREADER_SAMPLEFORMAT_INT 2
#define TINYTIFFREADER_SAMPLEFORMAT_FLOAT 3
#define TINYTIFFREADER_SAMPLEFORMAT_UNDEFINED 4

/*! \brief return the sample format of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT uint16_t TinyTIFFReader_getSampleFormat(TinyTIFFReaderFile* tiff);

/*! \brief return the bits per sample of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 
	\param sample return bits for the given sample number [default: 0]

 */
TINYTIFFREADER_LIB_EXPORT uint16_t TinyTIFFReader_getBitsPerSample(TinyTIFFReaderFile* tiff, int sample=0);
/*! \brief return the samples per pixel of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT uint16_t TinyTIFFReader_getSamplesPerPixel(TinyTIFFReaderFile* tiff);

/*! \brief read the given sample from the current frame into the given buffer,
           the byteorder is transformed to the byteorder of the system!
    \ingroup TinyTIFFReader

    \param tiff TIFF file 
    \return \c TRUE (non-zero) on success

    \note The user is responsible for providing the correct buffer size
          (taking width, height and bitsPerSample into account).

 */
TINYTIFFREADER_LIB_EXPORT double TinyTIFFReader_getFocalLength(TinyTIFFReaderFile* tiff);
/*! \brief return the loval length of the image
 
    \ingroup TinyTIFFReader

    \param tiff TIFF file
    \return \c non-zero on success
*/


TINYTIFFREADER_LIB_EXPORT int TinyTIFFReader_getSampleData(TinyTIFFReaderFile* tiff, void* buffer, uint16_t sample);



/*! \brief return the width of the current frame
    \ingroup TinyTIFFReader

    \param tiff TIFF file 

 */
TINYTIFFREADER_LIB_EXPORT uint32_t TinyTIFFReader_countFrames(TinyTIFFReaderFile* tiff);

/*! \brief template function that internally calls TinyTIFFReader_getSampleData() and copies the data into the specified output buffer
    \ingroup TinyTIFFReader

    \tparam Tin datatype of the sample in the TIFF file
    \tparam Tout datatype of \a buffer
    \param tif the TIFF file to read from
    \param buffer the buffer to write into

    This function may be used in reader code like this:
\code
bool intReadFrameFloat(float *data) {
    if (!tif) return false;

    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    if (!(wwidth>0 && hheight>0)) tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error in file '%1': frame %2 is too small\n").arg(filename).arg(frame));
    else {
        uint16_t sformat=TinyTIFFReader_getSampleFormat(tif);
        uint16_t bits=TinyTIFFReader_getBitsPerSample(tif);

        if (sformat==TINYTIFFREADER_SAMPLEFORMAT_UINT) {
            if (bits==8) TinyTIFFReader_readFrame<uint8_t, float>(tif, data);
            else if (bits==16) TinyTIFFReader_readFrame<uint16_t, float>(tif, data);
            else if (bits==32) TinyTIFFReader_readFrame<uint32_t, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_INT) {
            if (bits==8) TinyTIFFReader_readFrame<int8_t, float>(tif, data);
            else if (bits==16) TinyTIFFReader_readFrame<int16_t, float>(tif, data);
            else if (bits==32) TinyTIFFReader_readFrame<int32_t, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else if (sformat==TINYTIFFREADER_SAMPLEFORMAT_FLOAT) {
            if (bits==32) TinyTIFFReader_readFrame<float, float>(tif, data);
            else {
                tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
                return false;
            }
        } else {
            tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("frame %1 has a datatype not convertible to float (type=%2, bitspersample=%3)\n").arg(frame).arg(sformat).arg(bits));
            return false;
        }
    }
    if (TinyTIFFReader_wasError(tif)) {
        tinyTIFFErrorHandler("QFImageReaderTinyTIFF", QObject::tr("error reading frame %1: %2\n").arg(frame).arg(TinyTIFFReader_getLastError(tif)));
        return false;
    }

    return true;
}
\endcode
 */
template <class Tin, class Tout>
inline void TinyTIFFReader_readFrame(TinyTIFFReaderFile* tif, Tout* buffer) {
    uint32_t wwidth=TinyTIFFReader_getWidth(tif);
    uint32_t hheight=TinyTIFFReader_getHeight(tif);
    Tin* tmp=(Tin*)calloc(wwidth*hheight, sizeof(Tin));
    TinyTIFFReader_getSampleData(tif, tmp, 0);
    for (uint32_t i=0; i<wwidth*hheight; i++) {
        buffer[i]=tmp[i];
    }
    free(tmp);
}


#define EXIF_ExifVersion 36864 //UNDEFINED
#define EXIF_FlashpixVersion 40960 //UNDEFINED
#define EXIF_ColorSpace 40961 //SHORT
#define EXIF_Gamma 42240 //RATIONAL
#define EXIF_ComponentsConfiguration 37121 //UNDEFINED
#define EXIF_CompressedBitsPerPixel 37122 //RATIONAL
#define EXIF_PixelXDimension 40962 //SHORT or LONG
#define EXIF_PixelYDimension 40963 //SHORT or LONG
#define EXIF_MakerNote 37500 //UNDEFINED
#define EXIF_UserComment 37510 //UNDEFINED
#define EXIF_RelatedSoundFile 40964 //ASCII
#define EXIF_DateTimeOriginal 36867 //ASCII
#define EXIF_DateTimeDigitized 36868 //ASCII
#define EXIF_SubSecTime 37520 //ASCII
#define EXIF_SubSecTimeOriginal 37521 //ASCII
#define EXIF_SubSecTimeDigitized 37522 //ASCII
#define EXIF_ImageUniqueID 42016 //ASCII
#define EXIF_CameraOwnerName 42032 //ASCII
#define EXIF_BodySerialNumber 42033 //ASCII
#define EXIF_LensSpecification 42034 //RATIONAL
#define EXIF_LensMake 42035 //ASCII
#define EXIF_LensModel 42036 //ASCII
#define EXIF_LensSerialNumber 42037 //ASCII
//********************************
#define EXIF_ExposureTime 33434 //829A RATIONAL
#define EXIF_FNumber 33437 //829D RATIONAL
#define EXIF_ExposureProgram 34850 //8822 SHORT
#define EXIF_SpectralSensitivity 34852 //8824 ASCII
#define EXIF_PhotographicSensitivity 34855 //8827 SHORT
#define EXIF_OECF 34856 //8828 UNDEFINED
#define EXIF_SensitivityType 34864 //8830 SHORT
#define EXIF_StandardOutputSensitivity 34865 //8831 LONG
#define EXIF_RecommendedExposureIndex 34866 //8832 LONG
#define EXIF_ISOSpeed 34867 //8833 LONG
#define EXIF_ISOSpeedLatitudeyyy 34868 //8834 LONG
#define EXIF_ISOSpeedLatitudezzz 34869 //8835 LONG
#define EXIF_ShutterSpeedValue 37377 //9201 SRATIONAL
#define EXIF_ApertureValue 37378 //9202 RATIONAL
#define EXIF_BrightnessValue 37379 //9203 SRATIONAL
#define EXIF_ExposureBiasValue 37380 //9204 SRATIONAL
#define EXIF_MaxApertureValue 37381 //9205 RATIONAL
#define EXIF_SubjectDistance 37382 //9206 RATIONAL
#define EXIF_MeteringMode 37383 //9207 SHORT
#define EXIF_LightSource 37384 //9208 SHORT
#define EXIF_Flash 37385 //9209 SHORT
#define EXIF_FocalLength 37386 //920A RATIONAL
#define EXIF_SubjectArea 37396 //9214 SHORT
#define EXIF_FlashEnergy 41483 //A20B RATIONAL
#define EXIF_SpatialFrequencyResponse 41484 //A20C UNDEFINED
#define EXIF_FocalPlaneXResolution 41486 //A20E RATIONAL
#define EXIF_FocalPlaneYResolution 41487 //A20F RATIONAL
#define EXIF_FocalPlaneResolutionUnit 41488 //A210 SHORT
#define EXIF_SubjectLocation 41492 //A214 SHORT
#define EXIF_ExposureIndex 41493 //A215 RATIONAL
#define EXIF_SensingMethod 41495 //A217 SHORT
#define EXIF_FileSource 41728 //A300 UNDEFINED
#define EXIF_SceneType 41729 //A301 UNDEFINED
#define EXIF_CFAPattern 41730 //A302 UNDEFINED
#define EXIF_CustomRendered 41985 //A401 SHORT
#define EXIF_ExposureMode 41986 //A402 SHORT
#define EXIF_WhiteBalance 41987 //A403 SHORT
#define EXIF_DigitalZoomRatio 41988 //A404 RATIONAL
#define EXIF_FocalLengthIn35mmFilm 41989 //A405 SHORT
#define EXIF_SceneCaptureType 41990 //A406 SHORT
#define EXIF_GainControl 41991 //A407 RATIONAL
#define EXIF_Contrast 41992 //A408 SHORT
#define EXIF_Saturation 41993 //A409 SHORT
#define EXIF_Sharpness 41994 //A40A SHORT
#define EXIF_DeviceSettingDescription 41995 //A40B UNDEFINED
#define EXIF_SubjectDistanceRange 41996 //A40C SHORT

TINYTIFFREADER_LIB_EXPORT bool TinyTIFFReaderEXIF(string &retval, TinyTIFFReaderFile *tiff, uint16_t tag);
/*! \brief get the value of the EXIF tag from the currently opened file.  Value for the submitted tag must be of type string
 
    \ingroup TinyTIFFReader

    \param retval referece to where EXIF tag string is to be placed
    \param tiff TIFF file
    \param tag  EXIF tag
    \return \c true if success false if tag not found or of wrong type
*/
TINYTIFFREADER_LIB_EXPORT bool TinyTIFFReaderEXIF(uint32_t &retval, TinyTIFFReaderFile *tiff, uint16_t tag);
/*! \brief get the value of the EXIF tag from the currently opened file.  Value for the submitted tag must be of type unsigned int or undefined
 
    \ingroup TinyTIFFReader

    \param retval referece to where EXIF tag unsigned value is to be placed
    \param tiff TIFF file
    \param tag  EXIF tag
    \return \c true if success false if tag not found or of wrong type
*/
TINYTIFFREADER_LIB_EXPORT bool TinyTIFFReaderEXIF(int32_t &retval, TinyTIFFReaderFile *tiff, uint16_t tag);
/*! \brief get the value of the EXIF tag from the currently opened file.  Value for the submitted tag must be of type signed int
 
    \ingroup TinyTIFFReader

    \param retval referece to where EXIF tag signed value is to be placed
    \param tiff TIFF file
    \param tag  EXIF tag
    \return \c true if success false if tag not found or of wrong type
*/
TINYTIFFREADER_LIB_EXPORT bool TinyTIFFReaderEXIF(double &retval, TinyTIFFReaderFile *tiff, uint16_t tag);
/*! \brief get the value of the EXIF tag from the currently opened file.  Value for the submitted tag must be of type rational or srational
 
    \ingroup TinyTIFFReader

    \param retval referece to where EXIF tag signed value is to be placed
    \param tiff TIFF file
    \param tag  EXIF tag
    \return \c true if success false if tag not found or of wrong type
*/


#endif // TINYTIFFREADER_H
