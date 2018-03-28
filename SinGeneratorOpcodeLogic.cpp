/*
 *	SinGeneratorOpcodeLogic.cpp
 *
 *	!CHAOS [ExampleControlUnit]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/2017 INFN, National Institute of Nuclear Physics
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

#include "SinGeneratorOpcodeLogic.h"

#include <chaos/common/global.h>
#include <chaos/common/additional_lib/base64.h>
#define INFO INFO_LOG(SinGeneratorOpcodeLogic)
#define ERR ERR_LOG(SinGeneratorOpcodeLogic)
#define DBG DBG_LOG(SinGeneratorOpcodeLogic)

using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;
SinGeneratorOpcodeLogic::SinGeneratorOpcodeLogic(chaos::cu::driver_manager::driver::RemoteIODriverProtocol *_remote_driver):
OpcodeExternalCommandMapper(_remote_driver),
counter(0){}

SinGeneratorOpcodeLogic::~SinGeneratorOpcodeLogic() {}

void SinGeneratorOpcodeLogic::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
    INFO << init_parameter.getJSONString();
}

void SinGeneratorOpcodeLogic::driverDeinit() throw(chaos::CException) {}

int SinGeneratorOpcodeLogic::initSimulation(SinGeneratorData **data) {
    LSinGenMapWriteLock wl = generator_map.getWriteLockObject();
    ChaosSharedPtr<SinGeneratorData> new_generator(*data = new SinGeneratorData());
    std::memset(new_generator.get(), 0, sizeof(SinGeneratorData));
    new_generator->gen_id = counter++;
    generator_map().insert(SinGenMapPair(new_generator->gen_id, new_generator));
    return 0;
}

int SinGeneratorOpcodeLogic::setSimulationsPoints(SinGeneratorData *sin_data) {
    LSinGenMapReadLock wl = generator_map.getReadLockObject();
    if(generator_map().count(sin_data->gen_id) == 0) return -1;
    if(!sin_data->points){
        if(sin_data->data){
            free(sin_data->data);
            sin_data->data = NULL;
        }
    }else{
        uint64_t byte_size = sizeof(double) * sin_data->points;
        sin_data->data = (double*)realloc(sin_data->data, byte_size);
        if(sin_data->data) {
            memset(sin_data->data, 0, byte_size);
        }
    }
    return 0;
}

int SinGeneratorOpcodeLogic::computeSimulation(SinGeneratorData *sin_data) {
    int err = 0;
    LSinGenMapReadLock wl = generator_map.getReadLockObject();
    if(generator_map().count(sin_data->gen_id) == 0) return -1;
    CDWUniquePtr request(new CDataWrapper());
    CDWShrdPtr response;
    request->addInt32Value("opcode", OP_STEP_SIMULATION);
    request->addInt32Value("points", sin_data->points);
    request->addDoubleValue("freq", sin_data->parameter[freq]);
    request->addDoubleValue("phase", sin_data->parameter[phase]);
    request->addDoubleValue("gain", sin_data->parameter[gain]);
    request->addDoubleValue("gainNoise", sin_data->parameter[gainNoise]);
    request->addDoubleValue("bias", sin_data->parameter[bias]);
    
    if((err = sendOpcodeRequest("gen_sin",
                                ChaosMoveOperator(request),
                                response))) {
        ERR << "error receiving ack for step simulation with code" << err;
        return err;
    } else {
        INFO << response->getCompliantJSONString();
        if(response->hasKey("err") == false){
            return -2;
        } else {
            int opcode_err = response->getInt32Value("err");
            if(opcode_err) return opcode_err;
            uint32_t bin_size = 0;
            const char * bin_value = response->getBinaryValue("sin_wave", bin_size);
            std::memcpy(sin_data->data, bin_value, bin_size);
        }
    }
    return 0;
}

int SinGeneratorOpcodeLogic::destroySimulation(SinGeneratorData *sin_data) {
    LSinGenMapWriteLock wl = generator_map.getWriteLockObject();
    generator_map().erase(sin_data->gen_id);
    return 0;
}

int SinGeneratorOpcodeLogic::asyncMessageReceived(CDWUniquePtr message) {
    INFO << message->getJSONString();
    return 0;
}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult SinGeneratorOpcodeLogic::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case OP_INIT_SIMULATION: {
            cmd->ret = initSimulation(static_cast<SinGeneratorData**>(cmd->resultData));
            break;
        }
            
        case OP_SET_POINTS: {
            cmd->ret = setSimulationsPoints(static_cast<SinGeneratorData*>(cmd->inputData));
            break;
        }
            
        case OP_STEP_SIMULATION: {
            cmd->ret = computeSimulation(static_cast<SinGeneratorData*>(cmd->inputData));
            break;
        }
            
        case OP_DESTROY_SIMULATION: {
            cmd->ret = destroySimulation(static_cast<SinGeneratorData*>(cmd->inputData));
            break;
        }
    }
    return result;
}
