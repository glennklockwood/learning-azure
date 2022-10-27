To deploy the ARM template:

    az deployment group create --template-file template.json --parameters parameters.json -g glocktestrg

To deploy a bicep version:

    az deployment group create --template-file cluster.bicep --parameters parameters.json -g glocktestrg 
