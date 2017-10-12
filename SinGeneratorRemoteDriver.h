/*
 *	SinGeneratorRemoteDriver.h
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

#ifndef __ExampleControlUnit__D6576ED_8DAA_4864_8638_2636B614F9F5_SinGeneratorRemoteDriver_h
#define __ExampleControlUnit__D6576ED_8DAA_4864_8638_2636B614F9F5_SinGeneratorRemoteDriver_h

#include "SinGeneratorOpcodeLogic.h"
#include <chaos/cu_toolkit/driver_manager/driver/OpcodeDriverWrapper.h>
#include <chaos/cu_toolkit/driver_manager/driver/AbstractServerRemoteIODriver.h>

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(SinGeneratorRemoteDriver)

class SinGeneratorRemoteDriver:
public chaos::cu::driver_manager::driver::OpcodeDriverWrapper<SinGeneratorOpcodeLogic, chaos::cu::driver_manager::driver::AbstractServerRemoteIODriver> {};
#endif /* __ExampleControlUnit__D6576ED_8DAA_4864_8638_2636B614F9F5_SinGeneratorRemoteDriver_h */
