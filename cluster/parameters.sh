export VM_SKU="Standard_DS1_v2"
export VM_COUNT=3
export VM_IMAGE="Canonical:0001-com-ubuntu-server-jammy:22_04-lts-gen2:latest"

export ADMIN_USER="glockwood"

export PPG_NAME="glocktestppg"
export RG_NAME="glocktestrg"

# STORAGE_ACCOUNT_TYPE="--sku Standard_LRS" # HDD blob
export STORAGE_ACCOUNT_TYPE="--sku Premium_LRS --kind BlockBlobStorage" # SSD (Premium)blob

export STORAGE_ACCOUNT_NAME="glockteststorage"
