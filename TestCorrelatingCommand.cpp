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

using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;

uint64_t TestCorrelatingCommand::instance_cout = 0;
/*
 * T
 */
BATCH_COMMAND_OPEN_DESCRIPTION(,TestCorrelatingCommand,
                               "Test method for correlations",
                               "DBDD60DC-8462-4FAD-8CC0-008960A3B0CA")
BATCH_COMMAND_ADD_INT64_PARAM("exception-location", "The location where generate the exception(0-set,2-acquire 3-correlation)",/* BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG_MANDATORY*/0)
BATCH_COMMAND_ADD_STRING_PARAM("exception", "Message to report into the exception", 0)
BATCH_COMMAND_ADD_STRING_PARAM("correlation-message", "Message to report into the correlation phase", 0)
BATCH_COMMAND_ADD_INT32_PARAM("rs_mode", "Set the run mode RP_Exsc = 0, RP_Normal = 1", 0)
BATCH_COMMAND_CLOSE_DESCRIPTION()

TestCorrelatingCommand::TestCorrelatingCommand():
work_time_accumulator(0){
    local_instance_count = instance_cout++;
    exception_location = - 1;//out of parameter
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
    if(data && data->hasKey("exception")) {
        exception_message = data->getStringValue("exception");
    }
    
    if(data && data->hasKey("exception-location")) {
        exception_location = data->getInt64Value("exception-location");
    }
    
    if(data && data->hasKey("correlation-message")) {
        correlation_message = data->getStringValue("correlation-message");
    }
    
    
    if(exception_location == 1) {
        throw CException(-1, exception_message, __PRETTY_FUNCTION__);
    }
    
    if(data && data->hasKey("rs_mode")) {
        switch(data->getInt32Value("rs_mode")) {
            case RunningPropertyType::RP_Exsc:
                BC_EXEC_RUNNING_PROPERTY
                break;
            case RunningPropertyType::RP_Normal:
                BC_NORMAL_RUNNING_PROPERTY
                break;
            case RunningPropertyType::RP_End:
                BC_END_RUNNING_PROPERTY
                break;
            default:
                BC_EXEC_RUNNING_PROPERTY
        }
    } else {
        BC_EXEC_RUNNING_PROPERTY
    }
    
    if(BC_CHECK_EXEC_RUNNING_PROPERTY ||
       BC_CHECK_NORMAL_RUNNING_PROPERTY) {
        setBusyFlag(true);
        setAlarmSeverity("out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelWarning);
    }
    
    setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)100000);
    setFeatures(features::FeaturesFlagTypes::FF_SET_COMMAND_TIMEOUT, (uint64_t)30000000);
}

void TestCorrelatingCommand::acquireHandler() {
    if(exception_location == 2) {
        throw CException(-1, exception_message, __PRETTY_FUNCTION__);
    }
}

// Correlation and commit phase
void TestCorrelatingCommand::ccHandler() {
    if(exception_location == 3) {
        throw CException(-1, exception_message, __PRETTY_FUNCTION__);
    }
    
    work_time_accumulator += getLastStepDuration();
    uint64_t timeDiff = getStartStepTime() - getSetTime();
    if(correlation_message.size()) CMDCU_ << "Correlation Message: " << correlation_message << " " << timeDiff << " of " << 20000;
    if(timeDiff > 20000) {
        //we can terminate
        CMDCU_ << "End correlate simulation... average step time[microsecond]" << ((double)work_time_accumulator/getStepCounter());
        BC_END_RUNNING_PROPERTY
        setBusyFlag(false);
        setAlarmSeverity("out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    }
}

bool TestCorrelatingCommand::timeoutHandler() {
    setBusyFlag(false);
    setAlarmSeverity("out_of_set", chaos::common::alarm::MultiSeverityAlarmLevelClear);
    uint64_t timeDiff = getStartStepTime() - getSetTime();
    CMDCU_ << "timeout after " << timeDiff << " milliseconds";
    //move the state machine on fault
    throw chaos::CException(1, "timeout reached", __FUNCTION__);
    return true;
}
