/*
 *	SinWaveCommand.h
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
#include "SinWaveCommand.h"
#include <boost/lexical_cast.hpp>

#define CMDCU_ LAPP_ << "[SinWaveCommand] - " << getDeviceID() << " - "


using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cu::control_manager;

using namespace chaos::common::batch_command;

BATCH_COMMAND_OPEN_DESCRIPTION(,
                               SinWaveCommand,
                               "Default method for sinusoidal signal generation",
                               "2A3C9510-3C76-4B5A-89EE-4D1D5AD29D95")
BATCH_COMMAND_CLOSE_DESCRIPTION()

SinWaveCommand::SinWaveCommand():
rng((const uint_fast32_t) time(0) ),
one_to_hundred( -100, 100 ),
randInt(rng, one_to_hundred) {
    //boost::shared_ptr<chaos::common::data::CDataWrapper>  a = BATCH_COMMAND_GET_DESCRIPTION(SinWaveCommand);
    //set default scheduler delay 50 milliseconds, the delay is expressed in microseconds
    
}

SinWaveCommand::~SinWaveCommand() {
    
}

// return the implemented handler
uint8_t SinWaveCommand::implementedHandler() {
    return  HandlerType::HT_Set |
    HandlerType::HT_Acquisition |
    HandlerType::HT_Correlation;
}

// Start the command execution
void SinWaveCommand::setHandler(CDataWrapper *data) {
    // chaos::cu::DeviceSchemaDB *deviceDB = NULL;
    messageID = 0;
    
    srand((unsigned)time(0));
    PI = acos((long double) -1);
    out_sin_value_points = 0;
    lastStartTime = 0;
    uint64_t *cached_run_counter = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "run_counter");
    (*cached_run_counter) = 0;
    //this is necessary becase if the command is installed after the first start the
    //attribute is not anymore marched has "changed". so in every case at set handler
    //we call the set point funciton that respect the actual value of poits pointer.
    setWavePoint();
    
    setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
    
    //print the default value
    CMDCU_ << "SinWaveCommand::setHandler";
}

// Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void SinWaveCommand::acquireHandler() {
    bool tmp_value = false;
    
    double *cached_sin_value = getAttributeCache()->getRWPtr<double>(DOMAIN_OUTPUT, "sin_wave");
    uint64_t *cached_run_counter = getAttributeCache()->getRWPtr<uint64_t>(DOMAIN_OUTPUT, "run_counter");
    double cached_frequency = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "frequency");
    double cached_bias = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "bias");
    double cached_gain = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain");
    double cached_phase = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "phase");
    double cached_gain_noise = getAttributeCache()->getValue<double>(DOMAIN_INPUT, "gain_noise");
    
    (*cached_run_counter)++;
    
    double interval = (2*PI)/out_sin_value_points;
    for(int i=0; i<out_sin_value_points; i++){
        double sin_point = sin((interval*i*cached_frequency) + cached_phase);
        double sin_point_rumor = (((double)randInt()/(double)100) * cached_gain_noise);
        cached_sin_value[i] = (cached_gain * sin_point) + sin_point_rumor + cached_bias;
    }
    getAttributeCache()->setOutputDomainAsChanged();
}

// Correlation and commit phase
void SinWaveCommand::ccHandler() {
    uint64_t timeDiff = getStartStepTime() - lastStartTime;
    bool cached_quit = getAttributeCache()->getValue<bool>(DOMAIN_CUSTOM, "quit");
    
    if(timeDiff > 10000 || cached_quit) {
        //every ten seconds ste the state until reac the killable and
        //the return to exec
        lastStartTime = getStartStepTime();
        if(!cached_quit) {
            switch (SlowCommand::getRunningProperty()) {
                case RunningPropertyType::RP_EXSC:
                    BC_NORMAL_RUNNING_PROPERTY
                    CMDCU_ << "Change to SL_NORMAL_RUNNIG_STATE";
                    break;
                case RunningPropertyType::RP_NORMAL:
                    BC_EXCLUSIVE_RUNNING_PROPERTY
                    CMDCU_ << "Change to SL_EXEC_RUNNIG_STATE";
                    break;
            }
        } else {
            BC_END_RUNNING_PROPERTY;
        }
    }
    
    //check if some parameter has changed every 100 msec
    if(timeDiff > 100) {
        ChaosSharedPtr<SharedCacheLockDomain> r_lock = getAttributeCache()->getReadLockOnInputAttributeCache();
        r_lock->lock();
        
        std::vector<VariableIndexType> changed_input_attribute;
        getAttributeCache()->getChangedInputAttributeIndex(changed_input_attribute);
        if(changed_input_attribute.size()) {
            CMDCU_ << "We have " << changed_input_attribute.size() << " changed attribute";
            for (int idx =0; idx < changed_input_attribute.size(); idx++) {
                
                //the index is correlated to the creation sequence so
                //the index 0 is the first input parameter "frequency"
                switch (changed_input_attribute[idx]) {
                    case 0://points
                        
                        // apply the modification
                        setWavePoint();
                        break;
                    default:// all other parameter are managed into the function that create the sine waveform
                        break;
                }
                
            }
            getAttributeCache()->resetChangedInputIndex();
        }
    }
}

/*
 */
void SinWaveCommand::setWavePoint() {
    int32_t cached_points = getAttributeCache()->getValue<int32_t>(DOMAIN_INPUT, "points");
    if(cached_points < 1) cached_points = 0;
    if(cached_points == out_sin_value_points) return;
    
    if(!cached_points){
        //no wero point allowed
    }else{
        uint32_t byte_size = uint32_t(sizeof(double) * cached_points);
        if(getAttributeCache()->setOutputAttributeNewSize(0, byte_size)) {
            out_sin_value_points = cached_points;
        }
        
    }
}
