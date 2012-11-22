### start QVFb for Target-on-Host app:
qvfb -width 800 -height 600 -depth 32 &

### give QVFb time to start:
sleep 1

### start Agent: debug messages on; no VNC
#QWS_SIZE=800x600 ./AxedaAgent -qws -display QVFb:0 -g -s https://leica-sandbox.axeda.com:443/eMessage

### start Agent: debug messages off; no VNC
#QWS_SIZE=800x600 ./AxedaAgent -qws -display QVFb:0 -s https://leica-sandbox.axeda.com:443/eMessage

### start Agent: debug messages off; with VNC
QWS_SIZE=800x600 ./AxedaAgent -qws -display "Multi: VNC:0 QVFb:0" -s https://leica-sandbox.axeda.com:443/eMessage

