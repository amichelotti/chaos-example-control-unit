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
	SL_EXEC_RUNNIG_STATE
}

// Correlation and commit phase
void TestCorrelatingCommand::ccHandler() {
    uint64_t timeDiff = shared_stat->lastCmdStepStart - start_time;
    CMDCU_ << "Simulate correlation..." << timeDiff << " of " << 10000000;
    if(timeDiff > 10000000) {
			//we can terminate
		CMDCU_ << "End correlate simulation...";
		SL_END_RUNNIG_STATE
	}
}
