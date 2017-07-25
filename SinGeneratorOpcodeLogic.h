/*
 *	SinGeneratoOpcodeLogic.h
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

#ifndef __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_SinGeneratorOpcodeLogic_h
#define __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_SinGeneratorOpcodeLogic_h

#include "SinGeneratorTypes.h"
#include <chaos/cu_toolkit/driver_manager/driver/OpcodeExternalCommandMapper.h>

class SinGeneratorOpcodeLogic:
public chaos::cu::driver_manager::driver::OpcodeExternalCommandMapper {
protected:
    void initSimulation(int simulation_id, SinGeneratorData **data);
    void setSimulationsPoints(SinGeneratorData *sin_data);
    void computeSimulation(SinGeneratorData *sin_data);
public:
    SinGeneratorOpcodeLogic(chaos::cu::driver_manager::driver::AbstractRemoteIODriver *_remote_driver);
    ~SinGeneratorOpcodeLogic();
    
    chaos::cu::driver_manager::driver::MsgManagmentResultType::MsgManagmentResult execOpcode(chaos::cu::driver_manager::driver::DrvMsgPtr cmd);
};

#endif /* __ExampleControlUnit__DAB3EB0_DEBD_47DC_8DB8_05E1E8BB501E_SinGeneratoOpcodeLogic_h */
