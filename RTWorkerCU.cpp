
/*
 *	RTWorkerCU.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#include <boost/thread.hpp>

#include "RTWorkerCU.h"
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/action/ActionDescriptor.h>

#include <boost/lexical_cast.hpp>

#include <json/json.h>

#include <cmath>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <stdlib.h>
using namespace chaos;

#define SIMULATED_DEVICE_ID     "SIN_DEVICE"
#define DS_ELEMENT_1            "sinOutput"
#define DS_ELEMENT_2            "sinAltitude"
#define DS_ELEMENT_3            "sinPahseTime"

#define TEST_BUFFER_DIM         100
#define CU_DELAY_FROM_TASKS     1000000 //1Sec
#define ACTION_TWO_PARAM_NAME   "actionTestTwo_paramName"

PUBLISHABLE_CONTROL_UNIT_IMPLEMENTATION(RTWorkerCU)

using namespace chaos::cu::control_manager;
using namespace chaos::cu::driver_manager::driver;

/*
 Construct a new CU with an identifier
 */
RTWorkerCU::RTWorkerCU(const string& _control_unit_id,
                       const string& _control_unit_param,
                       const ControlUnitDriverList& _control_unit_drivers):
//call base constructor
chaos::cu::control_manager::RTAbstractControlUnit(_control_unit_id,
                                                  _control_unit_param,
                                                  _control_unit_drivers),
out_run_counter(NULL),
crash_run_count(0),
crasch_occured(false),
driver(NULL),
generation_data(NULL),
crash_location(-1){
    if(getCUParam().size()>0) {
        //scan json option
        Json::Reader    json_reader;
        Json::Value     json_params;
        if(json_reader.parse(getCUParam(), json_params)) {
            /*!
             crash_location can be:
             0: definition phase
             1: init phase
             2: start phase
             3: stop phase
             4: deinit phase
             5: run phase
             */
            const Json::Value& _crash_location = json_params["crash_location"];
            const Json::Value&  _crash_run_count = json_params["crash_run_count"];
            if(!_crash_location.isNull()) {
                //we nned to crasch in some situation
                crash_location = _crash_location.asInt();
            }
            if(!_crash_run_count.isNull()) {
                //we nned to crasch in some situation
                crash_run_count = _crash_run_count.asUInt();
            }
        }
    }
}

/*
 Destructor a new CU with an identifier
 */
RTWorkerCU::~RTWorkerCU() {
    
}

/*
 Return the default configuration
 */
void RTWorkerCU::unitDefineActionAndDataset() throw(CException) {
    if(crash_location == 0) throw CException(-1, "Test Exception in definition phase", __PRETTY_FUNCTION__);
    
    //set the default delay for the CU
    setDefaultScheduleDelay(CU_DELAY_FROM_TASKS);
    
    
    //add custom action
    AbstActionDescShrPtr
    actionDescription = addActionDescritionInstance<RTWorkerCU>(this,
                                                                &RTWorkerCU::actionTestOne,
                                                                "actionTestOne",
                                                                "comandTestOne this action will do some beautifull things!");
    
    actionDescription = addActionDescritionInstance<RTWorkerCU>(this,
                                                                &RTWorkerCU::resetStatistic,
                                                                "resetStatistic",
                                                                "resetStatistic this action will reset  all cu statistic!");
    
    actionDescription = addActionDescritionInstance<RTWorkerCU>(this,
                                                                &RTWorkerCU::actionTestTwo,
                                                                "actionTestTwo",
                                                                "comandTestTwo, this action will do some beautifull things!");
    
    //add param to second action
    actionDescription->addParam(ACTION_TWO_PARAM_NAME,
                                DataType::TYPE_INT32,
                                "integer 32bit action param description for testing purpose");
    
    
    //create the output attribute
    //    addAttributeToDataSet("sin_wave",
    //                          "The sin waveform",
    //                          DataType::TYPE_BYTEARRAY,
    //                          DataType::Output,
    //                          0);
    
    //    addBinaryAttributeAsSubtypeToDataSet("sin_wave",
    //                                         "The sin waveform",
    //                                         DataType::SUB_TYPE_DOUBLE,
    //                                         0,
    //                                         DataType::Output);
    
    //    std::vector<int32_t> sub_types;
    //    sub_types.push_back(DataType::SUB_TYPE_DOUBLE);
    //    sub_types.push_back(DataType::SUB_TYPE_INT16|DataType::SUB_TYPE_UNSIGNED);
    //    sub_types.push_back(DataType::SUB_TYPE_INT8);
    //    addBinaryAttributeAsSubtypeToDataSet("sin_wave",
    //                                         "The sin waveform",
    //                                         sub_types,
    //                                         0,
    //                                         DataType::Output);
    addBinaryAttributeAsSubtypeToDataSet("sin_wave",
                                         "The sin waveform",
                                         DataType::SUB_TYPE_DOUBLE,
                                         10000,
                                         DataType::Output);
    addAttributeToDataSet("run_counter",
                          "The number of run since last init phase",
                          DataType::TYPE_INT64,
                          DataType::Output);
    
    //create the input attribute
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
    addAttributeToDataSet("gain",
                          "The gain of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    addAttributeToDataSet("phase",
                          "The phase of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    addAttributeToDataSet("gain_noise",
                          "The gain of the noise of the wave",
                          DataType::TYPE_DOUBLE,
                          DataType::Input);
    addAttributeToDataSet("test_in_out",
                          "Bidirectional example",
                          DataType::TYPE_INT32,
                          DataType::Bidirectional);
    
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "points");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "frequency");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "bias");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "gain");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "phase");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "gain_noise");
    addVariantHandlerOnInputAttributeName<RTWorkerCU>(this,
                                                      &RTWorkerCU::variantHandler,
                                                      "test_in_out");
    
