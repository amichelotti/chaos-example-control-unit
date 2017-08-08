//
//  main.cpp
//  SinGeneratorExternalDriver
//
//  Created by bisegni on 08/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>
#include <chaos_micro_unit_toolkit/micro_unit_toolkit.h>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::protocol_adapter;

int executeWork(RawDriverUnitProxy& proxy);
int manageRemoteMessage(RawDriverUnitProxy& proxy);
int performIdleAction(RawDriverUnitProxy& proxy);
DataPackUniquePtr consumeMessage(DataPackSharedPtr& message);
bool authorized = false;

int main(int argc, const char * argv[]) {
    int err = 0;
    bool run = true;
    bool connecting = false;
    ChaosMicroUnitToolkit mut;
    const char *option="Content-Type: application/bson-json\r\n";
    ChaosUniquePtr< UnitConnection<RawDriverUnitProxy> > proxy = mut.createNewRawDriverUnit(ProtocolTypeHTTP,
                                                                                            "ws://localhost:8080/io_driver",
                                                                                            option);
    if(proxy.get() == NULL) {std::cerr << "No unit proxy allcoated";return EXIT_FAILURE;}
    while (run) {
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
    }
    
    return 0;
}

int executeWork(RawDriverUnitProxy& proxy) {
    int err = 0;
    switch (proxy.getAuthorizationState()) {
        case AuthorizationStateUnknown:
            authorized = false;
            std::cout << "Autorizing" << std::flush;
            proxy.authorization("work");
            break;
        case AuthorizationStateRequested:
            std::cout << "." << std::flush;
            proxy.manageAutorizationPhase();
            break;
        case AuthorizationStateDenied:
            std::cout << " authorization failed" << std::endl;
            return -1;
        case AuthorizationStateOk:
            if(authorized == false){
                std::cout << "Authorized" << std::endl;
                authorized = true;
            }
            //manage request by remote user
            err = manageRemoteMessage(proxy);
            //perform idle operation
            if(!err){
                err = performIdleAction(proxy);
            }
            break;
    }
    return err;
}

int manageRemoteMessage(RawDriverUnitProxy& proxy) {
    RemoteMessageUniquePtr remote_message;
    DataPackUniquePtr remote_req_resp;
    while(proxy.hasMoreMessage()) {
        remote_message = proxy.getNextMessage();
        if(remote_message->is_request &&
           remote_message->request_message.get()){
            remote_req_resp = consumeMessage(remote_message->request_message);
            proxy.sendAnswer(remote_message, remote_req_resp);
        } else if(remote_message->isError() == false){
            remote_req_resp = consumeMessage(remote_message->message);
        } else {
            std::cerr << "[Remote Error]\nCode:"<<remote_message->getErrorCode()<<
            "\nError Message:"<<remote_message->getErrorMessage()<<
            "\nError Domain:"<<remote_message->getErrorDomain();
        }
    }
    return 0;
}

DataPackUniquePtr consumeMessage(DataPackSharedPtr& message) {
    DataPackUniquePtr result(new DataPack());
    int32_t opcode = message->getInt32("opcode");
    switch (opcode) {
        case 0:
            std::cout << "OPCODE 0 - Create new generation id" << std::endl;
            result->addInt32("gen_id", 0);
            break;
            
        default:
            break;
    }
    return result;
}

int performIdleAction(RawDriverUnitProxy& proxy) {
    return 0;
}
