### start QVFb for Target-on-Host app:
qvfb -width 800 -height 600 -depth 32 &

### give QVFb time to start:
sleep 1

### start Agent: debug messages on; with VNC; intentionally wrong server link:
QWS_SIZE=800x600 ./AxedaAgent -qws -display "Multi: VNC:0 QVFb:0" -s https://leica-sandbox.blabla123.com:443/eMessage

