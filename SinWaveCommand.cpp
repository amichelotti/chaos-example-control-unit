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

using namespace chaos::common::batch_command;

SinWaveCommand::SinWaveCommand():rng((const uint_fast32_t) time(0) ),one_to_hundred( -100, 100 ),randInt(rng, one_to_hundred) {
    //set default scheduler delay 50 milliseconds, the delay is expressed in microseconds
    setFeatures(features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY, (uint64_t)1000000);
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
    sin_wave = NULL;
	sin_wave_points = 0;
	//get read only input attribute
	attribute_cache.getReadonlyCachedAttributeValue<int32_t>(AttributeValueSharedCache::SVD_INPUT, 0, &in_points);
	attribute_cache.getReadonlyCachedAttributeValue<double>(AttributeValueSharedCache::SVD_INPUT, 1, &in_freq);
	attribute_cache.getReadonlyCachedAttributeValue<double>(AttributeValueSharedCache::SVD_INPUT, 2, &in_bias);
	attribute_cache.getReadonlyCachedAttributeValue<double>(AttributeValueSharedCache::SVD_INPUT, 3, &in_phase);
	attribute_cache.getReadonlyCachedAttributeValue<double>(AttributeValueSharedCache::SVD_INPUT, 4, &in_gain);
	attribute_cache.getReadonlyCachedAttributeValue<double>(AttributeValueSharedCache::SVD_INPUT, 5, &in_gain_noise);

	//get custom attribute
	attribute_cache.getCachedCustomAttributeValue<bool>(0, &custom_quit);
    
    lastStartTime = 0;
	
	//this is necessary becase if the command is installed after the first start the
	//attribute is not anymore marched has "changed". so in every case at set handler
	//we call the set point funciton that respect the actual value of poits pointer.
	setWavePoint();
	
    CMDCU_ << "SinWaveCommand::setHandler";
}

// Aquire the necessary data for the command
/*!
 The acquire handler has the purpose to get all necessary data need the by CC handler.
 \return the mask for the runnign state
 */
void SinWaveCommand::acquireHandler() {
    uint64_t timeDiff = getStartStepTime() - lastStartTime;
    
    if(timeDiff > 10000000 || (*custom_quit)) {
        //every ten seconds ste the state until reac the killable and
        //the return to exec
        lastStartTime = getLastStepTime();
        if(!(*custom_quit)) {
            switch (SlowCommand::getRunningProperty()) {
                case RunningPropertyType::RP_Exsc:
                    BC_NORMAL_RUNNIG_PROPERTY
                    CMDCU_ << "Change to SL_NORMAL_RUNNIG_STATE";
                    break;
                case RunningPropertyType::RP_Normal:
                    BC_EXEC_RUNNIG_PROPERTY
                    CMDCU_ << "Change to SL_EXEC_RUNNIG_STATE";
                    break;
            }
        } else {
            BC_END_RUNNIG_PROPERTY;
        }
    }
    
    //check if some parameter has changed every 100 msec
    if(timeDiff > 100) {
		attribute_cache.getChangedInputAttributeIndex(changedIndex);
        if(changedIndex.size()) {
            CMDCU_ << "We have " << changedIndex.size() << " changed attribute";
            for (int idx =0; idx < changedIndex.size(); idx++) {

                //the index is correlated to the creation sequence so
                //the index 0 is the first input parameter "frequency"
                switch (changedIndex[idx]) {
                    case 0://points

                        // apply the modification
                        setWavePoint();
                        break;
                    default:// all other parameter are managed into the function that create the sine waveform
                        break;
                }

            }
            changedIndex.clear();
        }
    }
    CDataWrapper *acquiredData = getNewDataWrapper();
    if(!acquiredData) return;
    
    //put the messageID for test the lost of package
    acquiredData->addInt32Value("id", ++messageID);
    computeWave(acquiredData);
}

// Correlation and commit phase
void SinWaveCommand::ccHandler() {
    
}

void SinWaveCommand::computeWave(CDataWrapper *acquiredData) {
    if(!sin_wave) return;
    double interval = (2*PI)/(ATTRIBUTE_HANDLE_GET_VALUE(in_points));
    boost::mutex::scoped_lock lock(pointChangeMutex);
    for(int i=0; i<ATTRIBUTE_HANDLE_GET_VALUE(in_points); i++){
        sin_wave[i] = (ATTRIBUTE_HANDLE_GET_VALUE(in_gain)*sin((interval*i)+ ATTRIBUTE_HANDLE_GET_VALUE(in_phase))+
					   (((double)randInt()/(double)100)* ATTRIBUTE_HANDLE_GET_VALUE(in_gain_noise))+ATTRIBUTE_HANDLE_GET_VALUE(in_bias));
    }
    acquiredData->addBinaryValue("sinWave", (char*)sin_wave, (int32_t)sizeof(double)*ATTRIBUTE_HANDLE_GET_VALUE(in_points));
}

/*
 */
void SinWaveCommand::setWavePoint() {
    boost::mutex::scoped_lock lock(pointChangeMutex);
    uint32_t tmpNOP = ATTRIBUTE_HANDLE_GET_VALUE(in_points);
    if(tmpNOP < 1) tmpNOP = 0;
    
    if(!tmpNOP){
        if(sin_wave){
            free(sin_wave);
            sin_wave = NULL;
        }
    }else{
        size_t byteSize = sizeof(double) * tmpNOP;
        double* tmp_ptr = (double*)realloc(sin_wave, byteSize);
        if(tmp_ptr) {
            sin_wave = tmp_ptr;
			sin_wave_points = tmpNOP;
            memset(sin_wave, 0, byteSize);
        }
    }
}