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
    BufferedPort<Vector> p;
    int c; 
    Vector *data;

    void run()
    {

	if (c<500)
	    data=p.read(false);

        if (data != NULL)
	{
            std::cout << "\n[0] Got data " << data->toString() << "\n";
	    c++;
	}

    }

    bool threadInit()
    {
        std::cout << "Consumer::Calling init\n";

        if (!p.open("/consumer"))
            return false;
        else
            return true;

        data=NULL;
    }

    void threadRelease()
    {
        p.close();
    }

};


class Producer: public RateThread
{
public: 
    Producer(int p = 10) : RateThread(p)
    {
        int c = 0;
    }
   
private:
    Port p;
    int c;
    Vector b;

    void run()
    {
        b.clear();
        b.resize(1);
        c++;
        b = c;

	if (c<1000)
	    p.write(b);
    }

    bool threadInit()
    {
        std::cout << "Producer::Calling init\n";
        
        c = 0;
 
        if (!p.open("/producer"))
            return false;
        else
            return true;

      
    }

    void threadRelease()
    {
        p.close();
    }

};

int main()
{
    Network yarp;

    Producer producer;
    Consumer consumer;

    producer.start();
    consumer.start();

    char c=0;
    while(c!='q')
    {
        printf("Hit q to quit\n");
        scanf("%c", &c);
    }

    producer.stop();
    consumer.stop();

    return 0;  
}
