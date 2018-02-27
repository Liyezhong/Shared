# Workflow

## Best Practices

### Configuration

  * Ping rate: default 60 s, trade-off between communication effort and web gui response time
  * AeDRMExecute timeout: 5 s, only relevant for shutdown
  * Communication timeout period: no server connection, i.e. select() timeout, default 30 s is okay
  * Communication retry period: waiting time before trying connection, default 30 s is okay
  * Persistent connections should be used for HTTP 1.1 stacks. There are no known proxy/protocol compatibility issues.
  * The following parameters should be configurable by production/service:
    * Serial number - DeviceConfiguration.xml
    * MAC address (board EEPROM) - per uboot? FEC and SPI?
  * The following parameters should be configurable by development:
    * Model number
  * The following parameters should be configurable by admin on-site:
    * proxy settings; Attention: Axeda Agent restart after proxy changes needed.
  * The following parameters should be configurable for server switch (e.g. from sandbox to production):
    * Ideally, separate config section for development and production server and a gui element to switch.
      * SSL
      * URL
      * target database

## Workflow pattern 1: Platform-initiated action with asynchronous Agent-acknowledgement

The DataItem is managed by Master and must be manually stored if needed. This allows to connect request and result for the Platform user.
The SOAP parameter pStatus (failed, not implemented, deferred, invalid params) is not applicable here and would be synchronous anyway.

Pattern:

  * Agent: At start initialize DataItem using AeDRMPostDataItem() with constant value (action request) or value delivered by Master (state change request):  RequestSomething = 0
  * Platform: Configuration - New - Action - Set Data Item in the Asset: RequestSomething = 1
  * Platform: Execute Action
  * Agent: In callback set with AeDRMSetOnCommandSetTag():
     * Forward request to Master
  * Master: communicate result (accept or deny) back to Platform over Agent
  * Master: set Data Item using Command: CmdRCNotifyDataItem(RequestSomething = 0)
     * if accepted:              RequestSomething = 1
     * if denied (action):       leave DataItem 0 and send event with reason
     * if denied (state change): send event with error message
  * Platform: status/state of request is displayed by Data Item value and error event (just for state change)

Naming convention for Data Items: prefix depending on:

  * change initiator (Platform vs. Agent)
  * type (action vs. state related)

Platform:

  * action request: Request
  * state change request: Set

Agent:

  * action allowance: Enable
  * state: State

### Misc Info

DataItems Types: (Ref: Axeda Platform SDK)

  * Analog:  -1.0E14 - 1.0E14
  * Digital: 0 or 1
  * String: 0 - 4000 characters

Remote Enterprise Server (Web sandbox or production):

  * Event Name: max 100 characters
  * Event Description: max 200 characters

## Remote system access

We might need live access on the system level. The QWS VNC remote session only allows GUI access.
So, a second SSH remote session should optionally be prepared (always available or enabled via GUI element).
The remote session type telnet with the embedded java applet  supports SSH protocol version 1 as of Feb 2011.
But a simple tunnel and an external PuTTY should be sufficient.
Alternatively, a command or downloaded file could be executed on the asset triggered by a Set Data Item Action.

## Workarounds for Agent Embedded shortcomings

  * Remote session: Agent cant close remote session tunnel.
    * Restart agent (e.g. by going through outer init/shutdown loop)
    * or: system level: block port with firewall
    * Current solution: Reconnect Agent with changed configuration
  * No timeout for remote session (e.g. if accidentally left active)
    * Main software closes and blocks remote session before starting activity.
    * Master closes connection by restarting Agent with different configuration
  * Remote session: no callbacks for VNC connect and disconnect
    * system level: poll netstat info
  * No download end callbacks.
    * Platform knows when download is finished and can communicate this to Agent as a restart action step of a Software Package.
    * Fixed in Agent 6.5-402
  * Platform software package dependencies evaluates Data Item on the Agent, which is not supported by Agent Embedded.
    * Use rule in order to start Data Item state-dependent download action.
    * Downloading per Business Rule command if device user triggers a update
  * Connected assets with duplicate serial numbers are not reported by the Platform.
    * Send Data Item String "mac_" + MAC address during initilization.
  * No way to detect failed asset registration.
    * Very rare error (e.g. conflict with manually created model)
    * There will be a timeout for Axeda Agent start anyway.
    * OnWebAccessError could be used

## Tips

  * Bandwith limit for file transfers (trickle) can be achieved by setting smaller chunk sizes (1M => 4KB) on the Platform server. This applies to the whole server, though.
  * Agent can deny a download by creating an error:
    * begin callback: if allow return AeFalse else return AeTrue
    * data callback: return AeFalse

