//
//  SCWorkerCU.cpp
//  ControlUnitTest
//
//  Created by Claudio Bisegni on 7/20/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "SCWorkerCU.h"
#include "SinWaveCommand.h"
#include "TestCorrelatingCommand.h"
#include "TestSetOnly.h"
#include <boost/thread.hpp>

using namespace chaos::common::data;

using namespace chaos::common::data::cache;
using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::cu::driver_manager::driver;

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(SCWorkerCU)
/*
 Construct a new CU with an identifier
 */
SCWorkerCU::SCWorkerCU(const string& _control_unit_id,
                       const string& _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers):
chaos::cu::control_manager::SCAbstractControlUnit(_control_unit_id,
                                                  _control_unit_param,
                                                  _control_unit_drivers) {}

SCWorkerCU::~SCWorkerCU() {}

/*
 Return the default configuration
 */
void SCWorkerCU::unitDefineActionAndDataset()  {
    //set the base information
    RangeValueInfo rangeInfoTemp;
    //cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    
    // add two execution channels to the contor unit
    //addExecutionChannels(2);
    
    //install a command
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(SinWaveCommand), true); //is the default
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(TestCorrelatingCommand));
    installCommand(BATCH_COMMAND_GET_DESCRIPTION(TestSetOnly));
    
    //set the sin_base command to run on second channels
    //setDefaultCommand("sinwave_base", 2);
    //setup the dataset
    //    addAttributeToDataSet("sin_wave",
    //                          "The sin waveform",
    //                          DataType::TYPE_BYTEARRAY,
    //                          DataType::Output,
    //                          10000);
    addBinaryAttributeAsSubtypeToDataSet("sin_wave",
                                         "The sin waveform",
                                         DataType::SUB_TYPE_DOUBLE,
                                         10000,
                                         DataType::Output);
    addAttributeToDataSet("run_counter",
                          "The number of run since last init phase",
                          DataType::TYPE_INT64,
                          DataType::Output);
    
    addAttributeToDataSet("points",
                          "The number of point that compose the wave",
                          DataType::TYPE_INT32,
                          DataType::Input);
    
    addAttributeToDataSet("frequency",
                          "The frequency of the wave [1-10Mhz]",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("bias",
                          "The bias of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("phase",
                          "The phase of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("gain",
                          "The gain of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addAttributeToDataSet("gain_noise",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    
    addStateVariable(StateVariableTypeAlarmCU,
                     "hardware_failure",
                     "Notify when value to reach has not been reached");
    
    addStateVariable(StateVariableTypeAlarmDEV,
                     "out_of_set",
                     "Notify when value to reach has not been reached");
}

void SCWorkerCU::unitDefineCustomAttribute() {
    bool quit = false;
    //here are defined the custom shared variable
    getAttributeCache()->addCustomAttribute("quit", 1, chaos::DataType::TYPE_BOOLEAN);
    getAttributeCache()->setCustomAttributeValue("quit", &quit, sizeof(bool));
}

// Abstract method for the initialization of the control unit
void SCWorkerCU::unitInit()  {
}

// Abstract method for the start of the control unit
void SCWorkerCU::unitStart()  {
    
}

// Abstract method for the stop of the control unit
void SCWorkerCU::unitStop()  {
    
}

// Abstract method for the deinit of the control unit
void SCWorkerCU::unitDeinit()  {
    
}

//! restore the control unit to snapshot
bool SCWorkerCU::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache)  {
    ChaosStringVector keys;
    snapshot_cache->getAttributeNames(DOMAIN_INPUT, keys);
    for(ChaosStringVectorIterator it = keys.begin(), end = keys.end();
        it != end;
        it++) {
        INFO_LOG(SCWorkerCU) << "[RESTORE]-" << *it << "-"<<  snapshot_cache->getAttributeValue(DOMAIN_INPUT, *it)->getAsVariant().asString();
    }
    keys.clear();
    snapshot_cache->getAttributeNames(DOMAIN_OUTPUT, keys);
    for(ChaosStringVectorIterator it = keys.begin(), end = keys.end();
        it != end;
        it++) {
        INFO_LOG(SCWorkerCU) << "[RESTORE]-" << *it << "-"<<  snapshot_cache->getAttributeValue(DOMAIN_OUTPUT, *it)->getAsVariant().asString();
    }
    keys.clear();
    snapshot_cache->getAttributeNames(DOMAIN_SYSTEM, keys);
    for(ChaosStringVectorIterator it = keys.begin(), end = keys.end();
        it != end;
        it++) {
        INFO_LOG(SCWorkerCU) << "[RESTORE]-" << *it << "-"<<  snapshot_cache->getAttributeValue(DOMAIN_SYSTEM, *it)->getAsVariant().asString();
    }
    return true;
}