//    char * tmp = NULL;
//    memset(tmp, 0, 65000);
}

void RTWorkerCU::unitDefineCustomAttribute() {
    bool quit = false;
    //here are defined the custom shared variable
    getAttributeCache()->addCustomAttribute("quit", 1, chaos::DataType::TYPE_BOOLEAN);
    getAttributeCache()->setCustomAttributeValue("quit", &quit, sizeof(bool));
}

/*
 Initialize the Custom Contro Unit and return the configuration
 */
void RTWorkerCU::unitInit() throw(CException) {
    LAPP_ << "init RTWorkerCU";
    int err = 0;
    if(crash_location == 1) throw CException(-1, "Test Exception in init phase", __PRETTY_FUNCTION__);
    
    driver = getAccessoInstanceByIndex(0);
    CHECK_ASSERTION_THROW_AND_LOG((driver != NULL), ERR_LOG(RTWorkerCU), -2, CHAOS_FORMAT("Driver has not been allocated for CU %1%", %getCUID()));
    
    CHECK_ASSERTION_THROW_AND_LOG(((err = initGenerator()) == 0), ERR_LOG(RTWorkerCU), -2, CHAOS_FORMAT("Error %1% initilizing generator in cu %2%", %err%getCUID()));

    getAttributeCache()->getCachedOutputAttributeValue<uint64_t>(1, &out_run_counter);

    setGeneratorPoint(getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "points"));
    getAttributeCache()->resetChangedInputIndex();
    
    //reset counter
    (**out_run_counter) = 0;
}

/*
 Execute the work, this is called with a determinated delay, it must be as fast as possible
 */
void RTWorkerCU::unitStart() throw(CException) {
    if(crash_location == 2) throw CException(-1, "Test Exception in start phase", __PRETTY_FUNCTION__);
}

/*
 Execute the Control Unit work
 */
void RTWorkerCU::unitRun() throw(CException) {
    
    if((crash_location == 5) &&
       ((**out_run_counter) == crash_run_count) &&
       !crasch_occured) {
        crasch_occured = true;
        //we can throw the exception
        throw CException(-1, "Test Exception in run phase", __PRETTY_FUNCTION__);
    }
    (**out_run_counter)++;
    generateWave();
    getAttributeCache()->setOutputDomainAsChanged();
}

void  RTWorkerCU::unitInputAttributePreChangeHandler() throw(CException) {
    
}

//! changed attribute
void RTWorkerCU::unitInputAttributeChangedHandler() throw(CException) {
    //r_o_attr_lock->lock();
    
}

/*
 Execute the Control Unit work
 */
void RTWorkerCU::unitStop() throw(CException) {
    LAPP_ << "stop RTWorkerCU";
    if(crash_location == 3) throw CException(-1, "Test Exception in stop phase", __PRETTY_FUNCTION__);
}

/*
 Deinit the Control Unit
 */
void RTWorkerCU::unitDeinit() throw(CException) {
    LAPP_ << "deinit RTWorkerCU";
    if(crash_location == 3) throw CException(-1, "Test Exception in deinit phase", __PRETTY_FUNCTION__);
    
    if(driver){purgeGenerator();}
}

