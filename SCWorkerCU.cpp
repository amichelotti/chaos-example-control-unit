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
    installCommand<SinWaveCommand>("sinwave_base");
    installCommand<TestCorrelatingCommand>("corr_test");
	
    //set the sin_base command to run on second channels
	//setDefaultCommand("sinwave_base", 2);
    setDefaultCommand("sinwave_base");
    //setup the dataset
    addAttributeToDataSet("sinWave",
                          "The sin waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          10000);
    
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
void SCWorkerCU::unitRestoreToSnapshot(const std::string& restore_snapshot_tag,
									   chaos::cu::control_manager::AbstractSharedDomainCache * const restore_cache) throw(CException) {
	LAPP_ << "restore to snapshot " << restore_snapshot_tag;
	if(restore_cache &&
	   restore_cache->getSharedDomain(DOMAIN_INPUT).hasAttribute(std::string("corr_test"))) {
		auto_ptr<CDataWrapper> cmd_pack(restore_cache->getAttributeValue(DOMAIN_INPUT, "corr_test")->getValueAsCDatawrapperPtr(true));
		LAPP_ << "corr_test = " << cmd_pack->getJSONString();
	}

}