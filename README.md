# central_irrigacao_arduino
Central de Irrigação para 4 solenóides, 1 bomba e 1 sensor de chuva.

---

Lista de componentes
- Fonte de alimentação 9V - Utilizada para converter corrente alternada (CA) em cor-
rente contínua (CC) para alimentar o Arduíno
- Tomada AC Macho - Alimenta com CA a fonte de CC e os relés
- Módulo Display Oled - Exibe data, hora e a programação da central
- Sensor de Chuva - Detecta a presença de água
- Módulo Relé com 1 canal - Atuador responsável por ativar a bomba d’água no sistema
de irrigação
- Bloco terminal com 12 posições - Utilizado para conectar as solenoides e bomba aos relés
- Módulo Relé com 4 canais - Atuador responsável por ativar as válvulas solenóides
- Shield Data Logger - Possui um RTC (Relógio de Tempo Real) e um leitor de
cartão SD, utilizado para armazenar as configurações e
manter o controle do tempo
- Arduino UNO - Controla todos os componentes
- Placa Perfurada - Usada para montagem manual de circuitos eletrônicos
durante a prototipagem
- Push button redondo Botões para operação da central

---

![Circuito CC](https://raw.githubusercontent.com/Freitasx021/central_irrigacao_arduino/refs/heads/main/circuito_dc.png)

![Circuito CA](https://raw.githubusercontent.com/Freitasx021/central_irrigacao_arduino/refs/heads/main/circuito_ac.png)
