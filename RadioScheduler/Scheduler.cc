//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include <algorithm>
#include "Scheduler.h"

Define_Module(Scheduler);



Scheduler::Scheduler()
{
    selfMsg = nullptr;
}

Scheduler::~Scheduler()
{
    cancelAndDelete(selfMsg);
}


void Scheduler::initialize()
{
    NrUsers = par("gateSize").intValue();
    NrOfChannels = par("NrOfChannels").intValue();//read from omnetpp.ini
    selfMsg = new cMessage("selfMsg");
       scheduleAt(simTime(), selfMsg);
}

void Scheduler::handleMessage(cMessage *msg)
{
    int userWeights[NrUsers];
        userWeights[0] = 10; userWeights[1] = 5; userWeights[2] = 2; userWeights[3] = 1;
    int users[NrUsers], packets[NrUsers], timeLastServed[NrUsers];
    for (int q = 0; q < NrUsers; q++){
        users[q] = q;
        timeLastServed[q] = 0;
        packets[q] = NrUsers - q;
    }
    int suma = 0;

    /*cModule *module = 0;
    for (int q = 0; q < NrUsers; q++){
        module = getParentModule()->getSubmodule("User")->getSubmodule("MyQ");
        packets[q] = module->length;
        EV << "Lungimea cozii pentru userul " << q << " este: " << packets[q] << endl;
    }*/

    for(int a = 0; a < NrUsers; a++) suma += packets[a];
    if (msg == selfMsg){
      int isNotDone = NrOfChannels;
      if(NrOfChannels > suma) isNotDone = suma;
      while(isNotDone){
          for(int a = 0; a < NrUsers - 1; a++){
              for(int b = a + 1; b < NrUsers; b++){
                  if((simTime().raw() - timeLastServed[users[a]]) * userWeights[users[a]] <
                     (simTime().raw() - timeLastServed[users[b]]) * userWeights[users[b]]){
                      int aux = users[a];
                      users[a] = users[b];
                      users[b] = aux;
                  }
              }
          }
          for(int i = 0; i < NrUsers && isNotDone; i++){
              cMessage *cmd = new cMessage("cmd");
              if (packets[users[i]]){
                  if(packets[users[i]] <= userWeights[users[i]]){
                      if(packets[users[i]] <= isNotDone){
                          cmd->setKind((short) packets[users[i]]);
                          for (int q = 0; q < packets[users[i]]; q++){
                              cmd = new cMessage("cmd");
                              send(cmd,"txScheduling", users[i]);
                              delete cmd;
                          }
                          isNotDone -= packets[users[i]];
                          packets[users[i]] = 0;
                          //send(cmd,"txScheduling", users[i]);
                          timeLastServed[users[i]] = simTime().raw();
                      }
                      else{
                          cmd->setKind((short) isNotDone);
                          for (int q = 0; q < isNotDone; q++){
                              cmd = new cMessage("cmd");
                              send(cmd,"txScheduling", users[i]);
                              delete cmd;
                          }
                          isNotDone = 0;
                          packets[users[i]] -= isNotDone;
                          //send(cmd,"txScheduling", users[i]);
                          timeLastServed[users[i]] = simTime().raw();
                      }
                  }
                  else{
                      if(userWeights[users[i]] <= isNotDone){
                          cmd->setKind((short) userWeights[users[i]]);
                          for (int q = 0; q < userWeights[users[i]]; q++){
                              cmd = new cMessage("cmd");
                              send(cmd,"txScheduling", users[i]);
                              delete cmd;
                          }
                          isNotDone -= userWeights[users[i]];
                          packets[users[i]] -= userWeights[users[i]];
                          //send(cmd,"txScheduling", users[i]);
                          timeLastServed[users[i]] = simTime().raw();
                      }
                      else{
                          cmd->setKind((short) isNotDone);
                          for (int q = 0; q < isNotDone; q++){
                              cmd = new cMessage("cmd");
                              send(cmd,"txScheduling", users[i]);
                              delete cmd;
                          }
                          isNotDone = 0;
                          packets[users[i]] -= isNotDone;
                          //send(cmd,"txScheduling", users[i]);
                          timeLastServed[users[i]] = simTime().raw();
                      }
                  }
              }
          }
      }
	  scheduleAt(simTime()+par("schedulingPeriod").doubleValue(), selfMsg);
    }

}

