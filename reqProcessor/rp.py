#!/usr/bin/env python
import pika, sys, os

def singlePublish(address_i, routingKey_i, messages):
    connection = pika.BlockingConnection(pika.ConnectionParameters(address_i))
    channel = connection.channel()
    channel.queue_declare(queue=routingKey_i)
    for m in messages:
        channel.basic_publish(exchange='', # default value
                              routing_key=routingKey_i,
                              body=m)
        print(" [x] Sent '"+m+"'")
    connection.close()

class callbackHandler:
    # extended callback functionality per:
    # https://stackoverflow.com/questions/12855332/how-could-i-pass-parameter-to-pika-callback
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

        #publish to 'reply' queue
        singlePublish('localhost', 'reply', ["reply: %r" % body])
        if self.count >= 0:
            self.count-=1
            if self.count==0:
                self.closeConnectionAndChannel()

def main(address,listenQueue,):
    connection = pika.BlockingConnection(pika.ConnectionParameters(address))
    channel = connection.channel()
    myCallbackHandler = callbackHandler(channel, connection)
    try:
        # queue declaration is idempotent
        channel.queue_declare(queue=listenQueue)
        channel.basic_consume(  queue=listenQueue,
                                auto_ack=True,
                                on_message_callback=myCallbackHandler.callback)

        print('[*] Waiting for messages.  To exit press CTRL+C')
        channel.start_consuming()
    except KeyboardInterrupt:
        print('Interrupting in main()')
        myCallbackHandler.closeConnectionAndChannel()
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)

if __name__ == '__main__':
    address = 'localhost'
    listenQueue='request'
    main(address,listenQueue)
