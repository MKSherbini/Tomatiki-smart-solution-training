#include <Arduino.h>
#include <fs.h>

#define READING_TIME_INTERVAL_MS (100)
#define TABLE_SIZE 10
#include <time.h> 

char filename[] = "/test.txt";
u16 sensorValue;
File myDataFile;
unsigned long read_timer = 0;

typedef struct {
  uint32_t     id; 
  uint32_t time; 
  uint16_t sensorValue; 
} Record;

#define RECORD_SIZE (sizeof(Record))

Record sensor_data[TABLE_SIZE+1];
uint32_t log_count=1;

void delete_record(uint32_t offset);
void reload_db();
void print_record_I(uint32_t i);
void update_record(uint32_t i,uint32_t id,uint32_t time,uint16_t sensorValue);
uint32_t select_record(uint32_t id);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println();

  SPIFFS.remove(filename); 
  SPIFFS.remove("test.txt"); 
  myDataFile = SPIFFS.open(filename, "w+");
  myDataFile.println(String(log_count++)+char(9)+String(1111)+char(9)+String(millis())); 
  Serial.println("New Record Added "+String(log_count)+char(9)+String(1111)+char(9)+String(millis()));
  myDataFile.println(String(log_count++)+char(9)+String(2222)+char(9)+String(millis())); 
  Serial.println("New Record Added "+String(log_count)+char(9)+String(2222)+char(9)+String(millis()));
  myDataFile.println(String(log_count++)+char(9)+String(2222)+char(9)+String(millis())); 
  Serial.println("New Record Added "+String(log_count)+char(9)+String(2222)+char(9)+String(millis()));
  myDataFile.close();  

 
  update_record(1,5,60,100);
  update_record(select_record(5),4,60,100);

  reload_db();
  print_record_I(0);
  print_record_I(1);
  print_record_I(2);

  myDataFile = SPIFFS.open(filename, "r+");              // Open the file again, this time for reading
  if (!myDataFile) Serial.println("file open failed");  // Check for errors
  Serial.println(myDataFile.size()); 
  delete_record(4);
  while (myDataFile.available()) {
    Serial.write(myDataFile.read());                    // Read all the data from the file and display it
  }
  myDataFile.close();          

//  myDataFile = SPIFFS.open(filename, "a+");
//  myDataFile.close();  
// Dir dir = SPIFFS.openDir("/");
// while (dir.next()) {
//     Serial.print(dir.fileName()+" ");
//     if(dir.fileSize()) {
//         File f = dir.openFile("r");
//         Serial.println(f.size());
//     }
// }
}

void loop()
{
  // put your main code here, to run repeatedly:


  // if (millis() - read_timer > READING_TIME_INTERVAL_MS)
  // {
  //   myDataFile = SPIFFS.open(filename, "a+");
  //   if (!myDataFile)
  //   {
  //     Serial.println("file open failed"); // Check for errors
  //     return;
  //   }
  //   sensorValue = analogRead(A0);
  //   myDataFile.println(sensorValue); // Write some data to it
  //   myDataFile.close();              // Close the file
  //   read_timer = millis();
  // }
}
void delete_record(uint32_t i){
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i*RECORD_SIZE, SeekSet);
  uint32_t del=1;
  while(myDataFile.read()!='\n'){del++; }
  myDataFile.seek(i*RECORD_SIZE, SeekSet);
  while(del--){myDataFile.write(0); }
  myDataFile.close();
}
void update_record(uint32_t i,uint32_t id,uint32_t time,uint16_t sensorValue){
  delete_record(i);
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i*RECORD_SIZE, SeekSet);
  myDataFile.println(String(id)+char(9)+String(sensorValue)+char(9)+String(time)); 
  Serial.println("Record Updated "+String(id)+char(9)+String(sensorValue)+char(9)+String(time));
  myDataFile.close();  
}
void reload_db(){
  myDataFile=SPIFFS.open(filename, "r");
  log_count=0;
  while (myDataFile.available()) { 
    uint32_t read_ahead = myDataFile.parseInt(); 
    if (read_ahead != 0) { 
      sensor_data[log_count].id  = read_ahead;
      sensor_data[log_count].sensorValue  = myDataFile.parseInt();
      sensor_data[log_count].time = myDataFile.parseInt();
      log_count += 1;
    }
  }
  myDataFile.close();  
}
void print_record_I(uint32_t i){
   Serial.println("record "+String(sensor_data[i].id)+": "+char(9)+String(sensor_data[i].sensorValue)+char(9)+String(sensor_data[i].time));
}

uint32_t select_record(uint32_t id){
  myDataFile=SPIFFS.open(filename, "r");
  for (size_t i = 0; i < TABLE_SIZE; i++)
  {
    myDataFile.seek(i*RECORD_SIZE, SeekSet);
    if(myDataFile.parseInt()==id){
      myDataFile.close();  
      return i;
    }
  }
  myDataFile.close();  
 return 0;
}