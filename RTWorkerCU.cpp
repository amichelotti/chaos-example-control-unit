
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
#include <chaos/common/bson/bson.h>
#include <chaos/common/bson/util/hex.h>
#include <chaos/common/action/ActionDescriptor.h>

#include <boost/lexical_cast.hpp>

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
//instance variabl einizialization
rng((const uint_fast32_t) time(0) ),
one_to_hundred( -100, 100 ),
randInt(rng, one_to_hundred),
out_sin_value_points(0) {
    numberOfResponse = 0;
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
    //cuSetup.addStringValue(CUDefinitionKey::CS_CM_CU_DESCRIPTION, "This is a beautifull CU");
    
    
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
    addAttributeToDataSet("sinWave",
                          "The sin waveform",
                          DataType::TYPE_BYTEARRAY,
                          DataType::Output,
                          1);
	
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
	
	//get the defual value of the number of point
	RangeValueInfo attributeInfo;
	getAttributeRangeValueInfo("points", attributeInfo);
    //RTAbstractControlUnit::init();
    
    initTime = steady_clock::now();
    lastExecutionTime = steady_clock::now();
    numberOfResponse = 0;
    srand((unsigned)time(0));
    PI = acos((long double) -1);
    messageID = 0;
	out_sin_value_points = 0;

	//get handle to the output attribute value
	getAttributeCache()->getCachedOutputAttributeValue<double>(0, &out_sin_value);
	
	//get handle to the input attribute value
	getAttributeCache()->getReadonlyCachedAttributeValue<int32_t>(DOMAIN_INPUT, 0, &in_points);
	getAttributeCache()->getReadonlyCachedAttributeValue<double>(DOMAIN_INPUT, 1, &in_freq);
	getAttributeCache()->getReadonlyCachedAttributeValue<double>(DOMAIN_INPUT, 2, &in_bias);
	getAttributeCache()->getReadonlyCachedAttributeValue<double>(DOMAIN_INPUT, 3, &in_gain);
	getAttributeCache()->getReadonlyCachedAttributeValue<double>(DOMAIN_INPUT, 4, &in_phase);
	getAttributeCache()->getReadonlyCachedAttributeValue<double>(DOMAIN_INPUT, 5, &in_gain_noise);

	setWavePoint();
	getAttributeCache()->resetChangedInputIndex();
	
	r_o_attr_lock = getAttributeCache()->getLockOnOutputAttributeCache(true);
}

/*
 Execute the work, this is called with a determinated delay, it must be as fast as possible
 */
void RTWorkerCU::unitStart() throw(CException) {
   // RTAbstractControlUnit::start();
}

/*
 Execute the Control Unit work
 */
void RTWorkerCU::unitRun() throw(CException) {
	boost::shared_ptr<SharedCacheLockDomain> r_lock = getAttributeCache()->getReadLockOnInputAttributeCache();
	r_lock->lock();
	double *cached_sin_value = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "sinWave");
	int32_t cached_points = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "points");
	double cached_frequency = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "frequency");
	double cached_bias = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "bias");
	double cached_gain = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain");
	double cached_phase = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "phase");
	double cached_gain_noise = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain_noise");

	/*if(ATTRIBUTE_HANDLE_GET_PTR(out_sin_value) == NULL) return;
	double interval = (2*PI)/ATTRIBUTE_HANDLE_GET_VALUE(in_points);
	for(int i=0; i<ATTRIBUTE_HANDLE_GET_VALUE(in_points); i++){
		double * ptr = ATTRIBUTE_HANDLE_GET_PTR(out_sin_value);
		double sin_point = sin((interval*i)+ATTRIBUTE_HANDLE_GET_VALUE(in_phase));
		double sin_point_rumor = (((double)randInt()/(double)100)*ATTRIBUTE_HANDLE_GET_VALUE(in_gain_noise));
		ptr[i] = ((**in_gain) * sin_point) + sin_point_rumor + ATTRIBUTE_HANDLE_GET_VALUE(in_bias);
	}*/
	if(cached_sin_value == NULL) return;
	double interval = (2*PI)/cached_points;
	for(int i=0; i<cached_points; i++){
		double sin_point = sin((interval*i*cached_frequency) + cached_phase);
		double sin_point_rumor = (((double)randInt()/(double)100) * cached_gain_noise);
		cached_sin_value[i] = (cached_gain * sin_point) + sin_point_rumor + cached_bias;
	}
	getAttributeCache()->setOutputDomainAsChanged();
}

void  RTWorkerCU::unitInputAttributePreChangeHandler() throw(CException) {
	//r_o_attr_lock->lock();
}

//! changed attribute
void RTWorkerCU::unitInputAttributeChangedHandler() throw(CException) {

	
	
}

/*
 Execute the Control Unit work
 */
void RTWorkerCU::unitStop() throw(CException) {
    LAPP_ << "stop RTWorkerCU";
    //RTAbstractControlUnit::stop();
}

/*
 Deinit the Control Unit
 */
void RTWorkerCU::unitDeinit() throw(CException) {
    LAPP_ << "deinit RTWorkerCU";
}

//! restore the control unit to snapshot
void RTWorkerCU::unitRestoreToSnapshot(const std::string& restore_snapshot_tag,
									   chaos::cu::control_manager::AbstractSharedDomainCache * const restore_cache) throw(CException) {
	LAPP_ << "resto to snapshot " << restore_snapshot_tag;
}

/*
 */
void RTWorkerCU::setWavePoint() {
	int32_t tmpNOP = ATTRIBUTE_HANDLE_GET_VALUE(in_points);
    if(tmpNOP < 1) tmpNOP = 0;
	if(tmpNOP == out_sin_value_points) return;
	
    if(!tmpNOP){
		//no wero point allowed
		return;
    }else{
        uint32_t byte_size = uint32_t(sizeof(double) * tmpNOP);
		if(getAttributeCache()->setOutputAttributeNewSize(0, byte_size)) {
			out_sin_value_points = tmpNOP;
		}
		
    }
	
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
    numberOfResponse = 0;
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
