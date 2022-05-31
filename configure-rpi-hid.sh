#!/bin/bash

if ! grep -Fxq "dtoverlay=dwc2" /boot/config.txt; then
    echo "dtoverlay=dwc2" | sudo tee -a /boot/config.txt
fi

if ! grep -Fxq "dwc2" /etc/modules; then
    echo "dwc2" | sudo tee -a /etc/modules
fi

if ! grep -Fxq "libcomposite" /etc/modules; then
    sudo echo "libcomposite" | sudo tee -a /etc/modules
fi

sudo cp pichuk_usb /usr/bin/pichuk_usb
sudo chmod +x /usr/bin/pichuk_usb #make it executable


if ! grep -Fxq "/usr/bin/pichuk_usb" /etc/modules; then
    sudo sed -i '/^exit.*/i \/usr\/bin\/pichuk_usb' /etc/rc.local
fi
