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


#define NUMBER_OF_ITERATION 100

int main(int argc, char * argv[])
{
    try {
		std::string device_id;
		std::string action_name;
		uint32_t timeout = 0;
		uint32_t iteration = 0;
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>("device_id", "The identification string of the device to stress", &device_id);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>("action_name", "The name of cutom action to call", &action_name);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("timeout", "The timeout of rpc call", 1000, &timeout);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("iteration", "The numer of iteration to do", NUMBER_OF_ITERATION, &iteration);
		
        ChaosUIToolkit::getInstance()->init(argc, argv);
		
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("device_id")) {
			throw chaos::CException(-1, "The device id is mandatory", "init");
		}
		
		if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption("action_name")) {
			throw chaos::CException(-1, "The action name id is mandatory", "init");
		}
		
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(device_id, timeout);
        if (!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
		for(int idx = 0; idx < iteration; idx++) {
			controller->sendCustomMessage(action_name.c_str(), NULL);
			if(idx && ((idx % 100) == 0)) {
				LAPP_ << "Message sent: " << idx;
			}
		}
		LAPP_ << "Message sent: " << iteration;
        ChaosUIToolkit::getInstance()->deinit();
    } catch (CException& e) {
        std::cerr << e.errorCode << " - " << e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    
    return 0;
}

