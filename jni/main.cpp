//
// Created by dev on 16-12-3.
//

#include "ui_test.h"

int main(int argc, char** argv)
{
    UiBase* pUi = new UiTest();

    pUi->run();

    delete pUi;
	return 0;
}
