void setup() {
  // put your setup code here, to run once:
  Serial.begin(1000000);
  Serial1.begin(1000000);
}

class Comm
{
public:
  Comm()
  {
    CommCount = 0;
    RevSuc = 0;
  };
  long Explain(uint8_t bDat, char *CommStr)
  {
    if(bDat=='\n'){
      if(RevSuc==2){
        CommBuf[CommCount] = 0;
        //Serial.println(CommBuf);
        SubStr[0] = strtok(CommBuf, " ");
        SubStr[1] = strtok(NULL, " ");
        if(SubStr[0]==NULL || SubStr[1]==NULL){
          return -1;
        }
        if(!strcmp(CommBuf, CommStr)){
          return atol(SubStr[1]);
        }
        RevSuc = 0;
        CommCount = 0;
      }
    }else if(bDat=='\r'){
      RevSuc++;
    }else if(bDat=='#'){
      RevSuc = 1;
      CommCount = 0;
    }else{
      if(CommCount<127){
        CommBuf[CommCount] = bDat;
        CommCount++;
      }
    }
    return -1;
  }
private:
  char CommBuf[128];
  char *SubStr[2];
  uint8_t CommCount;
  uint8_t RevSuc;
};

Comm comm;

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available()){
    uint8_t bDat = Serial.read();
    Serial1.write(bDat);
    long Baud = comm.Explain(bDat, "BAUD");
    if(Baud!=-1 && Baud>=9600){
      //Serial.println(Baud);
      Serial.flush();
      Serial1.flush();
      Serial.begin(Baud);
      Serial1.begin(Baud);
    }
  }
  if(Serial1.available()){
    Serial.write(Serial1.read());
  }
}
