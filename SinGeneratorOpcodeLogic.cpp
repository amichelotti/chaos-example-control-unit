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

using namespace chaos::cu::driver_manager::driver;
SinGeneratorOpcodeLogic::SinGeneratorOpcodeLogic(chaos::cu::driver_manager::driver::AbstractRemoteIODriver *_remote_driver):
OpcodeExternalCommandMapper(_remote_driver){}

SinGeneratorOpcodeLogic::~SinGeneratorOpcodeLogic() {}

void SinGeneratorOpcodeLogic::initSimulation(int simulation_id, SinGeneratorData **data) {
    
}

void SinGeneratorOpcodeLogic::setSimulationsPoints(SinGeneratorData *sin_data) {
    
}

void SinGeneratorOpcodeLogic::computeSimulation(SinGeneratorData *sin_data) {
    
}

//! Execute a command
MsgManagmentResultType::MsgManagmentResult SinGeneratorOpcodeLogic::execOpcode(DrvMsgPtr cmd) {
    MsgManagmentResultType::MsgManagmentResult result = MsgManagmentResultType::MMR_EXECUTED;
    switch(cmd->opcode) {
        case OP_INIT_SIMULATION: {
            int *id = static_cast<int*>(cmd->inputData);
            SinGeneratorData *user_sin_data = static_cast<SinGeneratorData*>(cmd->resultData);
            initSimulation(*id, &user_sin_data);
            break;
        }
            
        case OP_SET_POINTS: {
            SinGeneratorData *sin_data = static_cast<SinGeneratorData*>(cmd->inputData);
            setSimulationsPoints(sin_data);
            break;
        }
            
        case OP_STEP_SIMULATION: {
            SinGeneratorData *sin_data = static_cast<SinGeneratorData*>(cmd->inputData);
            computeSimulation(sin_data);
            break;
        }
            
        case OP_GET_SIN_DATA: {
            break;
        }
    }
    return result;
}
