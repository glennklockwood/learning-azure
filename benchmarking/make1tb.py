#!/usr/bin/env python3
"""Create a 1 TiB block blob.

TODO: Properly parameterize this.
"""

import os
import time
from azure.core.exceptions import ResourceNotFoundError
from azure.storage.blob import BlobServiceClient, BlobType

# Define connection string and container name
conn_str = os.environ["AZURE_CONN_STR"]
container_name = os.environ["AZURE_CONTAINER_NAME"]

# Create BlobServiceClient object
blob_service_client = BlobServiceClient.from_connection_string(conn_str)

# Get BlobClient object for your block blob
blob_client = blob_service_client.get_blob_client(container_name, "testblob")

if(blob_client.exists):
    print("Cleaning up existing block blob")
    try:
        blob_client.delete_blob()
    except ResourceNotFoundError:
        print("But the block blob doesn't exist? let's try creating it")

# Set the blob type to BlockBlob
blob_client.upload_blob("", blob_type=BlobType.BlockBlob)

block_size = 32 * 1024**2
blob_size = 10 * 1024**3
num_chunks = blob_size // block_size
block_contents = os.urandom(block_size)

# Write block-by-block. Can be parallelized.
for i in range(num_chunks):
    t1 = time.time()
    block_id = f"{i:08x}"
    blob_client.stage_block(block_id=block_id, data=block_contents)
    t2 = time.time()
    print(f"Staged block {block_id:s} in {t2 - t1:.2f} seconds; {block_size / 1024 / 1024 / (t2 - t1):.2f} MiB/s; {i*block_size/1024**3:.2f} GiB total")

# Commit the block list
block_list = [f"{i:08x}" for i in range(num_chunks)]
t1 = time.time()
blob_client.commit_block_list(block_list)
t2 = time.time()
print(f"Commit block list in {t2 - t1:.2f} seconds")

print("Block blob uploaded successfully!")