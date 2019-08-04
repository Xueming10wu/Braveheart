#include "bamboo/ArmProtocol.h"


int main(int argc, char *argv[])
{
    //cout << "Hello arm" << endl;
    ArmProtocol protocol = ArmProtocol();
    protocol.readJoint(263);

    return 0;
}