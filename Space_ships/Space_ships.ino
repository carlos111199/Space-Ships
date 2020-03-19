/*Código creado por Carlos VL (carlos111199)*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fonts/FreeMonoBold18pt7b.h>//libreria para tipografia incluida en Adafruit GFX

#define SCREEN_WIDTH 128 // Ancho del display OLED en pixeles
#define SCREEN_HEIGHT 64 // Largo del display OLED en pixeles

// Declaracion para el display SSD1306 conectado a I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int movNave=60;//Posicion inicial de la nave principal
int puntuacion=0;//Medidor de la puntuacion
int flag=0; //Determina si se disparó el cañon o no
int row, col; //variables para leer el array naves[][]
int nivel=1;//determina el nivel del jjuego
int posDisparo;//determina la posición desde la que sale disparado el cañon
int altEnemigos=-27;//La altura maxima de las naves invasoras
int pulsoEnter;//Almacena el valor del boton Enter

//Array de las naves
int naves[4][5]={
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1}
};

void setup() {
  //declaracion de los pines 2, 3 y 4 como entradas
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  //determina la posición inicial de la nave principal
  posDisparo=64;
  
  //Inicializa el display, la dirección I2C de la pantalla es 0x3C, reemplaza eb caso de error
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
  }

  display.setFont(&FreeMonoBold18pt7b);
  display.clearDisplay();             
  display.setTextColor(WHITE);        
  display.setCursor(10,20);             
  display.print("SPACE");
  display.setCursor(10,50);
  display.print("SHIPS");
  display.setFont();
  display.setTextSize(1);
  display.setCursor(0,56);
  display.print("by Carlos VL");
  display.display();
  delay(3500);
  pantallaNivel();
}

void loop() {
  //display.clearDisplay();
  vuelo();
  display.display(); 
}

void vuelo(){
  int distDisparo;//distDisparo determina la distancia del disparo del cañon
  int ciclo;//ciclo determina el tiempo en que las naves dan la vuelta
  int posEnemigos=12;//determina posicion inical de las naves enemigas respecto al ancho de la pantalla
  navesEnemigas(posEnemigos, altEnemigos);//dibuja las naves enemigas del array
  for(ciclo=60; ciclo>0; ciclo--){
    display.clearDisplay();
    puntos();//Dibuja el score en pantalla
    if(((ciclo%5)==0) && (ciclo>=30)){
      posEnemigos--;//movimiento de naves a la izquiera
    }
    if(((ciclo%5)==0) && (ciclo<30)){
      posEnemigos++;//movimiento de naves a la derecha
    }
    navesEnemigas(posEnemigos, altEnemigos);
    mainShip();//Movimiento de la nave principal
    if(flag==0){//si no se ha disparado el cañon
      int disparo=shoot();//la funcion shoot retorna 1 o 0
      if(disparo==1){
        flag=1;
        distDisparo=60;
        posDisparo=movNave+4;
      }
    }
    if(flag==1){//si se disparó el cañon
      display.fillRect(posDisparo, distDisparo, 1, 3, WHITE);//dibuja el disparo
      distDisparo--;//el disparo avanza
      if(distDisparo==0){//si el disparo llega al final de la pantalla
        distDisparo=60;
        flag=0;
      }
      impacto(posEnemigos, altEnemigos, distDisparo);//determina si el disparo impactó con una nave
    }
    display.display();
  }
  if(ciclo==0){//Si acaba una vuelta
    altEnemigos=altEnemigos+nivel;//la altura de los enemigos reduce respecto al nivel
    navesEnemigas(posEnemigos, altEnemigos);
    estado();//determina si pasaste o perdiste el nivel
  }
}

int shoot(){
  pulsoEnter=digitalRead(3);//lee el pulso en X
  if(pulsoEnter==HIGH){//Si el pulso es alto, retorna 1
    return 1;
  }
  else{//sino retorna 0
    return 0;
  }
}

void mainShip(){
  int pulsoI, pulsoD;//Variable para almacenar los pulsos

  //lectura de los pulsos de los botones derecha/izquierda
  pulsoI=digitalRead(2);
  pulsoD=digitalRead(4);
  if(pulsoI==HIGH){
    if(movNave>0){
      movNave--;//movimiento de nave principal a la izquierda
    }
  }
  if(pulsoD==HIGH){
    if(movNave<119){
      movNave++;//movimiento de nave principal a la derecha
    }
  }
  display.fillRect(movNave+3, 58, 3, 3, WHITE);
  display.fillRect(movNave, 61, 9, 3, WHITE);
}

void puntos(){
  display.setCursor(0,57);
  display.setTextSize(1);             
  display.print("Score:");
  display.print(puntuacion);
}

void navesEnemigas(int posEnemiga, int altEnemiga){
  int posInicial;
  posInicial=posEnemiga;
  for(col=0; col<4; col++){
    for(row=0; row<5; row++){
      if(naves[col][row]==1){
        display.fillRect(posEnemiga, altEnemiga, 12, 5, WHITE);
        display.fillRect(posEnemiga+4, altEnemiga-3, 4, 3, WHITE);
        display.fillRect(posEnemiga, altEnemiga+5, 2, 2, WHITE);
        display.fillRect(posEnemiga+10, altEnemiga+5, 2, 2, WHITE);
      }
      posEnemiga=posEnemiga+25;
    }
    posEnemiga=posInicial;
    altEnemiga=altEnemiga+15;
  }
}

void impacto(int posEnemigos, int altEnemigo, int distDisparo){
  int cuadrante1;
  int cuadrante2=altEnemigo;
  for(col=0; col<4; col++){
    cuadrante1=posEnemigos;
    for(row=0; row<5; row++){
      if(naves[col][row]==1){
        if((posDisparo>=cuadrante1 && posDisparo<=cuadrante1+12) && (distDisparo>=cuadrante2 && distDisparo<=cuadrante2+5)){
          naves[col][row]=0;
          puntuacion=puntuacion+10;
          distDisparo=60;
          flag=0;
        }
      }
      cuadrante1=cuadrante1+25;
    }
    cuadrante1=12;
    cuadrante2=cuadrante2+15;
  }
}

void estado(){
  int cantNaves, altNaves=0;//Variables auxiliares para contar la altura de las naves respecto al suelo
  for(col=0; col<4; col++){
    for(row=0; row<5; row++){
      if(naves[col][row]==1){
        cantNaves++;
      }
    }
    if(cantNaves>0){
      altNaves++;
      cantNaves=0;
    }
  }
  if(altNaves==4){
    if((altEnemigos+51)>=64){
      gameOver();
      reset();
      pantallaNivel();
    }
  }
  if(altNaves==3){
    if((altEnemigos+36)>=64){
      gameOver();
      reset();
      pantallaNivel();
    }
  }
  if(altNaves==2){
    if((altEnemigos+21)>=64){
      gameOver();
      reset();
      pantallaNivel();
    }
  }
  if(altNaves==1){
    if((altEnemigos)>=64){
      gameOver();
      reset();
      pantallaNivel();
    }
  }
  if(altNaves==0){
    nextLevel();
    pantallaNivel();
  }
  altNaves=0;
}

void gameOver(){//Muestra la pantalla de juego terminado
  display.clearDisplay();
  display.setCursor(10,10);
  display.setTextSize(2);            
  display.println("GAME OVER");
  display.setTextSize(1);
  display.setCursor(35,30);
  display.print("Score: ");
  display.print(puntuacion);
  display.setCursor(2,50);
  display.println("Press X to play again");
  display.display();
  pulsoEnter=digitalRead(3);
  while(!pulsoEnter){//No reinicia el juego hasta que no presinas el boton X
    pulsoEnter=digitalRead(3);
  }
}

void reset(){//reinicia todas las variables del juego
  altEnemigos=-27;
  nivel=1;
  puntuacion=0;
  movNave=60;
  posDisparo=64;
  for(col=0; col<4; col++){
    for(row=0; row<5; row++){
      naves[col][row]=1;
    }
  }
}

void nextLevel(){//Incrementa el nivel de dificultad y reinicia variables
  nivel++;
  if(nivel>=5){
    altEnemigos=-42;
  }
  else{
    altEnemigos=-27; 
  }
  movNave=60;
  posDisparo=64;
  for(col=0; col<4; col++){
    for(row=0; row<5; row++){
      naves[col][row]=1;
    }
  }
}

void pantallaNivel(){//Muestra la pantalla del nivel actual
  display.clearDisplay();
  display.setCursor(25,20);
  display.setTextSize(2);            
  display.print("LEVEL ");
  display.print(nivel);
  display.setTextSize(1);
  display.setCursor(17,40);
  display.print("Ready Player One");
  display.display();
  delay(2000);
}
