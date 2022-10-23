#!/usr/bin/env python
import pika, sys, os


class callbackHandler:
    def __init__(self, channel_i, connection_i, count_i = -1):
        self.count = count_i
        self.channel = channel_i
        self.connection = connection_i
   
    def closeConnectionAndChannel(self):
        self.channel.close()
        self.connection.close()

    # callback to be used by Pika library
    def callback(self, ch, method, properties, body):
        print(" [x] Received %r" % body)
        if self.count >= 0:
            self.count-=1
            if self.count==0:
                self.closeConnectionAndChannel()
        


def myConsumer(address_i, queue_i, c):
    # create identical connection as send.py
    connection = pika.BlockingConnection(pika.ConnectionParameters(address_i))
    channel = connection.channel()

    myCallbackHandler = callbackHandler(channel, connection, c)

    try:
        # queue declaration is idempotent - only declares once
        channel.queue_declare(queue=queue_i)

        channel.basic_consume(  queue=queue_i,
                                auto_ack=True,
                                on_message_callback=myCallbackHandler.callback)

        print('[*] Waiting for messages.  To exit press CTRL+C')
        channel.start_consuming()
    except KeyboardInterrupt:
        print('Interrupting in myConsumer()')
        myCallbackHandler.closeConnectionAndChannel()
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)

if __name__ == '__main__':
        argLen = len(sys.argv)
        if argLen >= 1:
            address = 'localhost'
            queue = 'hello'
            count = -1
            if argLen >= 2:
                for i in sys.argv[1:]:
                    if i[0:2] == "-a":
                        address = i[2:]
                        print("address set to:"+address)
                    elif i[0:2] == "-1":
                        queue = i[2:]
                        print("queue set to:"+queue)
                    elif i[0:2] == "-c":
                        count = int(i[2:])
                    else:
                        print("unknown argument: \""+i+"\"")
            else:
                print("starting myConsumer()")
                print("  set address with: \"-a[address]\" (defult is '"+address+"')")
                print("  set queue with:   \"-q[queue]\"   (default is '"+queue+"')")
                print("  set count with:   \"-c[count]\"   (default is '"+str(count)+"')")
            myConsumer(address, queue, count)
        else:
            print("0 argumnents (?)")
