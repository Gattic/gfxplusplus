// Copyright 2020 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "RUMsgBox.h"
#include "../GItems/GPanel.h"
#include "../GItems/RUColors.h"
#include "Text/RUButton.h"
#include "Text/RULabel.h"
#include "Text/RUTextbox.h"

shmea::GString RUMsgBox::OK_BUTTON = "OK_Button";
shmea::GString RUMsgBox::YES_BUTTON = "YES_Button";
shmea::GString RUMsgBox::NO_BUTTON = "NO_Button";
shmea::GString RUMsgBox::SUBMIT_BUTTON = "SUBMIT_Button";

RUMsgBox::RUMsgBox(GPanel* gPanel, shmea::GString newTitle, shmea::GString newMessage, int newType, GeneralListener f)
{
	panel = gPanel;
	title = newTitle;
	message = newMessage;
	type = newType;
	MouseDownListener = f;
	confirmButtonClicked = -1;

	setWidth(DEFAULT_WIDTH);
	setHeight(DEFAULT_HEIGHT);
	setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
	toggleBG(true);
	setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
	setBorderWidth(2);
	toggleBorder(true);

	// title
	lbltitle = new RULabel();
	lbltitle->setWidth(DEFAULT_WIDTH / 2);
	lbltitle->setHeight(30);
	lbltitle->setMarginX(6);
	lbltitle->setMarginY(6);
	lbltitle->setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
	lbltitle->setText(title);
	lbltitle->setName("Title");
	addSubItem(lbltitle);

	// message
	lblmsg = new RULabel();
	lblmsg->setWidth(DEFAULT_WIDTH - 12);
	lblmsg->setHeight(30);
	lblmsg->setMarginX(6);
	lblmsg->setMarginY((DEFAULT_HEIGHT / 2.0f) - lblmsg->getHeight());
	lblmsg->setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
	lblmsg->setText(message);
	lblmsg->setName("Message");
	addSubItem(lblmsg);

	switch (type)
	{
	case MESSAGEBOX:

		msgButtonOK = new RUButton();
		msgButtonOK->setWidth(60);
		msgButtonOK->setHeight(25);
		msgButtonOK->setMarginX(DEFAULT_WIDTH - msgButtonOK->getWidth() - 6);	// -6 = padding
		msgButtonOK->setMarginY(DEFAULT_HEIGHT - msgButtonOK->getHeight() - 6); // -6 = padding
		msgButtonOK->setText(" OK ");
		msgButtonOK->setName(OK_BUTTON);
		addSubItem(msgButtonOK);

		break;

	case CONFIRMBOX:

		confirmButtonNO = new RUButton();
		confirmButtonNO->setWidth(60);
		confirmButtonNO->setHeight(25);
		confirmButtonNO->setMarginX(DEFAULT_WIDTH - confirmButtonNO->getWidth() -
									6); // -6 = padding
		confirmButtonNO->setMarginY(DEFAULT_HEIGHT - confirmButtonNO->getHeight() -
									6); // -6 = padding
		confirmButtonNO->setText(" NO ");
		confirmButtonNO->setName(NO_BUTTON);
		addSubItem(confirmButtonNO);

		confirmButtonYES = new RUButton();
		confirmButtonYES->setWidth(60);
		confirmButtonYES->setHeight(25);
		confirmButtonYES->setMarginX(confirmButtonNO->getMarginX() - confirmButtonYES->getWidth() -
									 6); // -6 = padding
		confirmButtonYES->setMarginY(DEFAULT_HEIGHT - confirmButtonNO->getHeight() -
									 6); // -6 = padding
		confirmButtonYES->setText(" YES ");
		confirmButtonYES->setName(YES_BUTTON);
		addSubItem(confirmButtonYES);

		break;

	case INPUTBOX:

		inputButtonSubmit = new RUButton();
		inputButtonSubmit->setWidth(60);
		inputButtonSubmit->setHeight(20);
		inputButtonSubmit->setMarginX(DEFAULT_WIDTH - inputButtonSubmit->getWidth());
		inputButtonSubmit->setMarginY(DEFAULT_HEIGHT - inputButtonSubmit->getHeight() - 6);
		inputButtonSubmit->setText("Submit");
		inputButtonSubmit->setName(SUBMIT_BUTTON);
		addSubItem(inputButtonSubmit);

		inputText = new RUTextbox();
		inputText->setWidth(this->getWidth() - inputButtonSubmit->getWidth() - 6); // -6 = padding
		inputText->setHeight(20);
		inputText->setMarginX(DEFAULT_WIDTH - inputText->getWidth() -
							  inputButtonSubmit->getWidth() - 6);			// -6 = padding
		inputText->setMarginY(DEFAULT_HEIGHT - inputText->getHeight() - 6); // 6 = padding
		inputText->setStaticBorder(true);
		inputText->setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
		inputText->setBorderWidth(2);
		inputText->toggleBorder(true);
		addSubItem(inputText);

		break;
	}
}

