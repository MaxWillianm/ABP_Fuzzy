#include <Fuzzy.h>


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
  // Set the Serial output
  Serial.begin(9600);
  // Set a random seed
  randomSeed(analogRead(0));

  // Every setup must occur in the function setup()

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
}

void loop()
{
  // get random entrances
  int inputAltura = random(0, 10);
  float inputPeso = random(32767) / 32767.0;


  Serial.println("____________________________________________________________\n");

  Serial.print("Altura: ");
  Serial.print(inputAltura);

  Serial.print(", Peso: ");
  Serial.println(inputPeso);
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

  // wait 1 seconds
  delay(1000);
}
