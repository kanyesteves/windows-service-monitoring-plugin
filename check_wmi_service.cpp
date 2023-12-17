/*
 * Project name: Sensor de monitoramento para serviços Windows;
 * Description: O sensor faz monitoramento dos serviços em execução e/ou parado do Windows usando WMI para as consultas desssas informações;
 * language programing: C++;
 * License: https://www.gnu.org/licenses/gpl-3.0.txt;
 * Author: Kanydian Esteves Canedo <kany.esteves@gmail.com>;
 * Date: 2023/12/13;
*/

#include <iostream>
#include <string>

string debug_file = "/var/log/check_wmi_service.log";
string status, output;
string args[][4] = {
  {"-H", "--host",       "host",        ""},
  {"-U", "--user",       "user",        ""},
  {"-P", "--passwd",     "passwd",      ""},
  {"-S", "--service",    "service",     ""},
};


// Functions
string help(int EXIT_STATUS); // Função de ajuda para executar o script;
string executeCommand(const char* command); // Executa camando para buscar informações de um serviço específico;
Json::Value _parser(int argc, char *argv[]); // Valida se os parâmetros passados estão corretos;
void _log(string message); // Log em debug_file


int main(int argc, char *argv[]) {
  if (argc < 5) {
    help(1);
  }

  Json::Value params = _parser(argc, argv);

  string user          = params["user"].asString();
  string password      = params["passwd"].asString();
  string host          = params["host"].asString();
  string service_name  = params["service"].asString();
  _log("Iniciando consulta do serviço "+ service_name);

  string cmd = "/usr/bin/sshpass -p '" + password + "' ssh -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no -o ConnectTimeout=3 -o ConnectionAttempts=1 " + user +"@"+ host + " "
                      "\"powershell \\\"Get-WmiObject -Class Win32_Service -Filter \\\\\\\"Name='" + service_name + "'\\\\\\\" | ConvertTo-Json\\\"\"";

  _log("[ CMD ]: "+ cmd);
  string result = executeCommand(cmd.c_str());

  Json::Value json;
  if (!result.size() && status == "ERROR") {
    output = status +": Erro ao executar o comando.";
  }

  stringToJson(result, json);
  string name = json["Name"].asString();
  string state = json["State"].asString();

  if (state == "Running") {
    status = "OK";
    _log("[ STATUS ]: "+ status);
    output = status +": Serviço em execução.";
  } else if (state == "Stopped") {
    status = "CRÍTICO";
    _log("[ STATUS ]: "+ status);
    output = status +": Serviço está parado.";
  } else {
    status = "UNKNOWN";
    _log("[ STATUS ]: "+ status);
    output = status +": Nenhum serviço encontrado.";
  }

  _log("[ OUTPUT ]: "+ output);
  cout << output << endl;

  _log("Finalizado consulta do serviço "+ service_name);
  return 0;
}

string executeCommand(const char* command) {
  _log("Executando comando...");
  FILE* pipe = popen(command, "r");
  if (!pipe)
    status = "ERROR";

  char buffer[128];
  string result = "";

  while (!feof(pipe)) {
    if (fgets(buffer, 128, pipe) != NULL) {
        result += buffer;
    }
  }

  pclose(pipe);
  return result;
}

string help(int EXIT_STATUS) {
  cout << "How to usage:\n "
        "[ -U | --user ]    - User\n "
        "[ -P | --passwd ]  - Password\n "
        "[ -H | --host ]    - Host ip\n "
        "[ -S | --service ] - Name service\n" << endl;
  exit(EXIT_STATUS);
}


Json::Value _parser(int argc, char *argv[]) {
  Json::Value parser;
  for (int i = 0; i < argc; i++) {
    for (unsigned int j = 0, jlen = sizeof(args) / sizeof(args[0]); j < jlen; j++) {
      if ((string(argv[i]) == args[j][0] || string(argv[i]) == args[j][1]) && i + 1 < argc && string(argv[i + 1]).substr(0, 1) != "-") {
        parser[args[j][2]] = string(argv[i + 1]);
        args[j][3] = "loaded";
      }
    }
  }

  string message = "";
  for (int i = 0; i < 4; i++) {
    if (!args[i][3].size()) {
      message = "error: Invalid [ " + args[i][0] + " | " + args[i][1] + " ] " + args[i][2] + " configuration.";
      cout << message << endl;
    }
  }

  return parser;
}

void _log(string message) {
  if (!fileExists(debug_file)) return;
  fstream file;
  file.open(debug_file.c_str(), ios::out);
  file << "[" + cur_date() + "] " << message << endl;
  file.close();
}