RUMsgBox::~RUMsgBox()
{
	// Delete all subcomponents
	if (lbltitle)
		delete lbltitle;
	lbltitle = NULL;

	if (lblmsg)
		delete lblmsg;
	lblmsg = NULL;

	if (msgButtonOK)
		delete msgButtonOK;
	msgButtonOK = NULL;

	if (confirmButtonYES)
		delete confirmButtonYES;
	confirmButtonYES = NULL;

	if (confirmButtonNO)
		delete confirmButtonNO;
	confirmButtonNO = NULL;

	if (inputText)
		delete inputText;
	inputText = NULL;

	if (inputButtonSubmit)
		delete inputButtonSubmit;
	inputButtonSubmit = NULL;
}

void RUMsgBox::updateBackground(gfxpp* cGfx)
{
	//
}

void RUMsgBox::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUMsgBox: onMouseDown(%d, %d);\n", eventX, eventY);

	// Iterate clicked subcomponents of RUMsgBox
	std::map<int, GItem*>::iterator itr = clickedSubItems.begin();
	for (; itr != clickedSubItems.end(); ++itr)
	{
		shmea::GString compType = itr->second->getType();
		shmea::GString compName = itr->second->getName();
		// printf("Subcomponent '%s' of type '%s' was clicked.\n", compName.c_str(),
		// compType.c_str());

		if (compType == "RUButton")
		{
			switch (type)
			{
			case MESSAGEBOX:

				if (itr->second->getName() == OK_BUTTON)
				{
					msgButtonOKClicked(cGfx);

					// Call the callback we saved
					MouseDownListener.call(getName(), eventX, eventY);
				}
				break;

			case CONFIRMBOX:

				if (itr->second->getName() == YES_BUTTON)
				{
					confirmButtonYESClicked(cGfx);

					// Call the callback we saved
					MouseDownListener.call(getName(), eventX, eventY);
				}
				else if (itr->second->getName() == NO_BUTTON)
				{
					confirmButtonNOClicked(cGfx);
				}
				break;

			case INPUTBOX:

				if (itr->second->getName() == SUBMIT_BUTTON)
				{
					inputButtonSUBMITClicked(cGfx);

					// Call the callback we saved
					MouseDownListener.call(getName(), eventX, eventY);
				}
				break;
			}
		}
	}
}

void RUMsgBox::msgButtonOKClicked(gfxpp* cGfx)
{
	// close it
	setVisible(false);
	panel->removeItem(cGfx, this->getID());

	// printf("\nOK CLICKED\n");
}

void RUMsgBox::confirmButtonYESClicked(gfxpp* cGfx)
{
	// close it and set confirmButtonClicked to 0
	setVisible(false);
	confirmButtonYES->setVisible(false);
	confirmButtonNO->setVisible(false);
	panel->removeItem(cGfx, this->getID());

	confirmButtonClicked = 0;

	// printf("\nYES CLICKED\n");
}

void RUMsgBox::confirmButtonNOClicked(gfxpp* cGfx)
{
	// close it and set confirmButtonClicked to 1
	setVisible(false);
	confirmButtonYES->setVisible(false);
	confirmButtonNO->setVisible(false);
	panel->removeItem(cGfx, this->getID());

	confirmButtonClicked = 1;

	// printf("\nNO CLICKED\n");
}

void RUMsgBox::inputButtonSUBMITClicked(gfxpp* cGfx)
{
	// close it and set inputSubmitText
	setVisible(false);
	inputText->setVisible(false);
	inputButtonSubmit->setVisible(false);
	panel->removeItem(cGfx, this->getID());

	inputSubmitText = inputText->getText();

	// printf("\nSUBMITTED: %s\n", inputSubmitText.c_str());
}

shmea::GString RUMsgBox::getType() const
{
	return "RUMsgBox";
}
