/*
 *	SinGeneratorTypes.h
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

#ifndef __ExampleControlUnit__5A9C28A_4C54_496E_8713_B4E2B0098302_SinGeneratorTypes_h
#define __ExampleControlUnit__5A9C28A_4C54_496E_8713_B4E2B0098302_SinGeneratorTypes_h

#include <stdint.h>
#include <chaos/cu_toolkit/driver_manager/driver/DriverTypes.h>
typedef enum {
    //! init the simulation with an id and the return value of the user need to be an handler to the SinData struct
    OP_INIT_SIMULATION = chaos::cu::driver_manager::driver::OpcodeType::OP_USER,
    OP_SET_POINTS, //set the number of points of the simulation
    /*!
     freq|bias|phase|gain|gainNoise
     */
    OP_SET_SIMULATION_PARAMETER,
    OP_STEP_SIMULATION,
    OP_DESTROY_SIMULATION
} SinGeneratorDriverOpcode;

typedef double SimulParameter[5];

typedef struct SinGeneratorData {
    int gen_id;
    double *data;
    uint32_t points;
    SimulParameter parameter;
    SinGeneratorData():
    gen_id(0),
    points(0){memset(&parameter, 0, sizeof(SimulParameter));}
    ~SinGeneratorData(){free(data);}
} SinGeneratorData;

typedef enum SinGeneratorParameter {
    freq,
    bias,
    phase,
    gain,
    gainNoise
} SinGeneratorParameter;

#endif /* __ExampleControlUnit__5A9C28A_4C54_496E_8713_B4E2B0098302_SinGeneratorTypes_h */
