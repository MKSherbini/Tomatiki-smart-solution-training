#include <Arduino.h>
#include <fs.h>

#define READING_TIME_INTERVAL_MS (100)
#define TABLE_SIZE (20)
#include <time.h>

char filename[] = "/test.txt";
u16 sensorValue;
File myDataFile;
unsigned long read_timer = 0;

typedef struct
{
  uint32_t id;
  uint32_t time;
  uint16_t sensorValue;
} Record;

#define RECORD_SIZE (sizeof(Record))

Record sensor_data[TABLE_SIZE + 1];
uint32_t log_count = 1;

void delete_record(uint32_t offset);
void reload_db();
void print_record_I(uint32_t i);
void add_record(uint32_t id, uint16_t sensorValue, uint32_t time);
void add_record_I(uint32_t i, uint16_t sensorValue, uint32_t time);
void update_record(uint32_t i, uint32_t id, uint32_t time, uint16_t sensorValue);
uint32_t select_record_I_byID(uint32_t id);
Record select_record(uint32_t i);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println();

  wdt_disable();

  Serial.println(select_record(0).sensorValue);
}

void loop()
{
  // put your main code here, to run repeatedly:
  // delay(10);
  if (millis() - read_timer > READING_TIME_INTERVAL_MS)
  {
    add_record_I(log_count++, analogRead(A0), millis());
    if (log_count > TABLE_SIZE)
      log_count = 1;
    read_timer = millis();
  }
}

void delete_record(uint32_t i)
{
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);
  uint32_t del = 1;
  while (myDataFile.read() != '\n')
  {
    del++;
  }
  myDataFile.seek(i * RECORD_SIZE, SeekSet);
  while (del--)
  {
    myDataFile.write(0);
  }
  myDataFile.close();
}
void add_record_I(uint32_t i, uint16_t sensorValue, uint32_t time)
{
  myDataFile = SPIFFS.open(filename, "a+");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);
  myDataFile.println(String(i) + char(9) + String(sensorValue) + char(9) + String(time));
  Serial.println("New Record Added " + String(i) + char(9) + String(sensorValue) + char(9) + String(time));
  myDataFile.close();
}
void add_record(uint32_t id, uint16_t sensorValue, uint32_t time)
{
  myDataFile = SPIFFS.open(filename, "a+");
  myDataFile.println(String(id) + char(9) + String(sensorValue) + char(9) + String(time));
  Serial.println("New Record Added " + String(id) + char(9) + String(sensorValue) + char(9) + String(time));
  myDataFile.close();
}
void update_record(uint32_t i, uint32_t id, uint32_t time, uint16_t sensorValue)
{
  delete_record(i);
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);
  myDataFile.println(String(id) + char(9) + String(sensorValue) + char(9) + String(time));
  Serial.println("Record Updated " + String(id) + char(9) + String(sensorValue) + char(9) + String(time));
  myDataFile.close();
}
void reload_db()
{
  myDataFile = SPIFFS.open(filename, "r");
  log_count = 1;
  while (myDataFile.available())
  {
    uint32_t read_ahead = myDataFile.parseInt();
    if (read_ahead != 0)
    {
      sensor_data[log_count].id = read_ahead;
      sensor_data[log_count].sensorValue = myDataFile.parseInt();
      sensor_data[log_count].time = myDataFile.parseInt();
      log_count += 1;
    }
  }
  myDataFile.close();
}
void print_record_I(uint32_t i)
{
  Serial.println("record " + String(sensor_data[i].id) + ": " + char(9) + String(sensor_data[i].sensorValue) + char(9) + String(sensor_data[i].time));
}
Record select_record(uint32_t i)
{
  myDataFile = SPIFFS.open(filename, "r");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);
  Record r = {0};
  uint32_t read_ahead = myDataFile.parseInt();
  if (read_ahead != 0)
  {
    r.id = read_ahead;
    r.sensorValue = myDataFile.parseInt();
    r.time = myDataFile.parseInt();
  }
  myDataFile.close();
  return r;
}
uint32_t select_record_I_byID(uint32_t id)
{
  myDataFile = SPIFFS.open(filename, "r");
  for (size_t i = 0; i < TABLE_SIZE; i++)
  {
    myDataFile.seek(i * RECORD_SIZE, SeekSet);
    if (myDataFile.parseInt() == id)
    {
      myDataFile.close();
      return i;
    }
  }
  myDataFile.close();
  return 0;
}