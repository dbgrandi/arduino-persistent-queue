#ifndef P_QUEUE
#define P_QUEUE

// library interface description
class PQueue
{
  // user-accessible "public" interface
  public:
    PQueue(int chip_select, String queue_file, String pointer_file);
    void initialize();

    //
    // Returns an unsigned_long which is a pointer into the readings.csv file
    //
    // Assumptions:
    //  - last_sent.txt has only a single line, which is definitely less than 19 characters long.
    //
    // Returns
    //  - long that represents the offset into the results.csv file where you'll get
    //    the next unsent message
    //
    unsigned long get_last_sent();

    //
    // Updates the last_sent.txt file to point to an offset within the readings.csv
    //
    boolean set_last_sent(unsigned long offset);

    //
    // Append a new message onto the end of readings.csv.
    //
    boolean write_new_message(String message);

    //
    // Check if there are any unsent messages.
    //
    boolean has_queued_message();

    //
    // Returns the next queued message by using last_sent.txt as a pointer into readings.csv
    //
    String get_next_queued_message();

  private:
    /* This one is driven LOW for chip selectzion */
    int SPI_CS;

    /* Filename for queue */
    String queue_file_name;

    /* Filename for queue_pointer */
    String queue_pointer_file_name;

};

#endif