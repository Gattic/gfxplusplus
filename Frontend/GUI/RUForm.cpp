// Copyright 2025 Robert Carneiro
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
#include "RUForm.h"
#include "../GItems/Mini/RUKeyDown.h"
#include "../GItems/Mini/RUKeyUp.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GItems/GPanel.h"
#include "../Graphics/graphics.h"

RUForm::RUForm(shmea::GString formName)
{
    name = formName;
}

RUForm::~RUForm()
{
}

void RUForm::updateBackground(gfxpp* cGfx)
{
    // RUForm itself draws nothing; it aggregates children
}

shmea::GString RUForm::getType() const
{
    return "RUForm";
}

int RUForm::findFocusedIndex() const
{
    for (unsigned int i = 0; i < subitems.size(); ++i)
    {
        GItem* cItem = subitems[i];
        if (!cItem)
            continue;
        if (cItem->isFocused())
            return (int)i;
    }
    return -1;
}

bool RUForm::isFocusable(GItem* item) const
{
    if (!item)
        return false;
    if (!item->isVisible())
        return false;
    // Heuristic: any RUComponent is focusable and can receive keyboard input through RUKeyDown/Up
    return true;
}

void RUForm::focusByIndex(gfxpp* cGfx, int index)
{
    if (!cGfx)
        return;
    if (subitems.empty())
        return;
    int n = (int)subitems.size();
    if (index < 0)
        index = (index % n + n) % n;
    else
        index = index % n;
    // seek next focusable starting from index inclusive
    for (int step = 0; step < n; ++step)
    {
        int i = (index + step) % n;
        GItem* candidate = subitems[i];
        if (isFocusable(candidate))
        {
            cGfx->setFocus(candidate);
            return;
        }
    }
}

void RUForm::focusNext(gfxpp* cGfx)
{
    int cur = findFocusedIndex();
    if (cur < 0)
        focusByIndex(cGfx, 0);
    else
        focusByIndex(cGfx, cur + 1);
}

void RUForm::focusPrev(gfxpp* cGfx)
{
    int cur = findFocusedIndex();
    if (cur < 0)
        focusByIndex(cGfx, (int)subitems.size() - 1);
    else
        focusByIndex(cGfx, cur - 1);
}

void RUForm::onKeyDown(gfxpp* cGfx, GPanel* cPanel, GfxKeycode keyPressed, Uint16 keyModPressed)
{
    (void)cPanel;
    if (!cGfx)
        return;

    // TAB cycles forward; Shift+TAB cycles backward
    if (keyPressed == SDLK_TAB)
    {
        if ((keyModPressed & KMOD_SHIFT) != 0)
            focusPrev(cGfx);
        else
            focusNext(cGfx);
        return;
    }

    // Arrow Up/Down also navigate
    if (keyPressed == GFXK_UP)
    {
        focusPrev(cGfx);
        return;
    }
    if (keyPressed == GFXK_DOWN)
    {
        focusNext(cGfx);
        return;
    }
}

void RUForm::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
    (void)parentOffset;
    // RUForm is a logical container only; it does not lay out its children.
}

void RUForm::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* parentPanel,
                           GfxEvent event, int mouseX, int mouseY)
{
    if (!eventsStatus)
        return;
    if (!parentPanel)
        return;
    if (!visible)
        return;

    // Then handle key-based focus navigation if this form is focused
    if (event.type == GFX_KEYDOWN)
    {
        GItem* selfItem = dynamic_cast<GItem*>(this);
        if (selfItem && selfItem->isFocused())
        {
            onKeyDown(cGfx, parentPanel, event.key.keysym.sym, event.key.keysym.mod);
        }
        else
        {
            // If focus is on one of our children and TAB/UP/DOWN pressed, intercept to move focus
            GfxKeycode keyPressed = event.key.keysym.sym;
            Uint16 keyModPressed = event.key.keysym.mod;
            bool isNav = (keyPressed == SDLK_TAB) || (keyPressed == GFXK_UP) || (keyPressed == GFXK_DOWN);
            if (isNav)
            {
                for (unsigned int i = 0; i < subitems.size(); ++i)
                {
                    if (subitems[i] && subitems[i]->isFocused())
                    {
                        if (keyPressed == SDLK_TAB && (keyModPressed & KMOD_SHIFT))
                            focusPrev(cGfx);
                        else if (keyPressed == SDLK_TAB || keyPressed == GFXK_DOWN)
                            focusNext(cGfx);
                        else if (keyPressed == GFXK_UP)
                            focusPrev(cGfx);
                        break;
                    }
                }
            }
        }
    }
}

void RUForm::updateBackgroundHelper(gfxpp* cGfx)
{
    (void)cGfx;
    // RUForm does not render its children; they render via their layout ownership.
}



