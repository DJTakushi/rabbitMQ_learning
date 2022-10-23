#!/usr/bin/env python
import pika, sys, os


class callbackHandler:
    def __init__(self, count_i = -1):
        self.count = count_i


    # callback to be used by Pika library
    def callback(ch, method, properties, body):
        print(" [x] Received %r" % body)


def myConsumer(address_i, queue_i):
    # create identical connection as send.py
    connection = pika.BlockingConnection(pika.ConnectionParameters(address_i))
    channel = connection.channel()

    # queue declaration is idempotent - only declares once
    channel.queue_declare(queue=queue_i)

    channel.basic_consume(  queue=queue_i,
                            auto_ack=True,
                            on_message_callback=callbackHandler.callback)

    print('[*] Waiting for messages.  To exit press CTRL+C')
    channel.start_consuming()

if __name__ == '__main__':
    try:
        argLen = len(sys.argv)
        if argLen >= 1:
            address = 'localhost'
            queue = 'hello'
            if argLen >= 2:
                for i in sys.argv[1:]:
                    if i[0:2] == "-a":
                        address = i[2:]
                        print("address set to:"+address)
                    elif i[0:2] == "-1":
                        queue = i[2:]
                        print("queue set to:"+queue)
                    else:
                        print("unknown argument: \""+i+"\"")
            else:
                print("starting myConsumer()")
                print("  set address with: \"-a[address]\" (defult is '"+address+"')")
                print("  set queue with:   \"-q[queue]\"   (default is '"+queue+"')")
            myConsumer(address, queue)
        else:
            print("0 argumnents (?)")
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)
