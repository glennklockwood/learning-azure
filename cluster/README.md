# Setting up an MPI cluster on Azure

This document contains supplementary information to [Quick MPI Cluster Setup on
Azure][1] and has tools to provision an Azure cluster in two different ways:

## Imperative method

This uses the Azure CLI and relies on

- parameters.sh - global configuration parameters for the cluster
- make-cluster.sh - creates the compute nodes, virtual network, etc
- cloud-init.txt - provides minimum software environment to provisioned nodes
- setup-cluster.sh - script that runs on the head node to do post-provision setup
- add-route.sh - helper script for macOS to add a local route through a VPN to Azure (optional)
- make-storage.sh - creates the storage accounts and service endpoint in the cluster vnet

## Declarative method

This uses ARM templates and relies on

- cluster.json - creates the compute nodes, virtual network, etc
- cluster-noppg.json - same as above, but without proximity placement groups - useful for I/O
- parameters-public.json - global configuration parameters for the cluster

[1]: https://www.glennklockwood.com/cloud/mpi-cluster.html
