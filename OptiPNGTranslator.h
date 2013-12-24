/*
 * Copyright 2013, Luke, noryb009@gmail.com. All rights reserved.
 * Based on TesseractTranslator by
 *    Gerasim Troeglazov, 3dEyes@gmail.com, Copyright 2013
 * Distributed under the terms of the MIT License.
 */

#ifndef OPTIPNG_TRANSLATOR_H
#define OPTIPNG_TRANSLATOR_H


#include "BaseTranslator.h"

#include <Translator.h>
#include <TranslatorFormats.h>
#include <TranslationDefs.h>
#include <GraphicsDefs.h>
#include <InterfaceDefs.h>
#include <DataIO.h>
#include <File.h>
#include <ByteOrder.h>
#include <OS.h>
#include <Notification.h>
#include <Resources.h>
#include <Roster.h>
#include <File.h>
#include <Bitmap.h>
#include <IconUtils.h>
#include <TranslationUtils.h>

#define DOCUMENT_COUNT "/documentCount"
#define DOCUMENT_INDEX "/documentIndex"

#define OPTIPNG_TRANSLATOR_VERSION B_TRANSLATION_MAKE_VERSION(0, 0, 1)
#define OPTIPNG_PNG_FORMAT	'OPNG'

#define OPTIPNG_SETTING_OPTIMIZATION_LEVEL		"optimization"
#define OPTIPNG_SETTING_BIT_DEPTH_REDUCTION		"bitdepth"
#define OPTIPNG_SETTING_COLOR_TYPE_REDUCTION	"colortype"
#define OPTIPNG_SETTING_PALETTE_REDUCTION		"palette"

#define BITS_IN_QUALITY		1
#define BITS_IN_CAPABILITY	1

#define OPTIPNG_OUT_QUALITY	0.8
#define OPTIPNG_OUT_CAPABILITY	0.6

class OptiPNGTranslator : public BaseTranslator {
	public:
		OptiPNGTranslator();

		virtual status_t DerivedIdentify(BPositionIO *inSource,
			const translation_format *inFormat, BMessage *ioExtension,
			translator_info *outInfo, uint32 outType);

		virtual status_t DerivedTranslate(BPositionIO *inSource,
			const translator_info *inInfo, BMessage *ioExtension,
			uint32 outType, BPositionIO *outDestination, int32 baseType);

		virtual status_t DerivedCanHandleImageSize(float width,
			float height) const;

		virtual BView *NewConfigView(TranslatorSettings *settings);	

	protected:
		virtual ~OptiPNGTranslator();
};

// Extensions that ShowImage supports
extern const char *kDocumentCount;
extern const char *kDocumentIndex;

#endif	/* OPTIPNG_TRANSLATOR_H */