//! restore the control unit to snapshot
bool RTWorkerCU::unitRestoreToSnapshot(chaos::cu::control_manager::AbstractSharedDomainCache * const snapshot_cache) throw(CException) {
    return true;
}

/*
 Test Action Handler
 */
CDataWrapper* RTWorkerCU::actionTestOne(CDataWrapper *actionParam, bool& detachParam) {
    CDataWrapper *result = new CDataWrapper();
    static uint64_t counter = 0;
    result->addInt64Value("call_counter", counter++);
    LAPP_ << "call_counter = " << counter;
    return result;
}

/*
 Test Action Handler
 */
CDataWrapper* RTWorkerCU::resetStatistic(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in RTWorkerCU called from rpc";
    return NULL;
}

/*
 Test Action Handler
 */
CDataWrapper* RTWorkerCU::actionTestTwo(CDataWrapper *actionParam, bool& detachParam) {
    LAPP_ << "resetStatistic in RTWorkerCU called from rpc";
    if(actionParam->hasKey(ACTION_TWO_PARAM_NAME)){
        int32_t sleepTime =  actionParam->getInt32Value(ACTION_TWO_PARAM_NAME);
        
        LAPP_ << "param for actionTestTwo with value:" << sleepTime;
        LAPP_ << "let this thread to waith "<< sleepTime << " usec";
        boost::this_thread::sleep(boost::posix_time::microseconds(sleepTime));
    } else {
        LAPP_ << "No param received for action actionTestTwo";
    }
    return NULL;
}

bool RTWorkerCU::i32Handler(const std::string& attribute_name,
                            int32_t value,
                            uint32_t value_size) {
    LAPP_ << boost::str(boost::format("Handler for %1% received with value %2%")%attribute_name%value);
    if(attribute_name.compare("points") == 0) {
        //setWavePoint(value.asInt32());
        setGeneratorPoint(value);
    } else if(attribute_name.compare("test_in_out") == 0) {
        LAPP_ << "bidirectional attribute as input with value " << value;
    }
    return true;
}

bool RTWorkerCU::variantHandler(const std::string& attribute_name,
                                const chaos::common::data::CDataVariant& value) {
    LAPP_ << boost::str(boost::format("Handler variant for %1% received")%attribute_name);
    if(attribute_name.compare("points") == 0) {
        //setWavePoint(value.asInt32());
        setGeneratorPoint(value.asInt32());
    } else if(attribute_name.compare("test_in_out") == 0) {
        LAPP_ << "bidirectional attribute as input with value " << value.asInt32();
    }
    return true;
}

int RTWorkerCU::initGenerator() {
    DrvMsg cmd;
    cmd.opcode = OP_INIT_SIMULATION;
    cmd.resultData = &generation_data;
    driver->send(&cmd);
    return cmd.ret;
}

int RTWorkerCU::setGeneratorPoint(int32_t points) {
    DrvMsg cmd;
    cmd.opcode = OP_SET_POINTS;
    cmd.inputData = generation_data;
    
    generation_data->points = points;
    
    driver->send(&cmd);
    if(cmd.ret == 0) {
        getAttributeCache()->setOutputAttributeNewSize(0, generation_data->points * sizeof(double));
    }
    return cmd.ret;
}

int RTWorkerCU::generateWave() {
    DrvMsg cmd;
    cmd.opcode = OP_STEP_SIMULATION;
    cmd.inputData = generation_data;
    ChaosSharedPtr<SharedCacheLockDomain> r_lock = getAttributeCache()->getReadLockOnInputAttributeCache();
    r_lock->lock();
    
    generation_data->parameter[freq] = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "frequency");
    generation_data->parameter[bias] = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "bias");
    generation_data->parameter[gain] = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain");
    generation_data->parameter[phase] = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "phase");
    generation_data->parameter[gainNoise] = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain_noise");
    driver->send(&cmd);
    if(cmd.ret == 0) {
        double *cached_sin_value = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "sin_wave");
        std::memcpy(cached_sin_value, generation_data->data, sizeof(double)*generation_data->points);
    }
    return cmd.ret;
}

int RTWorkerCU::purgeGenerator() {
    DrvMsg cmd;
    cmd.opcode = OP_DESTROY_SIMULATION;
    cmd.inputData = generation_data;
    driver->send(&cmd);
    if(cmd.ret == 0) {
        generation_data = NULL;
    }
    return cmd.ret;
}
