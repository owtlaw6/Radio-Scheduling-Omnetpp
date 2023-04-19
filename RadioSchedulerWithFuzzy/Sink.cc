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

#include "Sink.h"

Define_Module(Sink);

void Sink::initialize()
{
      lifetimeSignal = registerSignal("lifetime");
      NrUsers = par("gateSize").intValue();
      NrOfChannels = par("NrOfChannels").intValue();
      NetworkLoad = par("NetworkLoad").doubleValue();
      NrPackets = 0;
      totalDelayTime = 0;
      delay = 0;
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    int user0 = 0;
    if (msg->arrivedOn("rxPackets", user0)) {
        NrPackets += 1;
        totalDelayTime += lifetime.dbl();
    }
    else if (msg->arrivedOn("in")) {
        if(NrPackets > 0)
            delay = (int)((totalDelayTime * 10)/ NrPackets);
        else delay = 0;

        cMessage *msgr = new cMessage("response");
        msgr->setKind((short) delay);
        send(msgr, "out");

        EV << "delay logger " << delay << endl;

        NrPackets = 0;
        totalDelayTime = 0;
    }


    EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
    emit(lifetimeSignal, lifetime);

    delete msg;
}

void Sink::finish()
{
    EV << "finish() function in sink " << endl;
}

