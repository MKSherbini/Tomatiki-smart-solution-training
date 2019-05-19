#include <Arduino.h>
#include <fs.h>

#define READING_TIME_INTERVAL_MS (100)
#define TABLE_SIZE (20)
#define _1Sec_IN_US (1e6)
#define _1H_IN_US (60 * 60 * _1Sec_IN_US)
#include <time.h>

char filename[] = "/test.txt";
char config_filename[] = "/config.txt"; // I could also use the first line in /test.txt as it's currently not used, but i wanted to keep this file in case I need to add other configs.
u16 sensorValue;
File myDataFile;
unsigned long read_timer = 0;

typedef struct
{
  uint32_t id;
  uint32_t time;
  uint16_t sensorValue;
} Record;

#define RECORD_SIZE (28 + 1)

Record sensor_data[TABLE_SIZE + 1];
uint32_t log_count = 1;

void delete_record(uint32_t offset);
void reload_db();
void print_record_I(uint32_t i);
void print_record(uint32_t i);
void add_record(uint32_t id, uint16_t sensorValue, uint32_t time);
void add_record_I(uint32_t i, uint16_t sensorValue, uint32_t time);
void update_record(uint32_t i, uint32_t id, uint16_t sensorValue, uint32_t time);
uint32_t select_record_I_byID(uint32_t id);
Record select_record(uint32_t i);
void load_config();
void update_config();

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  SPIFFS.begin();
  Serial.println();

  // Serial.println(select_record(1).time);
  load_config();
  // Serial.println("setup " + String(log_count));
}

void loop()
{
  // put your main code here, to run repeatedly:
  // Serial.println("loop " + String(log_count));

  add_record_I(log_count, analogRead(A0), millis() + select_record((log_count) == 1 ? TABLE_SIZE : log_count - 1).time + _1H_IN_US);
  log_count++;
  if (log_count > TABLE_SIZE)
    log_count = 1;

  update_config();
  ESP.deepSleep(_1H_IN_US);
  delayMicroseconds(_1Sec_IN_US);
}
void load_config()
{
  if (!SPIFFS.exists(config_filename))
    return;
  myDataFile = SPIFFS.open(config_filename, "r");
  String read_ahead = myDataFile.readStringUntil(':');
  if (read_ahead == "log_count")
  {
    int temp = myDataFile.parseInt();
    log_count = temp;
    Serial.println("log_count loaded: " + String(temp));
  }
  else
  {
    SPIFFS.remove(config_filename);
    Serial.println("log_count header check failed, file removed");
  }
  myDataFile.close();
}

void update_config()
{
  myDataFile = SPIFFS.open(config_filename, "w+");
  char clog_count[10];
  sprintf(clog_count, "%10d", log_count);
  String slog_count(clog_count);
  myDataFile.println("log_count: " + slog_count);
  myDataFile.close();
}

void delete_record(uint32_t i)
{
  // myDataFile = SPIFFS.open(filename, "r+");
  // myDataFile.seek(i * (RECORD_SIZE), SeekSet);
  // uint32_t del = 0;
  // while (myDataFile.read() != '\n')
  // {
  //   del++;
  // }
  // myDataFile.seek(i * RECORD_SIZE, SeekSet);
  // int d=RECORD_SIZE;
  // Serial.println(d);
  // Serial.println(del);
  // while (del--)
  // {
  //   myDataFile.write('x');
  // }
  // myDataFile.close();
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i * (RECORD_SIZE), SeekSet);
  char cid[10];
  char csensorValue[5];
  char ctime[10];
  sprintf(cid, "%10d", 0);
  String sid(cid);
  sprintf(csensorValue, "%5d", 0);
  String ssensorValue(csensorValue);
  sprintf(ctime, "%10d", 0);
  String stime(ctime);
  myDataFile.println(sid + char(9) + ssensorValue + char(9) + stime);
  myDataFile.close();
}
void add_record_I(uint32_t i, uint16_t sensorValue, uint32_t time)
{
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);

  char cid[10];
  char csensorValue[5];
  char ctime[10];
  sprintf(cid, "%10d", i);
  String sid(cid);
  sprintf(csensorValue, "%5d", sensorValue);
  String ssensorValue(csensorValue);
  sprintf(ctime, "%10d", time);
  String stime(ctime);
  myDataFile.println(sid + char(9) + ssensorValue + char(9) + stime);

  Serial.println("New Record Added " + String(i) + char(9) + String(sensorValue) + char(9) + String(time));
  myDataFile.close();
}
void add_record(uint32_t id, uint16_t sensorValue, uint32_t time)
{
  myDataFile = SPIFFS.open(filename, "a+");
  char cid[10];
  char csensorValue[5];
  char ctime[10];
  sprintf(cid, "%10d", id);
  String sid(cid);
  sprintf(csensorValue, "%5d", sensorValue);
  String ssensorValue(csensorValue);
  sprintf(ctime, "%10d", time);
  String stime(ctime);
  myDataFile.println(sid + char(9) + ssensorValue + char(9) + stime);
  Serial.println("New Record Added " + String(id) + char(9) + String(sensorValue) + char(9) + String(time));
  myDataFile.close();
}
void update_record(uint32_t i, uint32_t id, uint16_t sensorValue, uint32_t time)
{
  delete_record(i);
  myDataFile = SPIFFS.open(filename, "r+");
  myDataFile.seek(i * RECORD_SIZE, SeekSet);

  char cid[10];
  char csensorValue[5];
  char ctime[10];
  sprintf(cid, "%10d", id);
  String sid(cid);
  sprintf(csensorValue, "%5d", sensorValue);
  String ssensorValue(csensorValue);
  sprintf(ctime, "%10d", time);
  String stime(ctime);
  myDataFile.println(sid + char(9) + ssensorValue + char(9) + stime);

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
  Serial.println("record " + String(i) + " " + String(sensor_data[i].id) + ": " + char(9) + String(sensor_data[i].sensorValue) + char(9) + String(sensor_data[i].time));
}
void print_record(uint32_t i)
{
  Record r = select_record(i);
  Serial.println("record " + String(r.id) + ": " + char(9) + String(r.sensorValue) + char(9) + String(r.time));
}
Record select_record(uint32_t i)
{
  myDataFile = SPIFFS.open(filename, "r");
  myDataFile.seek(i * (RECORD_SIZE), SeekSet);
  Record r = {0};
  uint32_t read_ahead = myDataFile.parseInt();
  // if(read_ahead!=0){
  r.id = read_ahead;
  r.sensorValue = myDataFile.parseInt();
  r.time = myDataFile.parseInt();
  // }

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