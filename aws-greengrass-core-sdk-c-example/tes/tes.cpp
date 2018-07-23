/*
 * Copyright 2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 */

#include <aws/core/Aws.h>
#include <aws/core/utils/Outcome.h>
#include <aws/core/auth/AWSCredentialsProviderChain.h>
#include <aws/dynamodb/DynamoDBClient.h>
#include <aws/dynamodb/model/ListTablesRequest.h>
#include <aws/dynamodb/model/ListTablesResult.h>
#include <iostream>
#include "tes.h"

void listTables()
{
    Aws::SDKOptions options;
    Aws::InitAPI(options);
    {
        Aws::Client::ClientConfiguration config;
        config.region = Aws::String("us-west-2");

        Aws::DynamoDB::DynamoDBClient dynamoClient(config);

        Aws::DynamoDB::Model::ListTablesRequest ltr;
        ltr.SetLimit(5);

        const Aws::DynamoDB::Model::ListTablesOutcome& lto = dynamoClient.ListTables(ltr);
        if(!lto.IsSuccess())
        {
            std::cout << "Error: " << lto.GetError().GetMessage() << std::endl;
        }
        else
        {
            std::cout << "Tables: " << std::endl;
            for(const auto& s : lto.GetResult().GetTableNames())
            {
                std::cout << s << std::endl;
            }
        }
    }

    Aws::ShutdownAPI(options);
}
