PersistentQueue
===============

The PersistentQueue allows you to store a time ordered series of messages on
an SD card. It is useful if you are (e.g.) trying to take a series of readings
and have a less than reliable network connection. We had just this problem
when trying to use a GSM connection and found it unusable every day during
rush hour. So, while the GSM network is overloaded, we queue up messages to
be sent out at a later time.

Dependencies
------------

PersistentQueue works directly on an `SD` object, as defined in `SD.h`. This
library assumes you are using one of the newer versions of the SD library that
allows more than 1 open file at a time.

While the PersistentQueue works best if you have a reliable time source,
prefereablly a RTC module, we use the Arduino chip time. It is up to you to
make sure that is accurate. This can be done by using the the RTC module as
a sync source for the Arduino chip.

Initialization
--------------

    #include "PersistantQueue.h"
    
    PersistentQueue p_queue;
    
    void setup()
    {
      p_queue = new PersistantQueue();
    }

API
---

1 - Create a p_queue object and point it to your SD card
2 - Write a record
3 - Get record count
4 - Manipulate the cursor

This code is released under the MIT LICENSE.