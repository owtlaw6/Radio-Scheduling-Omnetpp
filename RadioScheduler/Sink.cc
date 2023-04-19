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
      sendingTime = 0;
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    int user0 = 0, user1 = 1, user2 = 2;
    if (msg->arrivedOn("rxPackets", user0)) {
        lifetimeStats0.collect(lifetime);
        NrPackets += 1;
    }
    else if (msg->arrivedOn("rxPackets", user1)) {
        lifetimeStats1.collect(lifetime);
        NrPackets += 1;
    }
    else if (msg->arrivedOn("rxPackets", user2)) {
        lifetimeStats2.collect(lifetime);
        NrPackets += 1;
    }

    EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
    emit(lifetimeSignal, lifetime);
    delete msg;
}

void Sink::finish()
{
    EV << "lifetime mean user0 " << lifetimeStats0.getMean() << endl;
    EV << "lifetime mean user1 " << lifetimeStats1.getMean() << endl;
    EV << "lifetime mean user2 " << lifetimeStats2.getMean() << endl;

    EV << "lifetime min user0 " << lifetimeStats0.getMin() << endl;
    EV << "lifetime min user1 " << lifetimeStats1.getMin() << endl;
    EV << "lifetime min user2 " << lifetimeStats2.getMin() << endl;

    EV << "lifetime max user0 " << lifetimeStats0.getMax() << endl;
    EV << "lifetime max user1 " << lifetimeStats1.getMax() << endl;
    EV << "lifetime max user2 " << lifetimeStats2.getMax() << endl;

    FILE * fp;
    fp = fopen ("statistici.txt", "a");
    fprintf(fp, "%.3f, %.0f, %.0f\n", lifetimeStats0.getMean(), lifetimeStats0.getMin(), lifetimeStats0.getMax());
    fprintf(fp, "%.3f, %.0f, %.0f\n", lifetimeStats1.getMean(), lifetimeStats1.getMin(), lifetimeStats1.getMax());
    fprintf(fp, "%.3f, %.0f, %.0f\n", lifetimeStats2.getMean(), lifetimeStats2.getMin(), lifetimeStats2.getMax());

    sendingTime = (NrUsers * NrPackets) / (NetworkLoad * (NrOfChannels / 1));
    fprintf(fp, "%.1f, %.3f\n", NetworkLoad, sendingTime);

    fclose(fp);

    lifetimeStats0.recordAs("my lifetime user0");
    lifetimeStats1.recordAs("my lifetime user1");
    lifetimeStats2.recordAs("my lifetime user2");
}

