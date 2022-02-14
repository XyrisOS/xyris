/**
 * @file keyboard.cpp
 * @author Michel (JMallone) Gomes (michels@utfpr.edu.br)
 * @brief Keyboard management and control
 * @version 0.1
 * @date 2022-02-13
 *
 * @copyright Copyright the Xyris Contributors (c) 2022
 *
 */

#include <Devices/Keyboard/KBScanCodeSet1.hpp>
#include <Devices/Keyboard/keyboard.hpp>
#include <Arch/Arch.hpp>

namespace Keyboard {

char getPressKey(){
    uint8_t scanCode = readByte(0x60);
    
    if (scanCode < 0x3A){  
        return KBSet1::ScanCodeLookupTable[scanCode];
    }
    return 0;
}
}; // !namespace Keyboard