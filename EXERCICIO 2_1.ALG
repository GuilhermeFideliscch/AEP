algoritmo "exercicio 2"
var
   ma: vetor[1..5, 1..4] de real
   l,c: inteiro
   med:real
   funcao mediapreco(a:real):real
   var
   medias: vetor[1..5] de real
   l,c:inteiro
   soma,media:real
   inicio
   para l de 1 ate 5 faca
      soma <- 0
      para c de 1 ate 4 faca
         soma <- soma + a
      fimpara
      a <- soma / 4
      medias[l] <- a
   fimpara
   escreval("M�dias dos pre�os por regi�o: ")
   para l de 1 ate 5 faca
      escreval("Regi�o ", l, ": ", medias[l]:4:2)
      fimpara
      fimfuncao
inicio

   para l de 1 ate 5 faca
      para c de 1 ate 4 faca
         escreva("Digite o pre�o do produto ", l, " na regi�o ", c, ": ")
         leia(ma[l, c])
      fimpara
   fimpara
   med<-mediapreco(ma[l,c])
   escreva(med)
fimalgoritmo
