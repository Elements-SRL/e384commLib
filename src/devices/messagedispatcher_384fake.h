#ifndef MESSAGEDISPATCHER_384FAKE_H
#define MESSAGEDISPATCHER_384FAKE_H

#include "messagedispatcher_384nanopores.h"

class MessageDispatcher_384Fake : public MessageDispatcher_384NanoPores_V01 {
public:
    MessageDispatcher_384Fake(string id);
    virtual ~MessageDispatcher_384Fake();

    virtual ErrorCodes_t connect() override;
    virtual ErrorCodes_t disconnect() override;

protected:

    /*************\
     *  Methods  *
    \*************/

    virtual void sendCommandsToDevice() override;
    virtual void readDataFromDevice() override;
};

#endif // MESSAGEDISPATCHER_384FAKE_H
