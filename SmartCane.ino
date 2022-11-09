#include <SoftwareSerial.h>
#define BT_RXD 6
#define BT_TXD 5
SoftwareSerial btSerial(BT_RXD, BT_TXD);

//초기화
int disBuzzer = 40; //거리 값 초기화
int disMotor = 40; //거리 값 초기화

// 조도센서
long led_previousMillis = 0;  //led 이전시간

// 초음파 센서
long ultraLow_previousMillis = 0; // 초음파 센서 LOW 이전시간
long ultraHigh_previousMillis = 0;  // 초음파 센서 HIGH 이전시간

// 피에조부저
long buzzerDis_previousMillis = 0; // 부저 관련 초음파 거리 이전시간
long buzzer_previousMillis = 0;  // 피에조 부저 이전시간

// 모터
long vibDis_previousMillis = 0;   // 모터 관련 초음파 거리 이전시간
long vib_previousMillis = 0;   // 진동 모터 이전시간

// 블루투스
long blue_previousMillis = 0;   // 블루투스 이전시간

long start_time = 0;

void setup() {
  btSerial.begin(9600); // 블루투스 통신
  Serial.begin(9600); // 시리얼 통신

  digitalWrite(7, LOW); // 피에조 부저 LOW

  pinMode(A0, INPUT); //조도 센서
  pinMode(8, OUTPUT); //LED
  pinMode(7, OUTPUT); //피에조 부저
  pinMode(9, OUTPUT); //진동모터

}

void loop() {
  int readValue = digitalRead(2); //스위치 상태값 읽어오기
  //  long current_time = millis();
  //  if(current_time - start_time > 1000) {

  if (readValue == HIGH) {  // 스위치가 켜져있으면

    led_loop();   // 조도센서 & led

    int disBuz = buzzer_loop();   // 부저 거리
    int disVib = vib_loop();  // 진동 모터 거리


    unsigned long currentMillis = millis(); // 현재시간
    if (currentMillis - blue_previousMillis >= 2000) {  // 2초
      blue_previousMillis = currentMillis;

      if ( ( disVib > 0 and disVib <= 80) or ( disBuz > 0 and disBuz <= 80 ) ) {
        
        if ( disVib > disBuz) {
          btSerial.print(disBuz);
        }
        else if ( disVib < disBuz){
          btSerial.print(disVib);
        }
      }
    }
  }
  else {
    analogWrite(9, 0);
    noTone(7);  //부저 소리 끄기
    digitalWrite(8, LOW);
  }
}
//}


//조도센서 led 루프
void led_loop() {

  unsigned long currentMillis = millis(); //현재 시간 값 받아오기

  if (currentMillis - led_previousMillis >= 1000) {    // 1초
    led_previousMillis = currentMillis; //이전 시간에 측정한 현재 시간 저장

    int input = analogRead(A0); //조도 센서 값 읽어오기
    Serial.println(input);
    if (input > 150) { // 밝을 경우
      digitalWrite(8, LOW);    //LED OFF
    } else {          // 어두울 경우
      digitalWrite(8, HIGH);   //LED ON
      Serial.println("켜짐");
    }
  }
}


//초음파 함수
long readUltrasonicDistance(int triggerPin, int echoPin)  //초음파 센서 함수 ( 트리거핀, 에코핀)
{
  pinMode(triggerPin, OUTPUT);  //트리거 핀 출력
  unsigned long currentMillis = millis(); //현재 시간 값 받아오기

  if (currentMillis - ultraLow_previousMillis >= 200) {
    digitalWrite(triggerPin, LOW);
    digitalWrite(echoPin, LOW);

  }

  pinMode(echoPin, INPUT);    //초음파 센서 echoPin = 입력핀

  if (currentMillis - ultraHigh_previousMillis >= 1000) {

    digitalWrite(triggerPin, HIGH);

  }

  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);  //초음파가 돌아오기까지 걸린 시간을 받아옴
}

// 부저를 울리는 루프
int buzzer_loop() {

  unsigned long currentMillis = millis(); // 현재 시간 1
  unsigned long currentMillis2 = millis();  // 현재 시간 2

  //초음파로 거리 받아오기
  if (currentMillis - buzzerDis_previousMillis >= 2000) { // 1초
    buzzerDis_previousMillis = currentMillis; // 현재시간 = 이전시간
    disBuzzer = 0.01723 * readUltrasonicDistance(13, 12);  //거리 측정
      
  }

  //피에조
  if (currentMillis2 - buzzer_previousMillis >= 2000) { // 1초
    buzzer_previousMillis = currentMillis2;

    if (disBuzzer <= 80 and disBuzzer > 0 ) { //거릿값이 80이하면
      tone(7, 700, 20);  // 부저 소리 울리기
      
      Serial.print("부저 : ");
      Serial.println(disBuzzer);
    }
    else {
      noTone(7);  //부저 소리 끄기
    }
  }
  return disBuzzer;
}


// 진동 모터 루프
int vib_loop() {

  unsigned long currentMillis = millis(); // 현재 시간 1
  unsigned long currentMillis2 = millis();  // 현재 시간 2

  //초음파로 거리 받아오기
  if (currentMillis - vibDis_previousMillis >= 2000) { // 현재시간1 - 이전시간 >= 100
    vibDis_previousMillis = currentMillis; // 현재시간 = 이전시간
    disMotor = 0.01723 * readUltrasonicDistance(11, 10);

    //거리 측정
    //Serial.print("진동초음파 : ");
    //Serial.println(disMotor); //받아온 거리값 출력

  }
  //진동
  if (currentMillis2 - vib_previousMillis >= 2000) {
    vib_previousMillis = currentMillis2;

    if (disMotor <= 80 and disMotor > 0 ) {
      Serial.print("진동 : ");
      Serial.println(disMotor);
     

      analogWrite(9, 80);
    }
    else {
      analogWrite(9, 0);
      //진동 소리 끄기
    }
  }
  return disMotor;
}
