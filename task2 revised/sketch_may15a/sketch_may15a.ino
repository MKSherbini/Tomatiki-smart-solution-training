#include <fs.h>
char filename [] = "test.txt";
int sensorValue;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  //  if (SPIFFS.exists(filename)) {
  //    SPIFFS.remove(filename);  // check to see if a file already exists, if so delete it
  //    Serial.println("deleting..");
  //  }

  File myDataFile = SPIFFS.open(filename, "a+");        // Open a file for reading and writing (appending)
  if (!myDataFile)Serial.println("file open failed");   // Check for errors

  sensorValue = analogRead(A0);
  myDataFile.println(sensorValue); // Write some data to it
  //  Serial.println(myDataFile.size());                    // Display the file size (26 characters + 4-byte floating point number + 6 termination bytes (2/line) = 34 bytes)
  myDataFile.close();                                   // Close the file

  //  myDataFile = SPIFFS.open(filename, "r");              // Open the file again, this time for reading
  //  if (!myDataFile) Serial.println("file open failed");  // Check for errors
  //  while (myDataFile.available()) {
  //    Serial.write(myDataFile.read());                    // Read all the data from the file and display it
  //  }
  //  myDataFile.close();                                   // Close the file
  delay(100); // wait and then do it all again
}