# Main Scenarios

## File Transfer

### Platform user requests upload of asset information

Use "Workflow Pattern 1" (action request) as specified above:

  * Platform: Set number of log files to upload - SetLogNumber = {1,2,3,4}
    * 1: 5, 2: 10, 3: 15, 4: 30
  * Platform: Execute action - Set data Item: RequestAssetInformation = 1
  * Agent: Callback forwards request to Master software
  * Master: Assemble logfiles at specified pathname and communicate result to agent.
  * Agent: initiate upload of file (+delete)
  * Platform: displays status as RequestAssetInformation value and file transfer status
  * If there are alarms: Auto-upload information once per hour via Platform rules
  * Suppress and unsuppress Auto-upload via Platform rules

## Predefined Events

### Log classes

Everything is an event

Priority:
1. High
2. Medium
3. Low

Classes:
1. Alarm
2. Alarm + Warnings
3. Alarm + Warnings + Infos

#### Platform user requests filter to suppress events according to class

Use "Workflow Pattern 1" (state change request):

  * Platform: Execute action - Set data Item: e.g. SetLogLevel = 3
  * Platform: Action - SetLogClass = 2
  * Agent: Callback forwards request to Master software
  * Master: Set new defaul LogLevel (store configuration) and communicate result to agent
  * Platform: displays state as SetLogLevel and SetLogClass value

Platform user can filter alarms displayed in the service portal based on specific assets and alarm numbers
(see Online Help on Alarm Disposition)

### Software Management

#### Platform user sets the RemoteCare subscription status

Set subscription status (active, inactive) using "Workflow Pattern 1" (state change request):

  * Platform: Execute action - Set data Item: e.g. SetSubscription = 1
  * Agent: Callback forwards request to Master software
  * Master: Set and store configuration and communicate result to agent
  * Platform: displays status as SetSubscription value

#### Platform user deploys update notification

Notify assets that software update is available (also assets with inactive RemoteCare status).

Software Package with the following two steps is needed:

  * Download file with notification message
  * Platform: Execute action - Set data Item: e.g. SetUpdateAvailable = 1

Request display of notification file follows "Workflow Pattern 1" (state change request):

  * Agent: Callback forwards request to Master software
  * Master: Activate notification and available update state (store configuration) and  communicate result to agent;
    when device GUI user deactivates notification also store and communicate the change
  * Device User: Accepts notification
  * Master: set EnableSoftwareUpdate = 1
  * Agent: Downloads file and triggering SetDownloadFinished = 1
  * Master: triggers update
  * Platform (success): displays status as SetUpdateAvailable = 0, SetDownloadFinished=0 and EnableSoftwareUpdate=0 value
  * Platform (error): SetUpdateAvailable = 1, SetDownloadFinished=0 and EnableSoftwareUpdate=0

####  Platform user deploys software update

Download starts only after asset user has enabled the software update.

  * Agent: When asset user enables software update via GUI, set EnableSoftwareUpdate = 1

Workaround for lack of Software Package conditions:

  *  Create a rule which executes the package deployment based on a Data Item set by the Agent: EnableSoftwareUpdate = 1

From here it resembles "Workflow Pattern 1"

  * Agent: Downloads file and triggering SetDownloadFinished = 1
  * Master: Start update procedure and communicate result to agent;
  * Master (success):
    * EnableSoftwareUpdate = 0
    * SetUpdateAvailable = 0
    * SetDownloadFinished = 0

  * or in case of an error:
    * EnableSoftwareUpdate = 0
    * SetDownloadFinished = 0
    * SetUpdateAvailable = 1
    * Send Event with reason about the failure

  * Platform: displays status of all Data Items, deployment status and there might be alarms

## Remote Desktop Session

### Platform user requests remote session

  * Agent: Initialization
    * RequestRemoteSession = 0

Use "Workflow Pattern 1" (action request):

  * Platform: Execute action - Set data Item: RequestRemoteSession = 1
  * Agent: Callback forwards request to Master software
  * Master: Forward request to Gui
  * Gui: Asks "Allow Remote Session? Yes/No"
  * Master: sets RequestRemoteSession = 1 in configuration file and reconnects Agent
    * Without this request Agent will always deny the session and will prevent displaying name of the session on the Web interface.
      In this case Platform user is not able to select any remote session.
  * Platform: User should be able to see the Remote session name on the Web interface
