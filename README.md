
# Bota Aquecida
Bota Aquecida
Fernando Moretti, Vinicius Muchuslki e Nikolas Lopes
ENC | Departamento | Centro



Objetivo Geral 
Manter um circuito de aquecimento flexível nas roupas dos funcionários, acoplando um sistema de controle de energia térmico.

História Do Projeto
Uma rede de supermercados enfrenta o desafio de aquecimento dos funcionários que trabalham em ambientes frios, defasando o rendimento e bem-estar dos funcionários.

Looping infinito :

mostrar a temperatura que o usuário colocou no display.
Comparar a temperatura colocada pelo usuário com a do sensor
Caso a temperatura do sensor for menor que a do usuário, ligar aquecedor.
Se  a temperatura do sensor for maior que a do usuário, desligar o aquecedor.

Botões (Display)
-Aumentar 
-Diminuir
-Ok

Sub-rotina para o tratamento de interrupções.
Reseta a flag de interrupção
Passa os valores convertidos do registrador para a variável.
Coloca os valores convertidos no portB em binário.
Testa os valores da conversão em formato decimal.


Função para o calculo da temperatura.
Alimentação em 5 volts.
Conversão de voltagem.
Conversor A/D pois possui uma resolução de 1023 bits.
