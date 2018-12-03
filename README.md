# Introduction
A friend built a custom electric Baja Bug and is racing it in a sort of off road auto cross.  It turns out that when your car isn't filling the atmosphere with carbon, it isn't making noise either.  This, in my opinion is fantastic.  However, that noise is usually the que that racers use to know whether or not the tires are spinning.  

The curtis controller he is using broadcasts RPM, Amps and a few other pieces of information.  The OBDII reader that usually reads an ODBII output on an OEM car asks for RPM through the ODBII protocal, which is a layer over CAN.  The controller responds with the requested information.  The ODBII reader takes the RPM and current (engine load) and sends it to an android tablet over blue tooth where the tablet has an app that makes synthetic engine noise and pumps it through the stereo.   

Want your silent bug to sound like a Ferrari.  No problem.  A chainsaw.  No problem.  (his kids favorite.) 

The hardware is an Arduino Due with two CAN tranceivers.  

#  Prerequisites 

Requires due_can https://github.com/collin80/due_can,
Which requires can_common found here https://github.com/collin80/can_common.

Note: I had to make one small change to get it to compile.  I have since forgotten what that change was.  Email me if knowing this is important to you and I will chase it down.

# Status
This is currently a work in progress.  It works with the exception of needing two power cycles to get it to connect.  I suspect I'm not responding correctly to some request in the initial negotiation with the ODBII reader.  I can't imagine why it will connect reliably after a power cycle, but it does. 

Our schedules haven't since crossed in a way that I can try to fix that, or finish implementing a few other niceties.  



