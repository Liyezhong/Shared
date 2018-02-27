## Intro

This is general information page about RemoteCare project.
This document is writen in markdown style
[Markdown](http://lmsnus-s-gitlab.europe.leicams.com/help/markdown)

## Getting Started

### Setup

Configure Deviceconfiguration.xml, UserSettings.xml and RCConfiguration.xml for your needs.
This configuration is placed under Settings in /home/Leica or on your PC under your own structure.

### Run

Under /home/Leica/Bin

cd /home/Leica/Bin
./RemoteCareAgent ip port

IP: Should be the IP of master or stub
Port: 8801 or different but should be kept same for master and stub

Stub:

Should have changes in ServerAndClients.xml under Settings/Communication/ServerAndClients.xml you should change the server entry to an ip adress which is addressed by RemoteCareAgent see above. It is important if the stub is running on PC/Laptop.

### Time

Do not forget to set the time otherwise some events will not be shown on Web UI.

Replace MMDDhhmmYYY.ss with current date
* MM - month
* DD - day
* hh - hours
* mm - minutes
* YYYY - year
* ss  - seconds

date MMDDhhmmYYYY.ss
hwclock --systohc - need to store it in the hardware clock

### Axeda Connection

https://leica-sandbox.axeda.com/drm/actions/index

Access Axeda and take a look at the Assets. The configuration of device name and serial number is done in Settings/DeviceConfiguration.xml

## RemoteSession

To allow RemoteSession the DataItem RemoteSessionRequest Value need to be set to 1.
This happens by Master SW if RemoteSession request was granted by the Device user.

### VNC

* TightVNC: please consider to shutdown the server part otherwise the port is remapped to 10000
* AxedaDesktopView: need to change setting to Tight otherwise only a black window is shown

## Deploy

[Stub deploy matrix](https://lmsnus-s-cbs.europe.leicams.com/jenkins/view/ZDeploy/job/ColoradoDeployStubMatrix/)

RemoteCareAgent can be used from here
[Jenkins-DeployMatrix](https://lmsnus-s-cbs.europe.leicams.com/jenkins/view/ZDeploy/job/ColoradoDeployMatrix/)

__ATTENTION__ Make sure binaries are executable

* _ls -la_ - check settings
* _chmod u+x BinareName_ - change mode

## TODO - nice to have

* moving commands to remotecareagent - using autogen for CMD generation
* update local documentation markdown file with Leica template to generate CDD etc out of it
* Duplicate devices?
* replaces qt usage with STL but still fulfilling the command structure
* Rework Agent to a rosnode

## Open points

* Access Log files even when RemoteCare is deactivated. (Requested by Service - decision pending)

## External libraries

AgentEmbedded depends on openssl.
It can cause issues in some cases for example like "General SSL error" coming from AgentEmbedded. The root cause is an older version of openssl library.

### openssl 1.0.0j

Issue: RemoteCareAgent is unable to ignore untrusted certificate of sandbox server.
On VM: With OpenSSL 1.0.1 14 Mar 2012 it works on VM.
Cross verified by downgrading the local openssl library on the VM to openssl-1.0.0j and the behavior is reproducible.

Steps to compile for VM:
Remove libssl-dev package: sudo apt-get remove libssl-dev
Get openssl-1.0.0j from openssl web site
tar xzf openssl-1.0.0j.tar.gz
./config shared --prefix=/usr --openssldir=/lib/i386-linux-gnu
Remove or move shared library in /lib/i386-linux-gnu/libssl.so.1.0.0 and libcrypto.so.1.0.0 to different name or place
Create symbolic links for /lib/i386-linux-gnu/libssl.so.1.0.0 from /usr/lib/libssl.... and for libcrypto instead
Troubleshooting: If you get errors like undefined symbols EVP_idea_cbc then libcryto.so is not linked properly
Check with nm -D /lib/i386-linux-gnu/libcrypto.so.1.0.0 | grep EVP_idea and nm -D /lib/i386-linux-gnu/libssl.so.1.0.0 | grep EVP_idea
