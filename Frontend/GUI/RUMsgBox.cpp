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

RUMsgBox::RUMsgBox(GPanel* gPanel, int newType, GeneralListener f)
{
	panel = gPanel;
	type = newType;
	MouseDownListener = f;
	confirmButtonClicked = -1;
	msgButtonOK = NULL;
	confirmButtonYES = NULL;
	confirmButtonNO = NULL;
	inputText = NULL;
	inputButtonSubmit = NULL;

	setWidth(DEFAULT_WIDTH);
	setHeight(DEFAULT_HEIGHT);
	setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
	toggleBG(true);
	setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
	setBorderWidth(2);
	toggleBorder(true);
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

void RUMsgBox::MsgBox(GPanel* cPanel, const char* newTitle, const char* newMsg, int type, GeneralListener f)
{
    shmea::GString title(newTitle);
    shmea::GString msg(newMsg);
    MsgBox(cPanel, title, msg, type, f);
}

void RUMsgBox::MsgBox(GPanel* cPanel, shmea::GString newTitle, const char* newMsg, int type, GeneralListener f)
{
    shmea::GString msg(newMsg);
    MsgBox(cPanel, newTitle, msg, type, f);
}

void RUMsgBox::MsgBox(GPanel* cPanel, const char* newTitle, shmea::GString newMsg, int type, GeneralListener f)
{
    shmea::GString title(newTitle);
    MsgBox(cPanel, title, newMsg, type, f);
}

void RUMsgBox::MsgBox(GPanel* cPanel, shmea::GString title, shmea::GString msg, int type, GeneralListener f)
{
	// Type = Message Box, ConfirmBox, or InputBox
	RUMsgBox* newMsgBox = new RUMsgBox(cPanel, type, f);

	// title
	newMsgBox->lbltitle = new RULabel();
	newMsgBox->lbltitle->setWidth(DEFAULT_WIDTH / 2);
	newMsgBox->lbltitle->setHeight(30);
	newMsgBox->lbltitle->setMarginX(6);
	newMsgBox->lbltitle->setMarginY(6);
	newMsgBox->lbltitle->setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
	newMsgBox->lbltitle->setText(title);
	newMsgBox->lbltitle->setName("MsgBoxTitle");
	newMsgBox->addSubItem(newMsgBox->lbltitle);

	// message
	newMsgBox->lblmsg = new RULabel();
	newMsgBox->lblmsg->setWidth(DEFAULT_WIDTH - 12);
	newMsgBox->lblmsg->setHeight(30);
	newMsgBox->lblmsg->setMarginX(6);
	newMsgBox->lblmsg->setMarginY((DEFAULT_HEIGHT / 2.0f) - newMsgBox->lblmsg->getHeight());
	newMsgBox->lblmsg->setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
	newMsgBox->lblmsg->setText(msg);
	newMsgBox->lblmsg->setName("MsgBoxMessage");
	newMsgBox->addSubItem(newMsgBox->lblmsg);

	switch (type)
	{
	case MESSAGEBOX:

		newMsgBox->msgButtonOK = new RUButton();
		newMsgBox->msgButtonOK->setWidth(60);
		newMsgBox->msgButtonOK->setHeight(25);
		newMsgBox->msgButtonOK->setMarginX(DEFAULT_WIDTH - newMsgBox->msgButtonOK->getWidth() - 6);	// -6 = padding
		newMsgBox->msgButtonOK->setMarginY(DEFAULT_HEIGHT - newMsgBox->msgButtonOK->getHeight() - 6); // -6 = padding
		newMsgBox->msgButtonOK->setText(" OK ");
		newMsgBox->msgButtonOK->setName(OK_BUTTON);
		newMsgBox->addSubItem(newMsgBox->msgButtonOK);

		break;

	case CONFIRMBOX:

		newMsgBox->confirmButtonNO = new RUButton();
		newMsgBox->confirmButtonNO->setWidth(60);
		newMsgBox->confirmButtonNO->setHeight(25);
		newMsgBox->confirmButtonNO->setMarginX(DEFAULT_WIDTH - newMsgBox->confirmButtonNO->getWidth() -
									6); // -6 = padding
		newMsgBox->confirmButtonNO->setMarginY(DEFAULT_HEIGHT - newMsgBox->confirmButtonNO->getHeight() -
									6); // -6 = padding
		newMsgBox->confirmButtonNO->setText(" NO ");
		newMsgBox->confirmButtonNO->setName(NO_BUTTON);
		newMsgBox->addSubItem(newMsgBox->confirmButtonNO);

		newMsgBox->confirmButtonYES = new RUButton();
		newMsgBox->confirmButtonYES->setWidth(60);
		newMsgBox->confirmButtonYES->setHeight(25);
		newMsgBox->confirmButtonYES->setMarginX(newMsgBox->confirmButtonNO->getMarginX() - newMsgBox->confirmButtonYES->getWidth() -
									 6); // -6 = padding
		newMsgBox->confirmButtonYES->setMarginY(DEFAULT_HEIGHT - newMsgBox->confirmButtonNO->getHeight() -
									 6); // -6 = padding
		newMsgBox->confirmButtonYES->setText(" YES ");
		newMsgBox->confirmButtonYES->setName(YES_BUTTON);
		newMsgBox->addSubItem(newMsgBox->confirmButtonYES);

		break;

	case INPUTBOX:

		newMsgBox->inputButtonSubmit = new RUButton();
		newMsgBox->inputButtonSubmit->setWidth(60);
		newMsgBox->inputButtonSubmit->setHeight(20);
		newMsgBox->inputButtonSubmit->setMarginX(DEFAULT_WIDTH - newMsgBox->inputButtonSubmit->getWidth());
		newMsgBox->inputButtonSubmit->setMarginY(DEFAULT_HEIGHT - newMsgBox->inputButtonSubmit->getHeight() - 6);
		newMsgBox->inputButtonSubmit->setText("Submit");
		newMsgBox->inputButtonSubmit->setName(SUBMIT_BUTTON);
		newMsgBox->addSubItem(newMsgBox->inputButtonSubmit);

		newMsgBox->inputText = new RUTextbox();
		newMsgBox->inputText->setWidth(cPanel->getWidth() - newMsgBox->inputButtonSubmit->getWidth() - 6); // -6 = padding
		newMsgBox->inputText->setHeight(20);
		newMsgBox->inputText->setMarginX(DEFAULT_WIDTH - newMsgBox->inputText->getWidth() -
							  newMsgBox->inputButtonSubmit->getWidth() - 6);			// -6 = padding
		newMsgBox->inputText->setMarginY(DEFAULT_HEIGHT - newMsgBox->inputText->getHeight() - 6); // 6 = padding
		newMsgBox->inputText->setStaticBorder(true);
		newMsgBox->inputText->setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
		newMsgBox->inputText->setBorderWidth(2);
		newMsgBox->inputText->toggleBorder(true);
		newMsgBox->addSubItem(newMsgBox->inputText);

		break;
	}


	newMsgBox->setX((cPanel->getWidth() / 2.0f) - (newMsgBox->getWidth() / 2.0f));
	newMsgBox->setY((cPanel->getHeight() / 2.0f) - (newMsgBox->getHeight() / 2.0f));
	newMsgBox->setName(title + ":" + msg);
	cPanel->addSubItem(newMsgBox, GItem::Z_BACK);
}
