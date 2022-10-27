#!/usr/bin/env bash
#
# This script creates a storage account that can be used to serve up blob over
# NFS as a shared file system. It also adds the service endpoint to an existing
# cluster vnet.

source cluster-args.sh

VNET_NAME="$(az network vnet list -g $RG_NAME --query '[].name | [0]' | sed -e 's/"//g')"
SUBNET_NAME="$(az network vnet subnet list --resource-group $RG_NAME --vnet-name $VNET_NAME --query '[].name | [0]' | sed -e 's/"//g')"

az storage account create --name $STORAGE_ACCOUNT_NAME \
                          --resource-group $RG_NAME \
                          $STORAGE_ACCOUNT_TYPE \
                          --enable-hierarchical-namespace true \
                          --enable-nfs-v3 true \
                          --default-action deny

# enable the service endpoint for Azure Storage on our subnet
az network vnet subnet update --resource-group $RG_NAME \
                              --vnet-name $VNET_NAME \
                              --name $SUBNET_NAME \
                              --service-endpoints Microsoft.Storage

# add a network rule to the storage account allowing access from our vnet
az storage account network-rule add --resource-group $RG_NAME \
                                    --account-name $STORAGE_ACCOUNT_NAME \
                                    --vnet-name $VNET_NAME \
                                    --subnet $SUBNET_NAME
