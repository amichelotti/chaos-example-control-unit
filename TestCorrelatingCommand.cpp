//
//  TestCorrelatingCommand.cpp
//  ExampleControlUnit
//
//  Created by Claudio Bisegni on 08/11/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "TestCorrelatingCommand.h"
#define CMDCU_ LAPP_ << "[TestCorrelatingCommand] - " << getDeviceID() << " - [" << getUID()<< "-" << local_instance_count << "] - "

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;

uint64_t TestCorrelatingCommand::instance_cout = 0;

TestCorrelatingCommand::TestCorrelatingCommand()  {
	start_time = 0;
    local_instance_count = instance_cout++;
}

TestCorrelatingCommand::~TestCorrelatingCommand() {
	
}

// return the implemented handler
uint8_t TestCorrelatingCommand::implementedHandler() {
    return  HandlerType::HT_Set |
			HandlerType::HT_Correlation;
}

// Start the command execution
void TestCorrelatingCommand::setHandler(CDataWrapper *data) {
	start_time = getSetTime();
    CMDCU_ << "Start simulation at " << start_time << " microeconds ";
    if(data && data->hasKey("rs_mode")) {
        switch(data->getInt32Value("rs_mode")) {
            case RunningPropertyType::RP_Exsc:
                BC_EXEC_RUNNIG_PROPERTY
                break;
            case RunningPropertyType::RP_Normal:
                BC_NORMAL_RUNNIG_PROPERTY
                break;
            case RunningPropertyType::RP_End:
                BC_END_RUNNIG_PROPERTY
                break;
            default:
                BC_EXEC_RUNNIG_PROPERTY
                
        }
    } else {
         BC_EXEC_RUNNIG_PROPERTY
    }
	setFeatures(features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)2000000);
}

// Correlation and commit phase
void TestCorrelatingCommand::ccHandler() {
    uint64_t timeDiff = getStartStepTime() - start_time;
    CMDCU_ << "Simulate correlation..." << timeDiff << " of " << 20000000;
    if(timeDiff > 20000000) {
			//we can terminate
		CMDCU_ << "End correlate simulation...";
		BC_END_RUNNIG_PROPERTY
	}
}

bool TestCorrelatingCommand::timeoutHandler() {
	uint64_t timeDiff = getLastStepTime() - start_time;
	CMDCU_ << "timeout after " << timeDiff << " microsecond";
	//move the state machine on fault
	throw chaos::CException(1, "timeout reached", __FUNCTION__);
	return true;
}
