#!/usr/bin/env bash
#
# This is a macOS-specific script to add a route through a VPN interface to
# access Azure.  This is useful when a corporate policy restricts access to
# a VPN.

set -ex

if [[ $(uname) == Darwin ]]; then
    source cluster-args.sh

    tunnel_iface="$(ifconfig | grep -A 2 utun | awk '/inet / {print $2}')"

    headnode_ip=$(az vm list-ip-addresses --resource-group glocktestrg --query "[*].virtualMachine.network.publicIpAddresses[?name=='gloclusterPublicIP0'].ipAddress[] | [0]" | sed -e 's/"//g')

    if [ ! -z "$tunnel_iface" ]; then
        sudo route add -net $headnode_ip/32 "$tunnel_iface"
    fi
fi
