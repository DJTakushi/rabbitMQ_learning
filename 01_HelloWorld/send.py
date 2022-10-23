#!/usr/bin/env python
import sys
import pika

def singlePublish(address_i, routingKey_i, messages):
    connection = pika.BlockingConnection(pika.ConnectionParameters(address_i))
    channel = connection.channel()

    # We're connected now, to a broker on the local machine - hence the localhost.
    # If we wanted to connect to a broker on a different machine
    # we'd simply specify its name or IP address here.

    # Next, before sending we need to make sure the recipient queue exists.
    # If we send a message to non-existing location,
    # RabbitMQ will just drop the message.
    # Let's create a hello queue to which the message will be delivered:

    channel.queue_declare(queue=routingKey_i)

    # At this point we're ready to send a message.
    # Our first message will just contain a string Hello World!
    # and we want to send it to our hello queue.

    # In RabbitMQ a message can never be sent directly to the queue,
    # it always needs to go through an exchange.
    # But let's not get dragged down by the details ‒
    # you can read more about exchanges in the third part of this tutorial.
    # All we need to know now is how to use a default exchange identified by
    # an empty string.
    # This exchange is special ‒ it allows us to specify
    # exactly to which queue the message should go.
    # The queue name needs to be specified in the routing_key parameter:

    for m in messages:
        channel.basic_publish(exchange='', # default value
                              routing_key=routingKey_i,
                              body=m)
        print(" [x] Sent '"+m+"'")

    # Before exiting the program we need to make sure the network buffers
    # were flushed and our message was actually delivered to RabbitMQ.
    # We can do it by gently closing the connection.
    connection.close()

if __name__ == '__main__':
    argLen = len(sys.argv)
    if argLen >= 1:
        if argLen >= 2:
            messages = []
            address = 'localhost'
            routingKey = 'hello'
            for i in sys.argv[1:]:
                if i[0:2] == "-a":
                    address = i[2:]
                    print("address set to:"+address)
                elif i[0:2] == "-r":
                    routingKey = i[2:]
                    print("routingKey set to:"+routingKey)
                else:
                    messages.append(i)
            singlePublish(address,routingKey,messages)
        else:
            print("sending default")
            print("  set address with:    \"-a[address]\"    (defult is 'localhost')")
            print("  set routingKey with: \"-r[routingKey]\" (default is 'hello')")
            print("  set queue with:      \"-q[queue]\"      (default is 'hello')")
            print("  set messages with additional inputs   (default is 'Hello World!')")
            singlePublish('localhost','hello',['Hello World!'])
    else:
        print("0 argumnents (?)")
