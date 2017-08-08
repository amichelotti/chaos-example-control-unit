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

#define INFO INFO_LOG(SinGeneratorOpcodeLogic)
#define ERR ERR_LOG(SinGeneratorOpcodeLogic)
#define DBG DBG_LOG(SinGeneratorOpcodeLogic)

using namespace chaos::common::data;
using namespace chaos::cu::driver_manager::driver;
SinGeneratorOpcodeLogic::SinGeneratorOpcodeLogic(chaos::cu::driver_manager::driver::AbstractRemoteIODriver *_remote_driver):
OpcodeExternalCommandMapper(_remote_driver){
    //permit only one connection to the driver
    setNumberOfMaxConnection(1);
}

SinGeneratorOpcodeLogic::~SinGeneratorOpcodeLogic() {}

int SinGeneratorOpcodeLogic::initSimulation(SinGeneratorData **data) {
    CDWUniquePtr request(new CDataWrapper());
    CDWShrdPtr response;
    request->addInt32Value("opcode", OP_INIT_SIMULATION);
    if(sendRawRequest(ChaosMoveOperator(request),
                      response)) {
        ERR << "error receiving initilization response from remote server";
        return -1;
    } else if(response->hasKey("gen_id") == false){
        return -2;
    } else {
        ChaosSharedPtr<SinGeneratorData> new_generator(*data = new SinGeneratorData());
        new_generator->gen_id = response->getInt32Value("gen_id");
        generator_map().insert(SinGenMapPair(new_generator->gen_id, new_generator));
    }
    return 0;
}

int SinGeneratorOpcodeLogic::setSimulationsPoints(SinGeneratorData *sin_data) {
    return 0;
}

int SinGeneratorOpcodeLogic::computeSimulation(SinGeneratorData *sin_data) {
    return 0;
}

int SinGeneratorOpcodeLogic::destroySimulation(SinGeneratorData *sin_data) {
    return 0;
}

int SinGeneratorOpcodeLogic::asyncMessageReceived(CDWUniquePtr message) {
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
