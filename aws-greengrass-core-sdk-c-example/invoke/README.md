This example shows how to invoke a lambda function and it assumes you already have both invoker lambda and invokee lambda added to Greengrass group.

The invoker lambda sends a string payload to invokee which concatenates another string and send the result back. 
In the example, invoker sends "hello" to invokee which appends "world" to its input. In the end, "hello world" is sent back to invoker.
