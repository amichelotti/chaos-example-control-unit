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

using namespace chaos::common::batch_command;
using namespace chaos::common::data::cache;
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
void SCWorkerCU::unitDefineActionAndDataset() throw(CException) {
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
    
    addAlarm("out_of_set",
             "Notify when value to reach has not been reached");
}

void SCWorkerCU::unitDefineCustomAttribute() {
    bool quit = false;
    //here are defined the custom shared variable
    getAttributeCache()->addCustomAttribute("quit", 1, chaos::DataType::TYPE_BOOLEAN);
    getAttributeCache()->setCustomAttributeValue("quit", &quit, sizeof(bool));
}

// Abstract method for the initialization of the control unit
void SCWorkerCU::unitInit() throw(CException) {
}

// Abstract method for the start of the control unit
void SCWorkerCU::unitStart() throw(CException) {
    
}

// Abstract method for the stop of the control unit
void SCWorkerCU::unitStop() throw(CException) {
    
}

// Abstract method for the deinit of the control unit
void SCWorkerCU::unitDeinit() throw(CException) {
    
}

//! restore the control unit to snapshot
bool SCWorkerCU::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw(CException) {
    uint64_t cmd_id = 0;
    if(snapshot_cache &&
       snapshot_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute(std::string("TestCorrelatingCommand/correlation-message"))) {
        auto_ptr<CDataWrapper> cmd_pack(new CDataWrapper());
        cmd_pack->addStringValue("correlation-message", snapshot_cache->getAttributeValue(DOMAIN_INPUT, "TestCorrelatingCommand/correlation-message")->getAsVariant().asString());
        
        LAPP_ << "corr_test = " << cmd_pack->getJSONString();
        submitSlowCommand("TestCorrelatingCommand", cmd_pack.release(), cmd_id);
        
        std::auto_ptr<CommandState> cmd_state;
        do{
            cmd_state = getStateForCommandID(cmd_id);
            if(!cmd_state.get()) break;
            
            switch (cmd_state->last_event) {
                case BatchCommandEventType::EVT_QUEUED:
                    LAPP_ << cmd_id << " -> QUEUED";
                    break;
                case BatchCommandEventType::EVT_RUNNING:
                    LAPP_ << cmd_id << " -> RUNNING";
                    break;
                case BatchCommandEventType::EVT_WAITING:
                    LAPP_ << cmd_id << " -> WAITING";
                    break;
                case BatchCommandEventType::EVT_PAUSED:
                    LAPP_ << cmd_id << " -> PAUSED";
                    break;
                case BatchCommandEventType::EVT_KILLED:
                    LAPP_ << cmd_id << " -> KILLED";
                    break;
                case BatchCommandEventType::EVT_COMPLETED:
                    LAPP_ << cmd_id << " -> COMPLETED";
                    break;
                case BatchCommandEventType::EVT_FAULT:
                    LAPP_ << cmd_id << " -> FALUT";
                    break;
            }
            
           boost::this_thread::sleep_for(boost::chrono::seconds(1));
        }while(cmd_state->last_event != BatchCommandEventType::EVT_COMPLETED &&
               cmd_state->last_event != BatchCommandEventType::EVT_FAULT &&
               cmd_state->last_event != BatchCommandEventType::EVT_KILLED);
        
        LAPP_ << "Resubmitted command in restore method has ended";
    }
    
    return true;
}
