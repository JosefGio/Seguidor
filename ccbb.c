
int s_frontal = A0;
int s_esq = A1;
int s_dir = A2;
int s_baixo_esq = 2;
int s_baixo_dir = 3;
int microstart = 4;
int m_pwmesq = 5;
int m_pwmdir = 6; 
int m_esq_direcaoA = 7;
int m_esq_direcaoB = 8;
int m_dir_direcaoA = 9;
int m_dir_direcaoB = 10; 


// motor esquerdo
//direcao a== frente
//direcao b== tras

// motor direito
//direcao a== frente
//direcao b== tras

// os sensores de borda possuem logica invertida



void setup()
{
  Serial.begin(9600);

  

  pinMode(s_frontal, INPUT);  // frontal
  pinMode(s_esq, INPUT);  // esquerda
  pinMode(s_dir, INPUT);  // direita
  pinMode(s_baixo_esq, INPUT);   // borda esquerda
  pinMode(s_baixo_dir, INPUT);   // borda direito 

  pinMode(microstart, INPUT);   // microstart  
  
  pinMode(m_pwmesq, OUTPUT);  // pwm a motor esquerdo
  pinMode(m_pwmdir, OUTPUT);  // pwm b motor direito 
  pinMode(m_esq_direcaoA, OUTPUT);  // in1a 
  pinMode(m_esq_direcaoB, OUTPUT);  // in2a
  pinMode(m_dir_direcaoA, OUTPUT);  // in1b
  pinMode(m_dir_direcaoB, OUTPUT); // in2b
  
  pinMode(13, OUTPUT); // led



 /* while(digitalRead(microstart) == 0){
 
        digitalWrite(m_esq_direcaoA, LOW);
        digitalWrite(m_esq_direcaoB, HIGH);
        digitalWrite(m_dir_direcaoA, LOW);
        digitalWrite(m_dir_direcaoB, HIGH);
        analogWrite(m_pwmesq, 0);
        analogWrite(m_pwmdir, 0);
        digitalWrite(13,LOW);
  }*/
}

  void frente(void){
    digitalWrite(m_esq_direcaoB, LOW);
    digitalWrite(m_esq_direcaoA, HIGH);
    digitalWrite(m_dir_direcaoA, HIGH);
    digitalWrite(m_dir_direcaoB, LOW);
    analogWrite(m_pwmesq, 76);
    analogWrite(m_pwmdir, 80);
  }

  void frente_full(void){
  digitalWrite(m_esq_direcaoB, LOW);
  digitalWrite(m_esq_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoB, LOW);
  analogWrite(m_pwmesq, 136); // diferença dos motores
  analogWrite(m_pwmdir, 130);
  }
  
  void tras(void){
  digitalWrite(m_esq_direcaoB, HIGH);
  digitalWrite(m_esq_direcaoA, LOW);
  digitalWrite(m_dir_direcaoA, LOW);
  digitalWrite(m_dir_direcaoB, HIGH);
  analogWrite(m_pwmesq, 66);
   analogWrite(m_pwmdir, 90);
  }

  void esquerda(void){
  digitalWrite(m_esq_direcaoB, HIGH);
  digitalWrite(m_esq_direcaoA, LOW);
  digitalWrite(m_dir_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoB, LOW);
  analogWrite(m_pwmesq, 66);
  analogWrite(m_pwmdir, 90);
  }

  void direita(void){
  digitalWrite(m_esq_direcaoB, LOW);
  digitalWrite(m_esq_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoA, LOW);
  digitalWrite(m_dir_direcaoB, HIGH);
  analogWrite(m_pwmesq, 66);
  analogWrite(m_pwmdir, 90);
  }

  void rot_antih ()
  {
    digitalWrite(m_esq_direcaoB, LOW);
    digitalWrite(m_esq_direcaoA, HIGH);
    digitalWrite(m_dir_direcaoA, LOW);
    digitalWrite(m_dir_direcaoB, HIGH);
    analogWrite(m_pwmesq, 66);
    analogWrite(m_pwmdir, 66);
    delay (100);
  }
  void rot_h()
  {
      
    digitalWrite(m_esq_direcaoB, HIGH);
    digitalWrite(m_esq_direcaoA, LOW);
    digitalWrite(m_dir_direcaoA, HIGH);
    digitalWrite(m_dir_direcaoB, LOW);
    analogWrite(m_pwmesq, 66);
    analogWrite(m_pwmdir, 66);
    delay (100);
  }
  
  void parar (void)
  {
  digitalWrite(m_esq_direcaoB, HIGH);
  digitalWrite(m_esq_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoA, HIGH);
  digitalWrite(m_dir_direcaoB, HIGH);
  analogWrite(m_pwmesq, 0);
  analogWrite(m_pwmdir, 0);  
  }

  void borda_esquerda(void){
    parar();
    delay(200);
    tras(); 
    delay(200);
    direita();
    delay(400);
    frente();
    delay(200);
  
  }

  void borda_direita(void){
    parar();
    
    delay(200);
    tras(); 
    delay(200);
    esquerda();
    delay(400);
    frente();
    delay(200);
  
  }

//  void borda_teste_esquerda(void){
    
  //}
  
void loop(){
  if(digitalRead(microstart)){
    frente();
    delay (100);
    if(digitalRead(s_esq)){
        rot_h ();
        while(s_esq >0)
        {
          direita();
        }
      }
      else if(digitalRead(s_dir)){
        rot_antih();
        while(s_dir>0)
        {
          esquerda();
        }
        }


      if(digitalRead(s_baixo_esq)){
        borda_esquerda();
        }
      if(digitalRead(s_baixo_dir)){
          borda_direita();
  }
  }
}
  

 
// else if (digitalRead(s_baixo_dir) && digitalRead(s_baixo_esq)){  
//    if (digitalRead(s_frontal)) {frente_full();}
//    else if(!digitalRead(s_frontal)) {
//
//      if      (digitalRead(s_esq)  && !digitalRead(s_dir)){esquerda();}
//      else if (!digitalRead(s_esq) &&  digitalRead(s_dir)){direita();}
//      //else if (!digitalRead(s_esq) && !digitalRead(s_dir)){frente();}
    
// } 
