##  RemoteCare Integration

### Architecture Overview:

The Colorado Master software contains a RemoteCareController, which communicates with a separate process called  RemoteCareAgent over TCP/IP.
It's the same approach as the Colorado GUI takes, but there are fewer NetCommands.
The RemoteCare service is based on the product "ServiceLink" of the company Axeda. Axeda provides a small library which is used by the RemoteCareAgent.

### History and current state:

In 2011 Yaroslav Novak, a contractor, worked on RemoteCare design and implementation.
Unfortunately, the Axeda library for embedded devices and its documentation had some errors, which prevented Yaroslav from finishing the component.
Then he left the project and Torsten took over.
A workwhop with an Axeda trainer helped to solve the remaining problems and to define a workflow which fits to the web services.
The results have been summarized here:  [Link](http://10.221.153.193/dokuwiki/doku.php?id=tools:axeda_workshop)
And the resulting workflow has been respected in the requirements for the RemoteCare Component (see below).
The existing components are still in Yaroslav's work in progress state and need to be adapted to the new workflow and to the found workarounds.
This is now the task of our new colleague, Boris Stach. Torsten can assist within the limited time from home during his parental leave.
There is an overview of the low level axeda functionality, but this should not be relevant for Ramya's the NetCommand/Event integration:
[Link](http://10.221.153.193/dokuwiki/doku.php?id=tools:axeda)

Existing components:

* RemoteCareAgent component connected to Master via NetCommands:
  * Platform/RemoteCareAgent
* RemoteCareController (main sw thread) communicating with RemoteCareAgent:
  * Platform/Master/Components/RemoteCareController
  * NetCommands (need to be adapted)
  * Platform/Master/Components/NetCommands
    * CmdRCAgentAlarm.cpp
    * CmdRCAgentDataItem.cpp
    * CmdRCAgentEvent.cpp
    * CmdRCAgentRemoteSessionRequest.cpp
    * CmdRCAgentRemoteSessionStatus.cpp
    * CmdRCAgentSetTag.cpp
    * CmdRCAgentUpload.cpp
* Use as template for RemoteCareStub (as master substitute):
  * Colorado/ColoradoMain/Gui/GuiStub/

### Next steps:

*    Define needed NetCommands for required workflow scenarios. (needed for all further work) (Boris with assistance of Torsten)
*    Strip down and adapt GuiStub in order to send and receive NetCommands from RemoteCareAgent. (Boris:  Need to define clear Interface regarding to the described requirements)
*    Clean up RemoteCareAgent's Axeda callbacks and review or even refactor architecture. (Boris)
*    Implement workflow on master side via event handler. (Ramya)
*    Check testability. Unit tests with axeda library stub? (Boris: Testprotocol based on NetCommands)

## Requirements: (ATTENTION this requirements are not uptodate check the official releases)

### PRD - Maintainability & Support

* The product shall be able to integrate to Leica RemoteCare.
* The product shall provide lifetime counter values for critical modules/components to RemoteCare.
* The product shall provide instrument diagnosis relevant events to RemoteCare.
* RemoteCare shall be able to request lifetime counter, events, instrument set-up and instrument configurations.
* The product shall be able to share the GUI via RemoteCare after permission of the local user.
* Via RemoteCare the user shall be informed if a software update is available.
* The product shall enable users to update instrument software on-site and via RemoteCare keeping local instrument settings and configurations.
* Maintenance/support activities shall be possible from the front of the instrument.
* There shall be an interface available to enable service users to perform supported installations and functional tests of assemblies and subassemblies. All modules shall be executable independently.
* The service user shall be able to level the instrument during installation.
* All tubing shall be exchangeable and distinguishable.

### SRS - Remote Care

* The system shall be able to register with the Axeda ServiceLink platform.
* The system shall provide the supervisor user and the service user with the ability to edit internet proxy settings via GUI.
* The system shall provide the supervisor user and the service user with the ability to disable the RemoteCare service via GUI.
* The system shall create encrypted archive files for device meta data, status and process information.
* The system shall provide the Axeda web front-end user with the ability to request the upload of files to the Axeda ServiceLink platform.
* The system shall send events to the Axeda ServiceLink platform.
* The system shall provide the Axeda web front-end user with the ability to set the class of events sent to the Axeda ServiceLink platform.
* If the system is idle, the system shall be able to share the GUI session with an Axeda web front-end user.
* The system shall provide the Axeda web front-end user with the ability to request a shared GUI session.
* The system shall provide the user with the ability to allow or deny a shared GUI session via GUI.
* The system shall provide the Axeda web front-end user with the ability to inform the user about an available software update.
* The system shall provide the Axeda web front-end user with the ability to set the RemoteCare subscription status.
* If RemoteCare subscription is active AND the system is idle AND the user level is "Supervisor" OR "Service", the system shall provide the user with the ability to initiate a software update via RemoteCare.
* If a software update package is locally stored AND the system is idle AND the user level is "Supervisor" OR "Service", the system shall provide the user with the ability to initiate a software update.

### MRS Master Software - Remote Care

* The master SW shall be able to register with the Axeda ServiceLink platform.
* The master SW shall provide the supervisor user and the service user with the ability to edit internet proxy settings via GUI.
* The master SW shall provide the supervisor user and the service user with the ability to disable the RemoteCare service via GUI.
* The master SW shall create encrypted archive files for device meta data, status and process information.
* The master SW shall provide the Axeda web front-end user with the ability to request the upload of files to the Axeda ServiceLink platform.
* The master SW shall send events to the Axeda ServiceLink platform.
* The master SW shall provide the Axeda web front-end user with the ability to set the class of events sent to the Axeda ServiceLink platform.
* If the master SW is idle, the master SW shall be able to share the GUI session with an Axeda web front-end user.
* The master SW shall provide the Axeda web front-end user with the ability to request a shared GUI session.
* The master SW shall provide the user with the ability to allow or deny a shared GUI session via GUI.
* The master SW shall provide the Axeda web front-end user with the ability to inform the user about an available software update.
* The master SW shall provide the Axeda web front-end user with the ability to set the RemoteCare subscription status.
* If RemoteCare subscription is active AND the master SW is idle AND the user level is "Supervisor" OR "Service", the master SW shall provide the user with the ability to initiate a software update via RemoteCare.

