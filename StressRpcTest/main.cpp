//
//  main.cpp
//  StressRpcTest
//
//  Created by Claudio Bisegni on 25/04/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <iostream>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>

using namespace chaos;
using namespace chaos::ui;

int main(int argc, const char * argv[])
{
    try {
        ChaosUIToolkit::getInstance()->init(argc, argv);
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(mess_device_id, timeout);
        if (!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
        
        ChaosUIToolkit::getInstance()->deinit();
    } catch (CException& e) {
        std::cerr << e.errorCode << " - " << e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    
    return 0;
}

