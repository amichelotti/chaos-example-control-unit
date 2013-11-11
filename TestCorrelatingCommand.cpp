//
//  TestCorrelatingCommand.cpp
//  ExampleControlUnit
//
//  Created by Claudio Bisegni on 08/11/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "TestCorrelatingCommand.h"
#define CMDCU_ LAPP_ << "[TestCorrelatingCommand] - "

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cu::control_manager::slow_command;

uint64_t TestCorrelatingCommand::instance_cout = 0;

TestCorrelatingCommand::TestCorrelatingCommand()  {
	start_time = 0;
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
	start_time = shared_stat->lastCmdStepStart;
    CMDCU_ << "Simulate set handler from ";
    instance_cout++;
    if(data && data->hasKey("rs_mode")) {
        switch(data->getInt32Value("rs_mode")) {
            case chaos::cu::control_manager::slow_command::RunningStateType::RS_Exsc:
                SL_EXEC_RUNNIG_STATE
                break;
            case chaos::cu::control_manager::slow_command::RunningStateType::RS_Kill:
                SL_KILL_RUNNIG_STATE
                break;
            case chaos::cu::control_manager::slow_command::RunningStateType::RS_Stack:
                SL_STACK_RUNNIG_STATE
                break;
            default:
                SL_EXEC_RUNNIG_STATE
                
        }
    }
	setFeatures(ccc_slow_command::features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, 2000000);
}

// Correlation and commit phase
void TestCorrelatingCommand::ccHandler() {
    uint64_t timeDiff = shared_stat->lastCmdStepStart - start_time;
    CMDCU_ << "["<< instance_cout <<"] Simulate correlation..." << timeDiff << " of " << 2000000;
    if(timeDiff > 20000000) {
			//we can terminate
		CMDCU_ << "End correlate simulation...";
		SL_END_RUNNIG_STATE
	}
}

bool TestCorrelatingCommand::timeoutHandler() {
	//move the state machine on fault
	throw chaos::CException(1, "timeout reached", __FUNCTION__);
	return true;
}
