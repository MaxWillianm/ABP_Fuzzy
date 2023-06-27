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

// FuzzyInput
FuzzySet *alturaBaixo = new FuzzySet(0, 0, 2, 4);
FuzzySet *alturaMedio = new FuzzySet(3, 4, 6, 7);
FuzzySet *alturaAlto = new FuzzySet(6, 8, 10, 10);

// FuzzyInput
FuzzySet *pesoBaixo = new FuzzySet(0, 0, 0.2, 0.4);
FuzzySet *pesoMedio = new FuzzySet(0.3, 0.4, 0.6, 0.7);
FuzzySet *pesoAlto = new FuzzySet(0.6, 0.8, 1, 1);

// FuzzyOutput
FuzzySet *ledBaixo = new FuzzySet(0, 0, 0, 1);
FuzzySet *ledMedio = new FuzzySet(1, 2, 3, 4);
FuzzySet *ledAlto = new FuzzySet(4, 5, 5, 5);


void setup(){

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
  // REGRA 1 - ALTURA ALTO E PESO ALTO = LED "ALTO"
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

  // REGRA 2 - ALTURA MEDIO E PESO ALTO = LED "ALTO"
  FuzzyRuleAntecedent *altura_medioAndpeso_alto = new FuzzyRuleAntecedent();
  altura_medioAndpeso_alto->joinWithAND(alturaMedio, pesoAlto);
  FuzzyRuleConsequent *led_alto2 = new FuzzyRuleConsequent();
  led_alto2->addOutput(ledAlto);
  FuzzyRule *fuzzyRule2 = new FuzzyRule(2, altura_medioAndpeso_alto, led_alto2);
  fuzzy->addFuzzyRule(fuzzyRule2);

  // REGRA 3 - ALTURA ALTO E PESO ALTO = LED "ALTO"
  FuzzyRuleAntecedent *altura_altoAndpeso_medio = new FuzzyRuleAntecedent();
  altura_altoAndpeso_medio->joinWithAND(alturaAlto, pesoMedio);
  FuzzyRuleConsequent *led_alto3 = new FuzzyRuleConsequent();
  led_alto3->addOutput(ledAlto);
  FuzzyRule *fuzzyRule3 = new FuzzyRule(3, altura_altoAndpeso_medio, led_alto3);
  fuzzy->addFuzzyRule(fuzzyRule3);

  // REGRA 4 - ALTURA BAIXO E PESO ALTO = LED "ALTO"
  FuzzyRuleAntecedent *altura_baixoAndpeso_alto = new FuzzyRuleAntecedent();
  altura_baixoAndpeso_alto->joinWithAND(alturaBaixo, pesoAlto);
  FuzzyRuleConsequent *led_alto4 = new FuzzyRuleConsequent();
  led_alto4->addOutput(ledAlto);
  FuzzyRule *fuzzyRule4 = new FuzzyRule(4, altura_baixoAndpeso_alto, led_alto4);
  fuzzy->addFuzzyRule(fuzzyRule4);

  // REGRA 5 - ALTURA MEDIO E PESO MEDIO = LED "MEDIO"
  FuzzyRuleAntecedent *altura_medioAndpeso_medio = new FuzzyRuleAntecedent();
  altura_medioAndpeso_medio->joinWithAND(alturaMedio, pesoMedio);
  FuzzyRuleConsequent *led_medio = new FuzzyRuleConsequent();
  led_medio->addOutput(ledMedio);
  FuzzyRule *fuzzyRule5 = new FuzzyRule(5, altura_medioAndpeso_medio, led_medio);
  fuzzy->addFuzzyRule(fuzzyRule5);

  // REGRA 6 - ALTURA BAIXO E PESO MEDIO = LED "MEDIO"
  FuzzyRuleAntecedent *altura_baixoAndpeso_medio = new FuzzyRuleAntecedent();
  altura_baixoAndpeso_medio->joinWithAND(alturaBaixo, pesoMedio);
  FuzzyRuleConsequent *led_medio2 = new FuzzyRuleConsequent();
  led_medio2->addOutput(ledMedio);
  FuzzyRule *fuzzyRule6 = new FuzzyRule(6, altura_baixoAndpeso_medio, led_medio2);
  fuzzy->addFuzzyRule(fuzzyRule6);

  // REGRA 7 - ALTURA ALTO E PESO BAIXO = LED "MEDIO"
  FuzzyRuleAntecedent *altura_altoAndpeso_baixo = new FuzzyRuleAntecedent();
  altura_altoAndpeso_baixo->joinWithAND(alturaAlto, pesoBaixo);
  FuzzyRuleConsequent *led_medio3 = new FuzzyRuleConsequent();
  led_medio3->addOutput(ledMedio);
  FuzzyRule *fuzzyRule7 = new FuzzyRule(7, altura_altoAndpeso_baixo, led_medio3);
  fuzzy->addFuzzyRule(fuzzyRule7);

  // REGRA 8 - ALTURA MEDIO E PESO BAIXO = LED "BAIXO"
  FuzzyRuleAntecedent *altura_medioAndpeso_baixo = new FuzzyRuleAntecedent();
  altura_medioAndpeso_baixo->joinWithAND(alturaMedio, pesoBaixo);
  FuzzyRuleConsequent *led_baixo = new FuzzyRuleConsequent();
  led_baixo->addOutput(ledBaixo);
  FuzzyRule *fuzzyRule8 = new FuzzyRule(8, altura_medioAndpeso_baixo, led_baixo);
  fuzzy->addFuzzyRule(fuzzyRule8);

  // REGRA 9 - ALTURA BAIXO E PESO BAIXO = LED "BAIXO"
  FuzzyRuleAntecedent *altura_baixoAndpeso_baixo = new FuzzyRuleAntecedent();
  altura_baixoAndpeso_baixo->joinWithAND(alturaBaixo, pesoBaixo);
  FuzzyRuleConsequent *led_baixo2 = new FuzzyRuleConsequent();
  led_baixo2->addOutput(ledBaixo);
  FuzzyRule *fuzzyRule9 = new FuzzyRule(9, altura_baixoAndpeso_baixo, led_baixo2);
  fuzzy->addFuzzyRule(fuzzyRule9);

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

void loop(){


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
    Serial.println("Valor da taxa: R$0,00");
  }else if(outputLED >= 1 && outputLED < 2){
    digitalWrite(2, LOW);
    Serial.println("Valor da taxa: R$60,00");
  }else if(outputLED >= 2 && outputLED < 3){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    Serial.println("Valor da taxa: R$120,00");
  }else if(outputLED >= 3 && outputLED < 4){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    Serial.println("Valor da taxa: R$240,00");
  }else if(outputLED >= 4 && outputLED < 4.5) {
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    Serial.println("Valor da taxa: R$480,00");
  }else if(outputLED >= 4.5){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
    Serial.println("Valor da taxa: R$960,00");
  }
  // espera 1 segundo
  delay(1000);
}
