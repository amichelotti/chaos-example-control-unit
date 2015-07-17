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

BATCH_COMMAND_OPEN_DESCRIPTION_WITH_SLOWCOMMAND_INSTANCER(TestCorrelatingCommand,
                                                          "Test method for correlations",
                                                          "DBDD60DC-8462-4FAD-8CC0-008960A3B0CA")
BATCH_COMMAND_ADD_INT32_PARAMTER("exception-location", "The location where generate the exception(1-acquire 2-correlation)",/* BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY*/0)
BATCH_COMMAND_ADD_STRING_PARAMTER("exception", "Message to report into the exception", 0)
BATCH_COMMAND_ADD_STRING_PARAMTER("correlation-message", "Message to report into the correlation phase", 0)
BATCH_COMMAND_CLOSE_DESCRIPTION()

TestCorrelatingCommand::TestCorrelatingCommand()  {
	start_time = 0;
    local_instance_count = instance_cout++;
	exception_message = 10;//out of parameter
}

TestCorrelatingCommand::~TestCorrelatingCommand() {
	
}

// return the implemented handler
uint8_t TestCorrelatingCommand::implementedHandler() {
    return  HandlerType::HT_Set |
	HandlerType::HT_Correlation;
			//HandlerType::HT_Acquisition;
}

// Start the command execution
void TestCorrelatingCommand::setHandler(CDataWrapper *data) {
	start_time = getSetTime();
    CMDCU_ << "Start simulation at " << start_time << " microeconds ";
	if(data && data->hasKey("exception")) {
		exception_message = data->getStringValue("exception");
	}
	
	if(data && data->hasKey("exception-location")) {
		exception_location = data->getInt32Value("exception-location");
	}
	
	if(data && data->hasKey("correlation-message")) {
		correlation_message = data->getStringValue("correlation-message");
	}

	
	if(exception_location == 0) {
		throw CException(-1, exception_message, __PRETTY_FUNCTION__);
	}
	
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
	setFeatures(features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)20000);
}

void TestCorrelatingCommand::acquireHandler() {
	if(exception_location == 1) {
		throw CException(-1, exception_message, __PRETTY_FUNCTION__);
	}
}

// Correlation and commit phase
void TestCorrelatingCommand::ccHandler() {
	if(exception_location == 2) {
		throw CException(-1, exception_message, __PRETTY_FUNCTION__);
	}
    uint64_t timeDiff = getStartStepTime() - start_time;
    CMDCU_ << "Simulate correlation..." << timeDiff << " of " << 20000;
	if(correlation_message.size()) CMDCU_ << "Correlation Message: " << correlation_message;
    if(timeDiff > 20000) {
			//we can terminate
		CMDCU_ << "End correlate simulation...";
		BC_END_RUNNIG_PROPERTY
	}
}

bool TestCorrelatingCommand::timeoutHandler() {
	uint64_t timeDiff = getLastStepTime() - start_time;
	CMDCU_ << "timeout after " << timeDiff << " milliseconds";
	//move the state machine on fault
	throw chaos::CException(1, "timeout reached", __FUNCTION__);
	return true;
}
