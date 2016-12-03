//
// Created by dev on 16-12-3.
//

#ifndef RFTM_UI_MAIN_H
#define RFTM_UI_MAIN_H

#include "ui_base.h"

class UiTest : public UiBase
{
protected:
    void Draw();

    void OnKey(int code, int value);

    void OnLeftTouch(int code, int value);

    void OnRightTouch(int code, int value);
};


#endif // RFTM_UI_MAIN_H
