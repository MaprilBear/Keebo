#include "Bootloader.h"
#include "inc/CortexM.h"
#include "inc/PLL.h"

int main() {
    PLL_Init(Bus80MHz);

    Bootloader_Entry();
}