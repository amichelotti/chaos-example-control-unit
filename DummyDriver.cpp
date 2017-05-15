/*
 *	DummyDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#include "DummyDriver.h"

#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/regex.hpp>

namespace cu_driver = chaos::cu::driver_manager::driver;

#define SL7DRVLAPP_		LAPP_ << "[DummyDriver] "
#define SL7DRVLDBG_		LDBG_ << "[DummyDriver] "
#define SL7DRVLERR_		LERR_ << "[DummyDriver] "


//! Regular expression for check server hostname and port
static const boost::regex PlcHostNameAndPort("([a-zA-Z0-9]+(.[a-zA-Z0-9]+)+):([0-9]{3,5})");
//! Regular expression for check server ip and port
static const boost::regex PlcIpAnPort("(\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b):([0-9]{4,5})");


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(DummyDriver, 1.0.0, DummyDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(Sl7Drv,http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(DummyDriver),
    rng((const uint_fast32_t) time(0) ),
    one_to_hundred( -100, 100 ),
    randInt(rng, one_to_hundred) {}

//default descrutcor
DummyDriver::~DummyDriver() {
	
}

void DummyDriver::driverInit(const char *initParameter) throw(chaos::CException) {
	SL7DRVLAPP_ << "Init dummy driver";
    srand((unsigned)time(0));
    PI = acos((long double) -1);
}

void DummyDriver::driverDeinit() throw(chaos::CException) {
	SL7DRVLAPP_ << "Deinit dummy driver";

}

void DummyDriver::initSimulation(int simulation_id, SinData **data) {
    if(simulations.count(simulation_id) > 0) return;
    
    (*data) = new SinData();
    (*data)->id = simulation_id;
    (*data)->data = NULL;
    (*data)->points = 0;
    memset((*data)->parameter, 0, sizeof(SimulParameter));
    
    //insert into the hash
    simulations.insert(std::make_pair(simulation_id, *data));
}

void DummyDriver::setSimulationsPoints(SinData *sin_data) {
    if(simulations.count(sin_data->id) <= 0) return;
    
    SinData *w_data = simulations[sin_data->id];
    
    if(!w_data->points){
        if(w_data->data){
            free(w_data->data);
            w_data->data = NULL;
        }
    }else{
        uint64_t byte_size = sizeof(double) * w_data->points;
        double* tmpPtr = (double*)realloc(w_data->data, byte_size);
        if(tmpPtr) {
            w_data->data = tmpPtr;
            memset(w_data->data, 0, byte_size);
        }
    }
}

void DummyDriver::computeSimulation(SinData *sin_data) {
    if(simulations.count(sin_data->id) <= 0) return;
    
    SinData *w_data = simulations[sin_data->id];
    double interval = (2*PI)/(w_data->points);
    for(int i=0; i<w_data->points; i++){
        w_data->data[i] = ((w_data->parameter[gain])*sin((interval*i)+(w_data->parameter[phase]))+(((double)randInt()/(double)100)*(w_data->parameter[gainNoise]))+(w_data->parameter[bias]));
    }
}

//! Execute a command
cu_driver::MsgManagmentResultType::MsgManagmentResult DummyDriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
	cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case OP_INIT_SIMULATION: {
            int *id = static_cast<int*>(cmd->inputData);
            SinData *user_sin_data = static_cast<SinData*>(cmd->resultData);
            initSimulation(*id, &user_sin_data);
            break;
        }
            
        case OP_SET_POINTS: {
            SinData *sin_data = static_cast<SinData*>(cmd->inputData);
            setSimulationsPoints(sin_data);
            computeSimulation(sin_data);
            break;
        }
            
        case OP_STEP_SIMULATION: {
            SinData *sin_data = static_cast<SinData*>(cmd->inputData);
            computeSimulation(sin_data);
            break;
        }
            
        case OP_GET_SIN_DATA: {
            break;
        }
    }
	return result;
}
