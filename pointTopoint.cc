/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 
                  5Mbps
 node1-------------------------------node2
 
 Sends 1 packet from node 1 to node 2 with speed 5Mbps and delay 2millisec and with packet size 1024.
 
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"

using namespace ns3;



NS_LOG_COMPONENT_DEFINE ("FirstScriptExample");


int
main (int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  Time::SetResolution (Time::NS);
  LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_INFO); //Enabling log
  LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_INFO);

  NodeContainer nodes;
  nodes.Create (2); //Creation of two empty nodes


  PointToPointHelper pointToPoint; //creation of a interface
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps")); // adding data speed to interface
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));  //adding delay between packets

  NetDeviceContainer devices;   
  devices = pointToPoint.Install (nodes); //installing attributes for nodes 

  InternetStackHelper stack; //This helper enables pcap and ascii tracing of events in the internet stack associated with a node. 
  stack.Install (nodes); //creating stack to each node

  Ipv4AddressHelper address;
  address.SetBase ("10.1.1.0", "255.255.255.0"); //assigning ip address to both nodes
  
  //node1 ip=10.1.1.1
  //node2 ip=10.1.1.2

  Ipv4InterfaceContainer interfaces = address.Assign (devices); // connecting both nodes

  UdpEchoServerHelper echoServer (9);  //Giving server port no:9

  ApplicationContainer serverApps = echoServer.Install (nodes.Get (1)); //installing server on node 2
  serverApps.Start (Seconds (1.0)); //start server at time 1s
  serverApps.Stop (Seconds (10.0));  //stop server at time 10s

  UdpEchoClientHelper echoClient (interfaces.GetAddress (1), 9); //providing socket to client i.e ipaddress and port of server
  echoClient.SetAttribute ("MaxPackets", UintegerValue (1)); //sending maximum packet 1
  echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0))); // sending packet in an interval of 1s
  echoClient.SetAttribute ("PacketSize", UintegerValue (1024));  //Each packet should be of size 1024B

  ApplicationContainer clientApps = echoClient.Install (nodes.Get (0));   //installing client on node 1
  clientApps.Start (Seconds (2.0)); //start client at time 2s
  clientApps.Stop (Seconds (10.0)); //stop client at time 10s


 // Using NetAnim
	AnimationInterface::SetConstantPosition (nodes.Get (0), 10.0, 20.0); //provide co-ordinates it node 1 for network Animator
	AnimationInterface::SetConstantPosition (nodes.Get (1), 20.0, 20.0); //provide co-ordinates it node 2 for network Animator
	AnimationInterface anim ("p2p1.xml"); //Creating xml file to run network animator.

        AsciiTraceHelper ascii;
        pointToPoint.EnableAsciiAll (ascii.CreateFileStream ("p2p1.tr")); //To generate trace file
        pointToPoint.EnablePcapAll ("p2p1"); //To generate pcap files which can be monitered in wireshark


  Simulator::Run (); //start simulation
  Simulator::Destroy (); //destroy simulation
  return 0;
}
