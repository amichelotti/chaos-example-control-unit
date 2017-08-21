//
//  main.cpp
//  SinGeneratorExternalDriver
//
//  Created by bisegni on 08/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>

#include <chaos_micro_unit_toolkit/micro_unit_toolkit.h>

#include <boost/random.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

int event_handler(void *user_data, unsigned int event, void *event_data);

#define points "points"
#define freq "freq"
#define bias "bias"
#define gain "gain"
#define gainNoise "gainNoise"
#define phase "phase"

uint64_t last_auto_push_ts = 0;
const boost::posix_time::ptime EPOCH(boost::gregorian::date(1970,1,1));

long double PI = acos((long double) -1);

boost::mt19937 rng((const uint_fast32_t) time(0));
boost::uniform_int<> one_to_hundred( -100, 100 );
boost::variate_generator< boost::mt19937, boost::uniform_int<> > randInt(rng, one_to_hundred);

int main(int argc, const char * argv[]) {
    int err = 0;
    srand((unsigned)time(0));
    ChaosMicroUnitToolkit mut;
    const char *option="Content-Type: application/bson-json\r\n";
    ChaosUniquePtr<RawDriverHandlerWrapper> hw = mut.createNewRawDriverHandlerWrapper(ProtocolTypeHTTP,
                                                                                      "ws://localhost:8080/io_driver",
                                                                                      option,
                                                                                      event_handler,
                                                                                      NULL,
                                                                                      "work");
    
    while (err == 0) {
        err = hw->poll(10);
    }
    std::cout << "Exti with code" << err;
    return err?EXIT_FAILURE:EXIT_SUCCESS;
}

int event_handler(void *user_data,
                  unsigned int event,
                  void *event_data) {
    int err = 0;
    switch(event){
        case UP_EV_USR_ACTION:{
            uint64_t curr_msec = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            if((curr_msec-last_auto_push_ts)>10000) {
                //push new data automatically
                RawDriverHandlerWrapper *pi = static_cast<RawDriverHandlerWrapper*>(event_data);
                DataPackUniquePtr message(new DataPack());
                message->addString("key", "async messages");
                pi->sendMessage(message);
                last_auto_push_ts = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            }
            break;
        }
        case UP_EV_CONN_CONNECTED:{
            std::cout << "UP_EV_CONN_CONNECTED" << std::endl;
            break;
        }
        case UP_EV_CONN_ERROR:{
            std::cout << "UP_EV_CONN_ERROR" << std::endl;
            break;
        }
        case UP_EV_CONN_RETRY:{
            std::cout << "UP_EV_CONN_RETRY" << std::endl;
            break;
        }
        case UP_EV_CONN_DISCONNECTED:{
            std::cout << "UP_EV_CONN_DISCONNECTED" << std::endl;
            break;
        }
        case UP_EV_CONN_ACCEPTED:{
            std::cout << "UP_EV_CONN_ACCEPT" << std::endl;
            //set te start base ts for push
            last_auto_push_ts = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            break;
        }
        case UP_EV_CONN_REJECTED:{
            std::cout << "UP_EV_CONN_REJECTED" << std::endl;
            break;
        }
        case UP_EV_AUTH_ACCEPTED:{
            std::cout << "UP_EV_AUTH_ACCEPTED" << std::endl;
            break;
        }
        case UP_EV_AUTH_REJECTED:{
            std::cout << "UP_EV_AUTH_REJECTED" << std::endl;
            break;
        }
        case UP_EV_MSG_RECEIVED:{
            std::cout << "UP_EV_MSG_RECEIVED" << std::endl;
            break;
        }
        case UP_EV_REQ_RECEIVED:{
            std::cout << "UP_EV_REQ_RECEIVED" << std::endl;
            UPRequest *req = static_cast<UPRequest*>(event_data);
            switch (req->message->getInt32("opcode")) {
                case 0:
                    //unused
                    break;
                case 1:
                    //unuseda
                    break;
                 
                case 2:
                    //unused
                    break;
                    
                case 3: {
                    req->response->addInt32("opcode_err", 0);
                    const uint32_t _points = (uint32_t)req->message->getInt32(points);
                    const double _freq = req->message->getDouble(freq);
                    const double _gain = req->message->getDouble(gain);
                    const double _gainNoise = req->message->getDouble(gainNoise);
                    const double _bias = req->message->getDouble(bias);
                    const double _phase = req->message->getDouble(phase);
                    double sin_points[_points];
                    double interval = (2*PI)/(_points);
                    for(int i=0; i<_points; i++){
                        double sin_point = sin((interval*i*_freq) + _phase);
                        double sin_point_rumor = (((double)randInt()/(double)100) * _gainNoise);
                        sin_points[i] = (_gain * sin_point) + sin_point_rumor + _bias;
                    }
                    req->response->addBinary("sin_wave",
                                             reinterpret_cast<const char *>(sin_points),
                                             _points*sizeof(double));
                    break;
                }
                default:
                    break;
            }
            break;
        }
        case UP_EV_ERR_RECEIVED:{
            std::cout << "UP_EV_ERR_RECEIVED" << std::endl;
            UPError *req = static_cast<UPError*>(event_data);
            std::cerr << "[Remote Error]\nCode:"<<req->error<<
            "\nError Message:"<<req->message<<
            "\nError Domain:"<<req->domain;
            break;
        }
    }
    return err;
}

