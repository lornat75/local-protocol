#include <stdio.h>

#include <yarp/os/RateThread.h>
#include <yarp/os/Time.h>
#include <yarp/os/Network.h>
#include <yarp/os/Port.h>
#include <yarp/os/BufferedPort.h>

#include <yarp/sig/Vector.h>

using namespace yarp::os;
using namespace yarp::sig;


class Consumer : public RateThread
{
public:
    Consumer(int p = 0) : RateThread(p)
    {
        int c = 0;
    }

private:
    BufferedPort<Vector> input;
    BufferedPort<Vector> output;

    int c;
    Vector *dataIn;
    bool connected;

    void run()
    {
        if (!connected)
        {
           // while (!Network::isConnected("/consumer/out", "/producer/in"))
             //   Time::delay(1);
            //while (!Network::isConnected("/producer/out", "/consumer/in"))
                Time::delay(10);
            connected = true;
        }

        //read blocking
        dataIn = input.read();

        while (true)
        {
            std::cout << (*dataIn)[0] << "\n";
            Time::delay(0.1);
        }

        Vector &dataOut = output.prepare();
        dataOut = *dataIn; //copy
        output.write();

    }

    bool threadInit()
    {
        std::cout << "Consumer::Calling init\n";

        if (!input.open("/consumer/in"))
            return false;


        if (!output.open("/consumer/out"))
            return false;

        dataIn = NULL;
        connected = false;

        return true;
    }

    void threadRelease()
    {
        input.close();
        output.close();
    }

};


class Producer : public RateThread
{
public:
    Producer(int p = 10) : RateThread(p)
    {
        int c = 0;
    }

private:
    BufferedPort<Vector> input;
    BufferedPort<Vector> output;
    int c;
    double accDt;
    bool connected;

    void run()
    {

        if (!connected)
        {
            //while (!Network::isConnected("/consumer/out", "/producer/in"))
            //    Time::delay(1);
            //while (!Network::isConnected("/producer/out", "/consumer/in"))
            Time::delay(10);
            connected = true;
        }


        Vector &dataOut = output.prepare();
        dataOut.resize(1000000);
        dataOut[0] = c++;
        dataOut[1] = Time::now();

        output.write();

        Vector &dataOut1 = output.prepare();
        dataOut1.resize(1000000);
        while (true)
        {
            dataOut1[0]++;
            Time::delay(0.3);
        }

        Vector *inData = input.read();
        double now = Time::now();

        // compute timing
        if (c % 100 == 0)
        {
            double average = accDt / (double)c;
            std::cout << "Average round trip time " << 1000 * average << "\n";
            accDt = 0.0;
            c = 0;
        }
        else
        {
            if (c != ((*inData)[0] + 1))
                std::cout << "Error: dropped message?\n";
        }

        accDt += (now - (*inData)[1]);

    }

    bool threadInit()
    {
        std::cout << "Producer::Calling init\n";

        c = 0;

        if (!output.open("/producer/out"))
            return false;

        if (!input.open("/producer/in"))
            return false;

        c = 0;
        accDt = 0.0;

        connected = false;

        return true;
    }

    void threadRelease()
    {
        input.close();
        output.close();
    }

};

int main()
{
    Network yarp;

    Producer producer;
    Consumer consumer;

    producer.start();
    consumer.start();

    char c = 0;
    while (c != 'q')
    {
        printf("Hit q to quit\n");
        scanf("%c", &c);
    }

    producer.stop();
    consumer.stop();

    return 0;
}
