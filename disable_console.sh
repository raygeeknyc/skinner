#/bin/bash
# Enable the serial port on the UART for all Raspberry Pis as of 11/2016
port="$(ls -l /dev | grep 'serial0' | awk '{print $NF}')"
function die {
  echo $1 >&2
  exit -1
}
if [[ -z "$port" ]]; then
  die "No serial0 found"
fi
if [[ "$port" = "ttyAMA0" ]]; then
  echo "Raspberry Pi 3 port found"
else
  echo "Raspberry Pi non-3 port found"
fi
sudo systemctl stop serial-getty@$port.service
sudo systemctl disable serial-getty@$port.service
console="$(grep -e 'console=(serial0|$port'  /boot/cmdline.txt)"
if [[ -n "$console" ]]; then
  die "please remove console spec from /boot/cmdline.txt"
fi
config="$(grep "enable_uart=1" /etc/config.txt)"
if [[ -z "$config" ]]; then
  echo "fixing /etc/config.txt"
  cat etc_config.txt >> /etc/config.txt
fi
echo "UART is on /dev/ttyS0"
exit 0
