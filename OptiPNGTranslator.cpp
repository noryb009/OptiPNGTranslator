/*
 * Copyright 2013, Luke, noryb009@gmail.com. All rights reserved.
 * Based on TesseractTranslator by
 *    Gerasim Troeglazov, 3dEyes@gmail.com, Copyright 2013
 * Distributed under the terms of the MIT License.
 */

#include "OptiPNGTranslator.h"

#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <Catalog.h>
#include <FindDirectory.h>
#include <TranslatorRoster.h>

#include "ConfigView.h"

const char *kDocumentCount = "/documentCount";
const char *kDocumentIndex = "/documentIndex";

#define kPngMimeType "image/png"
#define kPngName "Optimized PNG images"

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "OptimizedPNGTranslator"

// The input formats that this translator supports.
static const translation_format sInputFormats[] = {
	{
		B_TRANSLATOR_BITMAP,
		B_TRANSLATOR_BITMAP,
		BITS_IN_QUALITY,
		BITS_IN_CAPABILITY,
		"image/x-be-bitmap",
		"Be Bitmap Format (PNGTranslator)"
	},
};

// The output formats that this translator supports.
static const translation_format sOutputFormats[] = {
	{
		OPTIPNG_PNG_FORMAT,
		B_TRANSLATOR_BITMAP,
		OPTIPNG_OUT_QUALITY,
		OPTIPNG_OUT_CAPABILITY,
		kPngMimeType,
		kPngName
	},
};

// Default settings for the Translator
static const TranSetting sDefaultSettings[] = {
	{OPTIPNG_SETTING_OPTIMIZATION_LEVEL, TRAN_SETTING_INT32, 2},
	{OPTIPNG_SETTING_BIT_DEPTH_REDUCTION, TRAN_SETTING_BOOL, true},
	{OPTIPNG_SETTING_COLOR_TYPE_REDUCTION, TRAN_SETTING_BOOL, true},
	{OPTIPNG_SETTING_PALETTE_REDUCTION, TRAN_SETTING_BOOL, true},
};

const uint32 kNumInputFormats = sizeof(sInputFormats)
	/ sizeof(translation_format);
const uint32 kNumOutputFormats = sizeof(sOutputFormats)
	/ sizeof(translation_format);
const uint32 kNumDefaultSettings = sizeof(sDefaultSettings)
	/ sizeof(TranSetting);

OptiPNGTranslator::OptiPNGTranslator()
	: BaseTranslator(B_TRANSLATE("Optimized PNG images"), 
		"OptiPNG image translator",
		OPTIPNG_TRANSLATOR_VERSION,
		sInputFormats, kNumInputFormats,
		sOutputFormats, kNumOutputFormats,
		"OptiPNGTranslator",
		sDefaultSettings, kNumDefaultSettings,
		B_TRANSLATOR_BITMAP, OPTIPNG_PNG_FORMAT)
{
}


OptiPNGTranslator::~OptiPNGTranslator()
{
}


status_t
OptiPNGTranslator::DerivedIdentify(BPositionIO *source,
	const translation_format *format, BMessage *ioExtension,
	translator_info *info, uint32 outType)
{
	if (outType != OPTIPNG_PNG_FORMAT)
		return B_NO_TRANSLATOR;

	BBitmap *originalbmp;
	originalbmp = BTranslationUtils::GetBitmap(source);
	if(originalbmp == NULL) {
		return B_NO_TRANSLATOR;
	}
	
	info->type = sInputFormats[0].type;
	info->translator = 0;
	info->group = sInputFormats[0].group;
	info->quality = sInputFormats[0].quality;
	info->capability = sInputFormats[0].capability;
	strcpy(info->name, sInputFormats[0].name);
	strcpy(info->MIME, sInputFormats[0].MIME);
	
	delete originalbmp;
	
	return B_OK;
}


status_t
OptiPNGTranslator::DerivedTranslate(BPositionIO *source,
	const translator_info *info, BMessage *ioExtension,
	uint32 outType, BPositionIO *target, int32 baseType)
{
	if(baseType == 1 && outType == OPTIPNG_PNG_FORMAT) {
		// create temp file
		int tempFileFD;
		BPath tempDir;
		BString tempFilePath;
		if(find_directory(B_SYSTEM_TEMP_DIRECTORY, &tempDir) != B_OK )
			return B_ERROR;
		
		tempFilePath.Append(tempDir.Path())
			.Append("/OptiPNGTranslator.XXXXXX");
		
		tempFileFD = mkstemp(tempFilePath.LockBuffer(0));
		tempFilePath.UnlockBuffer();
		
		if(tempFileFD == -1)
			return B_ERROR;
		close(tempFileFD);
		
		BFile tempFile = BFile(tempFilePath, O_WRONLY);
		
		// write PNG to file
		off_t sourceSize;
		source->GetSize(&sourceSize);
		
		BTranslatorRoster *roster = BTranslatorRoster::Default();
		roster->Translate(source, NULL, NULL, &tempFile, (uint32)B_PNG_FORMAT);
		
		// optimize file
		BString optipng;
		if(system("optipng &> /dev/null") == 0) {
			optipng = "optipng";
		} else if(system("optipng-x86 &> /dev/null") == 0) {
			optipng = "optipng-x86";
		} else {
			return B_ERROR;
		}
		
		// optipng -clobber -out (file) (file)
		BString command;
		command = optipng;
		if(!fSettings->SetGetBool(OPTIPNG_SETTING_BIT_DEPTH_REDUCTION))
			command += " -nb"; // no bit-depth reduction
		if(!fSettings->SetGetBool(OPTIPNG_SETTING_COLOR_TYPE_REDUCTION))
			command += " -nc";
		if(!fSettings->SetGetBool(OPTIPNG_SETTING_PALETTE_REDUCTION))
			command += " -np";
		
		command.Append(" -o")
			.Append((char)(fSettings->
				SetGetInt32(OPTIPNG_SETTING_OPTIMIZATION_LEVEL)+'0'),1);
		// rest of command
		command.Append(" -clobber -out ")
			.Append(tempFilePath)
			.Append(" ")
			.Append(tempFilePath)
		;
		
		if(system(command) != 0) {
			return B_ERROR;
		}
		
		// read the file
		tempFile = BFile(tempFilePath, O_RDONLY);
		
		off_t fileSize;
		tempFile.GetSize(&fileSize);
		unsigned char *buffer;
		
		buffer = new unsigned char[fileSize];
		if(buffer == NULL)
			return B_ERROR;
		tempFile.ReadAt(0, buffer, fileSize);
		target->Write(buffer, fileSize);
		delete [] buffer;
		
		// delete the file
		BEntry entry = BEntry(tempFilePath);
		entry.Remove();
		
		return B_OK;
	}
	return B_NO_TRANSLATOR;
}


status_t
OptiPNGTranslator::DerivedCanHandleImageSize(float width, float height) const
{
	return B_OK;
}


BView *
OptiPNGTranslator::NewConfigView(TranslatorSettings *settings)
{
	return new ConfigView(settings);
}


//	#pragma mark -


BTranslator *
make_nth_translator(int32 n, image_id you, uint32 flags, ...)
{
	if (n != 0)
		return NULL;

	return new OptiPNGTranslator();
}

