/*
 * Copyright 2013, Luke, noryb009@gmail.com. All rights reserved.
 * Based on TesseractTranslator by
 *    Gerasim Troeglazov, 3dEyes@gmail.com, Copyright 2013
 * Distributed under the terms of the MIT License.
 */

#include "ConfigView.h"
#include "OptiPNGTranslator.h"

#include <Catalog.h>
#include <ControlLook.h>
#include <SpaceLayoutItem.h>
#include <StringView.h>

#include <stdio.h>

static const uint32 kMsgOptim		= 'opti';
static const uint32 kMsgBitDepth	= 'bitd';
static const uint32 kMsgColorType	= 'coty';
static const uint32 kMsgPaletteReduc= 'palr';

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "OptimizedPNGTranslator"

ConfigView::ConfigView(TranslatorSettings *settings)
	: BGroupView("OptiPNGTranslator Settings", B_VERTICAL, 0)
{
	fSettings = settings;
	
	BAlignment leftAlignment(B_ALIGN_LEFT, B_ALIGN_VERTICAL_UNSET);

	BStringView *stringView = new BStringView("title",
		B_TRANSLATE("OptiPNG image translator"));
	stringView->SetFont(be_bold_font);
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	float spacing = be_control_look->DefaultItemSpacing();
	AddChild(BSpaceLayoutItem::CreateVerticalStrut(spacing));

	char version[256];
	sprintf(version, B_TRANSLATE("Version %d.%d.%d, %s"),
		int(B_TRANSLATION_MAJOR_VERSION(OPTIPNG_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_MINOR_VERSION(OPTIPNG_TRANSLATOR_VERSION)),
		int(B_TRANSLATION_REVISION_VERSION(OPTIPNG_TRANSLATOR_VERSION)),
		__DATE__);
	stringView = new BStringView("version", version);
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	stringView = new BStringView("my_copyright",
		B_UTF8_COPYRIGHT "2013 Luke <noryb009@gmail.com>");
	stringView->SetExplicitAlignment(leftAlignment);
	AddChild(stringView);

	AddChild(BSpaceLayoutItem::CreateVerticalStrut(spacing));
	
	fOptimizationLevel = new BSlider(
		"optimization",
		B_TRANSLATE("Optimization level:"), new BMessage(kMsgOptim),
		0, 7, B_HORIZONTAL, B_BLOCK_THUMB);
	fOptimizationLevel->SetHashMarks(B_HASH_MARKS_BOTTOM);
	fOptimizationLevel->SetHashMarkCount(8);
	fOptimizationLevel->SetLimitLabels(B_TRANSLATE("Low"),
		B_TRANSLATE("High"));
	fOptimizationLevel->SetValue(
		fSettings->SetGetInt32(OPTIPNG_SETTING_OPTIMIZATION_LEVEL));
	AddChild(fOptimizationLevel);
	
	fBitDepthCheckBox = new BCheckBox((char*)OPTIPNG_SETTING_BIT_DEPTH_REDUCTION,
		B_TRANSLATE("Reduce bit depth"), new BMessage(kMsgBitDepth));
	if (fSettings->SetGetBool(OPTIPNG_SETTING_BIT_DEPTH_REDUCTION))
		fBitDepthCheckBox->SetValue(B_CONTROL_ON);
	AddChild(fBitDepthCheckBox);
	
	fColorTypeCheckBox = new BCheckBox((char*)OPTIPNG_SETTING_COLOR_TYPE_REDUCTION,
		B_TRANSLATE("Reduce color type"), new BMessage(kMsgColorType));
	if (fSettings->SetGetBool(OPTIPNG_SETTING_COLOR_TYPE_REDUCTION))
		fColorTypeCheckBox->SetValue(B_CONTROL_ON);
	AddChild(fColorTypeCheckBox);
	
	fPaletteCheckBox = new BCheckBox((char*)OPTIPNG_SETTING_PALETTE_REDUCTION,
		B_TRANSLATE("Reduce palette size"), new BMessage(kMsgPaletteReduc));
	if (fSettings->SetGetBool(OPTIPNG_SETTING_PALETTE_REDUCTION))
		fPaletteCheckBox->SetValue(B_CONTROL_ON);
	AddChild(fPaletteCheckBox);
	
	AddChild(BSpaceLayoutItem::CreateGlue());
	GroupLayout()->SetInsets(B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING, 
		B_USE_DEFAULT_SPACING, B_USE_DEFAULT_SPACING);

	SetExplicitPreferredSize(GroupLayout()->MinSize());		
}

ConfigView::~ConfigView()
{
	fSettings->Release();
}

void
ConfigView::MessageReceived(BMessage* message)
{
	struct {
		const char*		name;
		uint32			what;
		TranSettingType	type;
	} maps[] = {
		{ OPTIPNG_SETTING_OPTIMIZATION_LEVEL, kMsgOptim, TRAN_SETTING_INT32 },
		{ OPTIPNG_SETTING_BIT_DEPTH_REDUCTION, kMsgBitDepth, TRAN_SETTING_BOOL },
		{ OPTIPNG_SETTING_COLOR_TYPE_REDUCTION, kMsgColorType, TRAN_SETTING_BOOL },
		{ OPTIPNG_SETTING_PALETTE_REDUCTION, kMsgPaletteReduc, TRAN_SETTING_BOOL },
		{ NULL }
	};
	
	int i;
	for (i = 0; maps[i].name != NULL; i++) {
		if (maps[i].what == message->what)
			break;
	}

	if (maps[i].name == NULL) {
		BView::MessageReceived(message);
		return;
	}

	int32 value;
	if (message->FindInt32("value", &value) == B_OK
		|| message->FindInt32("be:value", &value) == B_OK) {
		switch(maps[i].type) {
			case TRAN_SETTING_BOOL:
			{
				bool boolValue = value;
				fSettings->SetGetBool(maps[i].name, &boolValue);
				break;
			}
			case TRAN_SETTING_INT32:
				fSettings->SetGetInt32(maps[i].name, &value);
				break;
		}
		fSettings->SaveSettings();
	}
}

void
ConfigView::AllAttached()
{
	fOptimizationLevel->SetTarget(this);
	fBitDepthCheckBox->SetTarget(this);
	fColorTypeCheckBox->SetTarget(this);
	fPaletteCheckBox->SetTarget(this);
}
