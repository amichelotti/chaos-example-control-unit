/*
 *	SinGeneratorDriver.h
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
#include "SinGeneratorDriver.h"

#include <string>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <regex>

namespace cu_driver = chaos::cu::driver_manager::driver;

#define SL7DRVLAPP_		LAPP_ << "[SinGeneratorDriver] "
#define SL7DRVLDBG_		LDBG_ << "[SinGeneratorDriver] "
#define SL7DRVLERR_		LERR_ << "[SinGeneratorDriver] "


//! Regular expression for check server hostname and port
static const std::regex PlcHostNameAndPort("([a-zA-Z0-9]+(.[a-zA-Z0-9]+)+):([0-9]{3,5})");
//! Regular expression for check server ip and port
static const std::regex PlcIpAnPort("(\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b):([0-9]{4,5})");


//GET_PLUGIN_CLASS_DEFINITION
//we need only to define the driver because we don't are makeing a plugin
OPEN_CU_DRIVER_PLUGIN_CLASS_DEFINITION(SinGeneratorDriver, 1.0.0, SinGeneratorDriver)
REGISTER_CU_DRIVER_PLUGIN_CLASS_INIT_ATTRIBUTE(SinGeneratorDriver,http_address/dnsname:port)
CLOSE_CU_DRIVER_PLUGIN_CLASS_DEFINITION

//default constructor definition
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(SinGeneratorDriver),
rng((const uint_fast32_t) time(0) ),
one_to_hundred( -100, 100 ),
randInt(rng, one_to_hundred),
generator_id(0) {}

//default descrutcor
SinGeneratorDriver::~SinGeneratorDriver(){}

void SinGeneratorDriver::driverInit(const char *initParameter)  {
    SL7DRVLAPP_ << "Init dummy driver";
    srand((unsigned)time(0));
    PI = acos((long double) -1);
}

void SinGeneratorDriver::driverDeinit()  {
    SL7DRVLAPP_ << "Deinit dummy driver";
}

int SinGeneratorDriver::initSimulation(SinGeneratorData **data) {
    
    ChaosSharedPtr<SinGeneratorData> sin_data = ChaosMakeSharedPtr<SinGeneratorData>();
    sin_data->gen_id = generator_id++;
    sin_data->data = NULL;
    sin_data->points = 0;
    memset(sin_data->parameter, 0, sizeof(SimulParameter));
    
    //insert into the hash
    LSinGenMapWriteLock wl = simulations.getWriteLockObject();
    simulations().insert(std::make_pair(sin_data->gen_id, sin_data));
    *data = sin_data.get();
    return 0;
}

int SinGeneratorDriver::setSimulationsPoints(SinGeneratorData *sin_data) {
    LSinGenMapReadLock wl = simulations.getReadLockObject();
    if(!sin_data->points){
        if(sin_data->data){
            free(sin_data->data);
            sin_data->data = NULL;
        }
    }else{
        uint64_t byte_size = sizeof(double) * sin_data->points;
        double* tmpPtr = (double*)realloc(sin_data->data, byte_size);
        if(tmpPtr) {
            sin_data->data = tmpPtr;
            memset(sin_data->data, 0, byte_size);
        }
    }
    return 0;
}

int SinGeneratorDriver::computeSimulation(SinGeneratorData *sin_data) {
    LSinGenMapReadLock wl = simulations.getReadLockObject();
    double interval = (2*PI)/(sin_data->points);
//    for(int i=0; i<sin_data->points; i++){
//        sin_data->data[i] = ((sin_data->parameter[gain])*sin((interval*i)+(sin_data->parameter[phase]))+(((double)randInt()/(double)100)*(sin_data->parameter[gainNoise]))+(sin_data->parameter[bias]));
//    }
//    
    for(int i=0; i<sin_data->points; i++){
        double sin_point = sin((interval*i*sin_data->parameter[freq]) + sin_data->parameter[phase]);
        double sin_point_rumor = (((double)randInt()/(double)100) * sin_data->parameter[gainNoise]);
        sin_data->data[i] = (sin_data->parameter[gain] * sin_point) + sin_point_rumor + sin_data->parameter[bias];
    }
    return 0;
}

int SinGeneratorDriver::destroySimulation(SinGeneratorData *sin_data) {
    LSinGenMapWriteLock wl = simulations.getWriteLockObject();
    simulations().erase(sin_data->gen_id);
    return 0;
}

//! Execute a command
cu_driver::MsgManagmentResultType::MsgManagmentResult SinGeneratorDriver::execOpcode(cu_driver::DrvMsgPtr cmd) {
    cu_driver::MsgManagmentResultType::MsgManagmentResult result = cu_driver::MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case OP_INIT_SIMULATION: {
            SinGeneratorData **user_sin_data = static_cast<SinGeneratorData**>(cmd->resultData);
            cmd->ret = initSimulation(user_sin_data);
            break;
        }
            
        case OP_SET_POINTS: {
            SinGeneratorData *sin_data = static_cast<SinGeneratorData*>(cmd->inputData);
            cmd->ret = setSimulationsPoints(sin_data);
            break;
        }
            
        case OP_STEP_SIMULATION: {
            SinGeneratorData *sin_data = static_cast<SinGeneratorData*>(cmd->inputData);
            cmd->ret = computeSimulation(sin_data);
            break;
        }
            
        case OP_DESTROY_SIMULATION: {
            cmd->ret = destroySimulation( static_cast<SinGeneratorData*>(cmd->inputData));
            break;
        }
    }
    return result;
}
