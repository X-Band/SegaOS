/*
	File:		RadioButtons.h

	Contains:	xxx put contents here xxx

	Written by:	Chris Yerga

	Copyright:	© 1994 by Catapult Entertainment, Inc., all rights reserved.

	Change History (most recent first):

		 <2>	 8/15/94	HEC		Added param to SetupRadioButton selector.
		 <1>	 7/14/94	CMY		first checked in

	To Do:
*/


#ifndef __RadioButtons__
#define __RadioButtons__


typedef void *RadioButtonRef;

RadioButtonRef SetupRadioButton(DBID radioID,char *choiceString) =
	CallDispatchedFunction(kSetupRadioButton);

void DrawRadioButton(RadioButtonRef target) =
	CallDispatchedFunction(kDrawRadioButton);
	
void ActivateRadioButton(RadioButtonRef target) =
	CallDispatchedFunction(kActivateRadioButton);
	
void DeactivateRadioButton(RadioButtonRef target) =
	CallDispatchedFunction(kDeactivateRadioButton);
	
void RadioButtonSelectNext(RadioButtonRef target) =
	CallDispatchedFunction(kRadioButtonSelectNext);
	
void RadioButtonSelectPrevious(RadioButtonRef target) =
	CallDispatchedFunction(kRadioButtonSelectPrevious);
	
short RadioButtonGetSelection(RadioButtonRef target) =
	CallDispatchedFunction(kRadioButtonGetSelection);

void RadioButtonSetSelection(RadioButtonRef target, short selection) =
	CallDispatchedFunction(kRadioButtonSetSelection);

void RadioButtonIdle(RadioButtonRef target) =
	CallDispatchedFunction(kRadioButtonIdle);
	
void DisposeRadioButtonRef(RadioButtonRef target) =
	CallDispatchedFunction(kDisposeRadioButtonRef);

#endif