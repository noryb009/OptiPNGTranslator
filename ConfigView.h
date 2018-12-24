/*
 * Copyright 2013, Luke, noryb009@gmail.com. All rights reserved.
 * Based on TesseractTranslator by
 *    Gerasim Troeglazov, 3dEyes@gmail.com, Copyright 2013
 * Distributed under the terms of the MIT License.
 */

#ifndef CONFIG_VIEW_H
#define CONFIG_VIEW_H

#include "TranslatorSettings.h"

#include <View.h>
#include <TextView.h>
#include <String.h>
#include <GroupView.h>
#include <Slider.h>
#include <CheckBox.h>

class ConfigView : public BGroupView {
	public:
		ConfigView(TranslatorSettings *settings);
		virtual ~ConfigView();
	private:
		void 		MessageReceived(BMessage *message);
		void 		AllAttached();

		TranslatorSettings	*fSettings;
		BTextView			*fCopyrightView;
		BSlider				*fOptimizationLevel;
		BCheckBox			*fBitDepthCheckBox;
		BCheckBox			*fColorTypeCheckBox;
		BCheckBox			*fPaletteCheckBox;
};

#endif	// CONFIG_VIEW_H
