/*
 * Copyright 2013, Luke, noryb009@gmail.com. All rights reserved.
 * Based on TesseractTranslator by
 *    Gerasim Troeglazov, 3dEyes@gmail.com, Copyright 2013
 * Distributed under the terms of the MIT License.
 */

#include <Application.h>

#include "OptiPNGTranslator.h"
#include "TranslatorWindow.h"

int
main(int /*argc*/, char ** /*argv*/)
{
	BApplication app("application/x-vnd.Haiku-OptiPNGTranslator");

	status_t result;
	result = LaunchTranslatorWindow(new OptiPNGTranslator, 
		"OptiPNG Translator Info", BRect(0, 0, 320, 200));
	if (result != B_OK)
		return 1;

	app.Run();
	return 0;
}
