#!/bin/bash

sudo gdbus introspect -y -r -d name.marples.roy.dhcpcd -o /name/marples/roy/dhcpcd
sudo gdbus call -y -d name.marples.roy.dhcpcd -o /name/marples/roy/dhcpcd -m name.marples.roy.dhcpcd.ListInterfaces
sudo gdbus call -y -d name.marples.roy.dhcpcd -o /name/marples/roy/dhcpcd -m name.marples.roy.dhcpcd.GetInterfaces

#sudo gdbus introspect -y -r -d fi.w1.wpa_supplicant1 -o /fi/w1/wpa_supplicant1
#sudo gdbus introspect -y -r -d fi.epitest.hostap.WPASupplicant -o /fi/epitest/hostap/WPASupplicant


#sudo gdbus introspect -y -r -d org.freedesktop.PackageKit -o /org/freedesktop/PackageKit
#sudo gdbus call -y -d org.freedesktop.PackageKit -o /org/freedesktop/PackageKit -m org.freedesktop.DBus.Properties.Get org.freedesktop.PackageKit Groups
