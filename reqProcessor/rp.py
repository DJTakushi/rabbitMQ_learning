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


# callback to be used by Pika library for requests queue
def requestsCB(ch, method, properties, body):
    print(" [x] Received %r" % body)
    singlePublish('localhost', 'hello', ["reply: %r" % body])


def main(address):
    connection = pika.BlockingConnection(pika.ConnectionParameters(address))
    channel = connection.channel()

    queueName = "reply"
    # queue declaration is idempotent
    channel.queue_declare(queue=queueName)

    channel.basic_consume(  queue=queueName,
                            auto_ack=True,
                            on_message_callback=requestsCB)
    
    print('[*] Waiting for messages.  To exit press CTRL+C')
    channel.start_consuming()

if __name__ == '__main__':
    try:
        address = 'localhost'
        main(address)
    except KeyboardInterrupt:
        print('Interrupted')
        try:
            sys.exit(0)
        except SystemExit:
            os._exit(0)