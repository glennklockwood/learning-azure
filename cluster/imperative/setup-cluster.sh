#!/usr/bin/env bash
#
# This script is meant to be run from the head node of the cluster after the
# cluster is already provisioned.

source parameters.sh

WORK_DIR=$HOME

cd $WORK_DIR

sudo bash -c 'cat hosts.append >> /etc/hosts'

for (( i = 0; i < $VM_COUNT; i++ ))
do
    ssh-keyscan glocluster${i}
done > ~/.ssh/known_hosts

clush -w "glocluster[0-$((VM_COUNT - 1))]" -c ~/.ssh/id_rsa ~/.ssh/known_hosts
