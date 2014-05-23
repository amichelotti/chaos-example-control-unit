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

int main(int argc, char * argv[])
{
    try {
        ChaosUIToolkit::getInstance()->init(argc, argv);
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID("rt_sin_a", 2000000);
        if (!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
		for(int idx = 0; idx < 10; idx++) {
			chaos::common::data::CDataWrapper *result = NULL;
			controller->sendCustomRequest("actionTestOne", NULL, &result);
			if(result)delete(result);
		}
        ChaosUIToolkit::getInstance()->deinit();
    } catch (CException& e) {
        std::cerr << e.errorCode << " - " << e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    
    return 0;
}

