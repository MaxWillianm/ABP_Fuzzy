#include <Fuzzy.h>
#include <NewPing.h>
#include "HX711.h"

// Define os pinos para o sensor ultrassônico e a distância máxima para medir (em centímetros)
#define TRIGGER_PIN  24
#define ECHO_PIN     26
#define MAX_DISTANCE 200

// Define os pinos para o sensor da balança
#define SCK A0
#define DT A1

// cria uma instancia da biblioteca HX711 na variavel escala para o sensor da balança
HX711 escala;

// Cria uma instância do objeto NewPing para o ultrassonico
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Fuzzy
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput funções de pertinencia
FuzzySet *alturaBaixo = new FuzzySet(0, 3.33, 3.33, 6.66);
FuzzySet *alturaMedio = new FuzzySet(3.33, 6.66, 6.66, 10);
FuzzySet *alturaAlto = new FuzzySet(6.66, 10, 10, 10);

// FuzzyInput funções de pertinencia
FuzzySet *pesoBaixo = new FuzzySet(0, 0.33, 0.33, 0.66);
FuzzySet *pesoMedio = new FuzzySet(0.33, 0.66, 0.66, 1);
FuzzySet *pesoAlto = new FuzzySet(0.66, 1, 1, 1);

// FuzzyOutput
FuzzySet *ledBaixo = new FuzzySet(0, 0, 0.5, 1);
FuzzySet *ledMedio = new FuzzySet(0.5, 1, 3, 4);
FuzzySet *ledAlto = new FuzzySet(3, 4, 5, 5);


void setup()
{

  // define o pinMode de cada led

  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);

  // desliga os led caso iniciem ligados
  digitalWrite(2, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(4, HIGH);
  digitalWrite(5, HIGH);
  digitalWrite(6, HIGH);

  // liga o serial
  Serial.begin(9600);
  // Set a random seed
  randomSeed(analogRead(0));

  // FuzzyInput
  FuzzyInput *altura = new FuzzyInput(1);

  altura->addFuzzySet(alturaBaixo);
  altura->addFuzzySet(alturaMedio);
  altura->addFuzzySet(alturaAlto);
  fuzzy->addFuzzyInput(altura);

  // FuzzyInput
  FuzzyInput *peso = new FuzzyInput(2);

  peso->addFuzzySet(pesoBaixo);
  peso->addFuzzySet(pesoMedio);
  peso->addFuzzySet(pesoAlto);
  fuzzy->addFuzzyInput(peso);

  // FuzzyOutput
  FuzzyOutput *led = new FuzzyOutput(1);

  led->addFuzzySet(ledBaixo);
  led->addFuzzySet(ledMedio);
  led->addFuzzySet(ledAlto);
  fuzzy->addFuzzyOutput(led);


  // Building FuzzyRule
  // Criar regra de antecedente passando o nome da regra e iniciando a instancia do fuzzy
  FuzzyRuleAntecedent *altura_altaAndpeso_alto = new FuzzyRuleAntecedent();
  // passar valores para a regra
  altura_altaAndpeso_alto->joinWithAND(alturaAlto, pesoAlto);
  // cria a regra de consequencia passando o nome da regra e iniciando a instancia do fuzzy
  FuzzyRuleConsequent *led_alto = new FuzzyRuleConsequent();
  // passa o valor de saida consequente
  led_alto->addOutput(ledAlto);
  FuzzyRule *fuzzyRule1 = new FuzzyRule(1, altura_altaAndpeso_alto, led_alto);
  fuzzy->addFuzzyRule(fuzzyRule1);


  // Building FuzzyRule
  // Criar regra de antecedente passando o nome da regra e iniciando a instancia do fuzzy
  FuzzyRuleAntecedent *altura_medioAndpeso_baixo = new FuzzyRuleAntecedent();
  // passar valores para a regra
  altura_medioAndpeso_baixo->joinWithAND(alturaMedio, pesoBaixo);
  // cria a regra de consequencia passando o nome da regra e iniciando a instancia do fuzzy
  FuzzyRuleConsequent *led_baixo = new FuzzyRuleConsequent();
  // passa o valor de saida consequente
  led_baixo->addOutput(ledBaixo);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, altura_medioAndpeso_baixo, led_baixo);
  fuzzy->addFuzzyRule(fuzzyRule2);



  // inicia a instancia 'escala'
  escala.begin (DT, SCK);
  // começa a leitura e calculos para iniciar a pesagem
  Serial.print("Leitura do Valor ADC:  ");
  Serial.println(escala.read());   // Aguarda até o dispositivo estar pronto
  Serial.println("Nao coloque nada na balanca!");
  Serial.println("Iniciando...");
  escala.set_scale(765991.129);     // Substituir o valor encontrado para escala
  escala.tare(20);                // O peso é chamado de Tare.
  Serial.println("Insira o item para Pesar");

}

void loop()
{


  // SENSOR ULTRASSONICO
  // Faz uma leitura da distância em centímetros
  unsigned int distance = sonar.ping_cm();
  // Faz o calculo da altura do objeto na plataforma de medição
  int distancia = 10 - distance;

  // VARIAVEL "DISTANCIA" GUARDA O VALOR DA ALTURA DO OBJETO NA PLATAFORMA
  // Serial.print("Altura: ");
  // Serial.print(distancia);
  // Serial.println("cm");

  // SENSOR DA BALANÇA
  // Serial.print("Peso: ");
  // Serial.println(escala.get_units(20), 3);


  // get random entrances
  int inputAltura = distancia;
  float inputPeso = escala.get_units(20);


  Serial.println("____________________________________________________________\n");

  Serial.print("Altura: ");
  Serial.print(inputAltura);

  Serial.print(", Peso: ");
  Serial.println(inputPeso, 3);
  Serial.println();

  fuzzy->setInput(1, inputAltura);
  fuzzy->setInput(2, inputPeso);
  fuzzy->fuzzify();

  Serial.print("Altura: ");
  Serial.print("Baixo-> ");
  Serial.print(alturaBaixo->getPertinence());
  Serial.print(" |  Medio-> ");
  Serial.print(alturaMedio->getPertinence());
  Serial.print(" | Alto-> ");
  Serial.println(alturaAlto->getPertinence());

  Serial.print("Peso: ");
  Serial.print("  Baixo-> ");
  Serial.print(pesoBaixo->getPertinence());
  Serial.print(" |  Medio-> ");
  Serial.print(pesoMedio->getPertinence());
  Serial.print(" | Alto-> ");
  Serial.println(pesoAlto->getPertinence());

  float outputLED = fuzzy->defuzzify(1);
  // float output2 = fuzzy->defuzzify(2);

  Serial.print("LED: ");
  Serial.print("Baixo-> ");
  Serial.print(ledBaixo->getPertinence());
  Serial.print(" |  Medio-> ");
  Serial.print(ledMedio->getPertinence());
  Serial.print(" |  Alto-> ");
  Serial.println(ledAlto->getPertinence());

  Serial.println("Resultado: ");
  Serial.print("LED: ");
  Serial.println(outputLED);

  if(outputLED < 1){
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
  }else if(outputLED > 1 && outputLED < 2){
    digitalWrite(2, LOW);
  }else if(outputLED > 2 && outputLED < 3){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
  }else if(outputLED > 3 && outputLED < 4){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  }else if(outputLED > 4 && outputLED < 5) {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
  }else if(outputLED > 5){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }
  // wait 1 seconds
  delay(1000);
}
