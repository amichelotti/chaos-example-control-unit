//
//  main.cpp
//  StressRpcTest
//
//  Created by Claudio Bisegni on 25/04/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <iostream>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

using namespace chaos;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::node_controller;

#define NUMBER_OF_ITERATION 1000000000

int main(int argc, char * argv[])
{
    try {
        std::string device_id;
        std::string action_name;
        uint32_t timeout = 0;
        uint32_t iteration = 0;
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>("device_id", "The identification string of the device to stress", &device_id);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<string>("action_name", "The name of cutom action to call", &action_name);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("timeout", "The timeout of rpc call", 1000, &timeout);
        ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>("iteration", "The numer of iteration to do", NUMBER_OF_ITERATION, &iteration);
        
        ChaosMetadataServiceClient::getInstance()->init(argc, argv);
        ChaosMetadataServiceClient::getInstance()->start();
        if(!ChaosMetadataServiceClient::getInstance()->getGlobalConfigurationInstance()->hasOption("device_id")) {
            throw chaos::CException(-1, "The device id is mandatory", "init");
        }
        
        //        for(int idx = 0; idx < 1000000; idx++) {
        //            DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(device_id, timeout);
        //            if (!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
        //            HLDataApi::getInstance()->disposeDeviceControllerPtr(controller);
        //        }
        
        
        //		for(int idx = 0; idx < iteration; idx++) {
        //            chaos::common::data::CDataWrapper test_data;
        //            ChaosUniquePtr<chaos::common::data::CDataWrapper> test_data_tmp;
        //            ChaosUniquePtr<chaos::common::data::CDataWrapper> test_data_constructor;
        //            chaos::common::data::CDataWrapper test_data_return;
        //
        //            test_data.addStringValue("string", "string");
        //            test_data.addInt32Value("i32", 0);
        //            test_data.addInt64Value("i64", (int64_t)1);
        //            test_data.addDoubleValue("double", 2.0);
        //            test_data_tmp.reset(test_data.clone());
        //            test_data.addCSDataValue("csvalue", *test_data_tmp);
        //            for(int idx = 0;idx < 100; idx++) {
        //                test_data.appendCDataWrapperToArray(*test_data_tmp);
        //            }
        //            test_data.finalizeArrayForKey("csvalue_array");
        //            test_data_tmp.reset(test_data.getCSDataValue("csvalue"));
        //            ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> arr(test_data.getVectorValue("csvalue_array"));
        //            for(int idx = 0; idx < arr->size(); idx++) {
        //                ChaosUniquePtr<chaos::common::data::CDataWrapper> ele(arr->getCDataWrapperElementAtIndex(idx));
        //            }
        //            ChaosUniquePtr<chaos::common::data::SerializationBuffer> ser(test_data.getBSONData());
        //            test_data.copyAllTo(test_data_return);
        //
        //            //test_data_constructor.reset(new chaos::common::data::CDataWrapper(test_data.getBSONRawData()));
        //			//controller->echoTest(&test_data, &test_data_return);
        //			//if(idx && ((idx % 100) == 0)) {
        //            //    if(test_data_return){std::cout<<idx<<std::endl;}
        //			//}
        //
        //            //delete(test_data_return);
        //		}
        CUController *controller = NULL;
        ChaosMetadataServiceClient::getInstance()->getNewCUController(device_id, &controller);
        CHAOS_ASSERT(controller);
        for(int idx = 0; idx < iteration; idx++) {
            chaos::common::data::CDataWrapper test_data;
            chaos::common::data::CDataWrapper *test_data_return;
            test_data.addStringValue("string", "string");
            test_data.addInt32Value("coutner", idx);
            controller->echoTest(&test_data, &test_data_return);
            if(idx && ((idx % 100) == 0)) {
                if(test_data_return){std::cout<<idx<<std::endl;}
            }
            
            delete(test_data_return);
            usleep(1000000);
        }
        LAPP_ << "Message sent: " << iteration;
        ChaosMetadataServiceClient::getInstance()->stop();
        ChaosMetadataServiceClient::getInstance()->deinit();
    } catch (CException& e) {
        std::cerr << e.errorCode << " - " << e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    
    return 0;
}

