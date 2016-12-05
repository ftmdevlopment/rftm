//
// Created by dev on 16-12-3.
//

#include "ui_main.h"

int main(int argc, char** argv)
{
    UiBase::SetIgnoreRelease(true);

    UiBase* pUi = new UiMain();

    pUi->run();

    delete pUi;

	return 0;
}
