# Plugin de monitoramento dos serviços do Windows para o NAGIOS

## Problemática
Cliente precisava monitorar os serviços do sistema TOTVS dele através do sistema [SM](https://sm.seti.com.br/) da empresa [Seti Tecnologia](https://www.seti.com.br/).

## Parâmetros para rodar

- Compilar o script.

        $ gcc ./check_wmi_service.cpp -o ./check_wmi_service
- Executar o script.

        $ ./check_wmi_service -U [USER] -P [PASSWORD] -H [HOST] -S [SERVICE NAME]

## NAGIOS
[Nagios](https://www.nagios.org/) é uma ferramenta de monitoramento de código aberto e é distribuída sob a Licença Pública Geral GNU (GNU General Public License, GPL). É usado no [SM](https://sm.seti.com.br/) para fazer o monitoramento de rede e requer dos plugins criados um formato específico para ele ler.

- Formato

        'label'=value[UOM];[warn];[crit];[min];[max]

## O que o plugin retorna para o NAGIOS?
O plugin foi desenvolvido para retornar o status (em execução ou parado) do nome do serviço que foi passado por parâmetro.

- Retorno do script.

        $ OK: Serviço em execução

## Liguagens usadas

- **C++**

## Observações
O script falta algumas libs a serem colocados no `#include` para poder executar o script.