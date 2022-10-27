#!/usr/bin/env bash
#
# This creates a cluster of VMs on the same proximity placement group and does
# basic post-provisioning configuration using the setup-cluster.sh bash script.

set -ex

source cluster-args.sh

az group create --name $RG_NAME --location eastus

az ppg create --name $PPG_NAME \
              --resource-group $RG_NAME \
              --intent-vm-sizes $VM_SKU

az vm create --name glocluster \
             --resource-group $RG_NAME \
             --image "$VM_IMAGE" \
             --ppg $PPG_NAME \
             --generate-ssh-keys \
             --size $VM_SKU \
             --admin-username $ADMIN_USER \
             --accelerated-networking true \
             --custom-data cloud-init.txt \
             --count $VM_COUNT

headnode_ip=$(az vm list-ip-addresses --resource-group $RG_NAME --query "[*].virtualMachine.network.publicIpAddresses[?name=='gloclusterPublicIP0'].ipAddress[] | [0]" | sed -e 's/"//g')

./add-route.sh

scp ~/.ssh/id_rsa ${ADMIN_USER}@${headnode_ip}:.ssh/

az vm list-ip-addresses --resource-group $RG_NAME \
                        --query '[*].virtualMachine.[name, network.privateIpAddresses[0]]' \
                        -o table | tail +3 > hosts.append

scp hosts.append ${ADMIN_USER}@${headnode_ip}:hosts.append

scp cluster-args.sh setup-cluster.sh ${ADMIN_USER}@${headnode_ip}:

ssh ${ADMIN_USER}@${headnode_ip} "./setup-cluster.sh"
