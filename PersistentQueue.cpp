/*
 * PersistentQueue.cpp - SD backed queue with persistent pointer  
 * Copyright (c) David Grandinetti 2012
 */

int chip_select, String queue_file, String pointer_file

PQueue::PQueue(int chip_select, String queue_file, String pointer_file)
{
  SPI_CS = chip_select;
  queue_file_name = queue_file;
  queue_pointer_file_name = pointer_file;

  //
  // can we test that this has not already been done?
  //
  Serial.begin(9600);
}

void PQueue::initialize_queue(void) {
  Serial.print("\nInitializing SD card...");
  pinMode(SPI_CS, OUTPUT);     // change this to 53 on a mega

  if (!SD.begin(SPI_CS)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open or create a readings.csv file
  File readingsFile = SD.open(queue_file_name);

  // test that file has been opened
  if(readingsFile) {
    Serial.println("Readings.csv has been created/opened");
    while (readingsFile.available()) {
      Serial.write(readingsFile.read());
    }
    Serial.print("Readings.txt length = ");
    Serial.println(readingsFile.size());
    readingsFile.close();
  } else {
    Serial.println("Readings.csv has NOT BEEN OPENED!");
  }
  
  // reset message queue pointer
  set_last_sent(0);
  Serial.print("last_sent = ");
  Serial.println(get_last_sent());
}


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
unsigned long get_last_sent(){
    //
  // open the file for writing, and truncate the file length to 0
  //
  File pointerFile = SD.open(queue_pointer_file_name, FILE_READ);

  char line[20];
  int i=0;

  if (pointerFile)
  {
    while( pointerFile.available() )
    {
      line[i++] = pointerFile.read();
    }
    line[i++]='\0';
    return atoi(line);
  }
  else
  {
    return 0;
  }
}

//
// Updates the last_sent.txt file to point to an offset within the readings.csv
//
boolean set_last_sent(unsigned long offset) {
  //
  // open the file for writing, and truncate the file length to 0
  //
  File pointerFile = SD.open(queue_pointer_file_name, (O_TRUNC | O_WRITE));

  if (pointerFile) {
    pointerFile.println(String(offset));
    pointerFile.close();
    return true;
  }  
  else {
    return false;
  }
}

//
// Append a new message onto the end of readings.csv.
//
boolean write_new_message(String message) {

  File dataFile = SD.open(queue_file_name, FILE_WRITE);

  if (dataFile) {
    dataFile.println(message);
    dataFile.close();
    return true;
  }  
  else {
    return false;
  }
}

//
// Check if there are any unsent messages.
//
boolean has_queued_message() {
  long last_sent = get_last_sent();
  unsigned long length_of_readings = 0;

  File dataFile = SD.open(READINGS_FILE, FILE_READ);

  if (dataFile)
  {
    length_of_readings = dataFile.size();
    dataFile.close();
    if( length_of_readings > last_sent + 3 )
    {
      return true;
    }
  }  
  else
  {
    return false;
  }
}

//
// Returns the next queued message by using last_sent.txt as a pointer into readings.csv
//
String get_next_queued_message() {
  File dataFile = SD.open(READINGS_FILE, FILE_READ);

  if (dataFile) {
    dataFile.seek(get_last_sent());

    //
    // loop as long as:
    //  - more characters available from the file
    //  - the next character is not a newline
    //  - the next character is not an EOF
    //  - the buffer is not full
    //
    int i=0;
    char line[100];
    boolean done = false;
//    while( dataFile.available() && ( dataFile.peek() != '\n' ) && ( dataFile.peek() != EOF) && i < 99 )
    while( !done )
    {
      if( dataFile.available() )
      {
        char next = dataFile.peek();
        if( next == EOF || next == '\n' || next == '\r' )
        {
          done = true;
        }
        else
        {
          line[i++] = dataFile.read();
        }
      }
      else
      {
        done = true;
      }
    }
    // then null terminate the string, and return it
    line[i++]='\0';
    dataFile.close();

    return String(line);
  }
  else
  {
    return String("");
  }
}

//
// Returns 
//  - true  if we could advance the message pointer
//  - false if we have reached the end of the queue
//
boolean advance_to_next_message() {
  unsigned long last_sent = get_last_sent();
  unsigned long length_of_readings = 0;

  File dataFile = SD.open(READINGS_FILE, FILE_READ);

  if (dataFile) {

    //
    // we may alrady be at the end of the message queue
    //
    if( last_sent +2 > dataFile.size() )
    {
      return false;
    }
    dataFile.seek(last_sent);

    //
    // loop as long as:
    //  - more characters available from the file
    //  - the next character is not a newline
    //  - the buffer is not full
    //
    int i=0;
    char next;
    while( dataFile.available() && next != '\n')
    {
      next = dataFile.read();
    }
    unsigned long next_position = dataFile.position();
    dataFile.close();
      
    set_last_sent(next_position);
    return true;
  }
  else
  {
    return false;
  }
}