/*
 //schedule work
 switch (proxy->protocol_adapter->getConnectionState()) {
 case ConnectionStateConnecting: {
 std::cout << "." << std::flush;
 break;
 }
 
 case ConnectionStateConnected:{
 std::cout << "Connected" << std::endl;
 break;
 }
 
 case ConnectionStateDisconnected:
 case ConnectionStateConnectionError:
 case ConnectionStateNotAccepted:
 while (proxy->unit_proxy->hasMoreMessage()) {
 std::cout << proxy->unit_proxy->getNextMessage()->message->toString();
 }
 if((ConnectionStateDisconnected == proxy->protocol_adapter->getConnectionState()) &&
 connecting == false) {
 std::cout << "Connecting" << std::flush;
 proxy->protocol_adapter->connect();
 connecting = true;
 } else {
 connecting = false;
 std::cout << "Retry in 5 seconds"<<std::endl;
 sleep(5);
 }
 break;
 
 case ConnectionStateAccepted:{
 if(connecting) {
 std::cout << "Accepted" << std::endl;
 connecting = false;
 }
 //manage the work on driver
 if((err = executeWork(*proxy->unit_proxy))) {
 std::cerr << "Error " << err << " during work";
 run = false;
 }
 break;
 }
 }
 
 //execute pool on connection
 proxy->protocol_adapter->poll(100);
 */
//int executeWork(raw_driver::RawDriverUnitProxy& proxy) {
//    int err = 0;
//    switch (proxy.getAuthorizationState()) {
//        case AuthorizationStateUnknown:
//            authorized = false;
//            std::cout << "Autorizing" << std::flush;
//            proxy.authorization("work");
//            break;
//        case AuthorizationStateRequested:
//            std::cout << "." << std::flush;
//            proxy.manageAutorizationPhase();
//            break;
//        case AuthorizationStateDenied:
//            std::cout << " authorization failed" << std::endl;
//            return -1;
//        case AuthorizationStateOk:
//            if(authorized == false){
//                std::cout << "Authorized" << std::endl;
//                authorized = true;
//            }
//            //manage request by remote user
//            err = manageRemoteMessage(proxy);
//            //perform idle operation
//            if(!err){
//                err = performIdleAction(proxy);
//            }
//            break;
//    }
//    return err;
//}
//
//int manageRemoteMessage(raw_driver::RawDriverUnitProxy& proxy) {
//    RemoteMessageUniquePtr remote_message;
//    DataPackUniquePtr remote_req_resp;
//    while(proxy.hasMoreMessage()) {
//        remote_message = proxy.getNextMessage();
//        if(remote_message->is_request &&
//           remote_message->request_message.get()){
//            remote_req_resp = consumeMessage(remote_message->request_message);
//            proxy.sendAnswer(remote_message, remote_req_resp);
//        } else if(remote_message->isError() == false){
//            remote_req_resp = consumeMessage(remote_message->message);
//        } else {
//            std::cerr << "[Remote Error]\nCode:"<<remote_message->getErrorCode()<<
//            "\nError Message:"<<remote_message->getErrorMessage()<<
//            "\nError Domain:"<<remote_message->getErrorDomain();
//        }
//    }
//    return 0;
//}
//
//DataPackUniquePtr consumeMessage(DataPackSharedPtr& message) {
//    DataPackUniquePtr result(new DataPack());
//    int32_t opcode = message->getInt32("opcode");
//    switch (opcode) {
//        case 0:
//            std::cout << "OPCODE 0 - Create new generation id" << std::endl;
//            result->addInt32("gen_id", 0);
//            break;
//
//        default:
//            break;
//    }
//    return result;
//}
//
//int performIdleAction(raw_driver::RawDriverUnitProxy& proxy) {
//    return 0;
//}
