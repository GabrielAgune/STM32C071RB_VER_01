#include "relato.h"

Config_Aplicacao_t config_snapshot;



const char Ejeta[] = "================================\n\r" "\n\r"  "\n\r"  "\n\r ";
const char Dupla[] = "\n\r================================\n\r";
const char Linha[] = "--------------------------------\n\r";


void Who_am_i(void)
{
	Gerenciador_Config_Get_Config_Snapshot(&config_snapshot);
	
  printf(Dupla);
  printf("         G620_Teste_Gab\n\r");
  printf("     (c) GEHAKA, 2004-2025\n\r");
  printf(Linha);
  printf("CPU      =           STM32C071RB\n\r");
  printf("Firmware = %21s\r\n", FIRMWARE);
  printf("Hardware = %21s\r\n", HARDWARE);
  printf("Serial   = %21s\r\n", config_snapshot.nr_serial);
  printf(Linha);
  printf("Medidas  = %21d\n\r", 22);
  printf(Ejeta);
}

void Assinatura(void)
{
	uint8_t hours, minutes, seconds;
	uint8_t day, month, year; 
	
	printf("\n\r");
  printf("\n\r");
  printf(Linha);
	if (RTC_Driver_GetTime(&hours, &minutes, &seconds))
	{
		printf("Assinatura              %02d:%02d:%02d\n\r", hours, minutes, seconds);
	}
	if (RTC_Driver_GetDate(&day, &month, &year))
	{
		printf("Responsavel             %02d/%02d/%02d\n\r", day, month, year);
	}
	printf ("\n\r");
  printf ("\n\r");
  printf ("\n\r");
  printf ("\n\r");
}

void Cabecalho(void)
{
	Gerenciador_Config_Get_Config_Snapshot(&config_snapshot);
	
  printf(Dupla);
 	printf("GEHAKA            G620_Teste_Gab\n\r");
  printf(Linha);
	printf("Versao Firmware= %15s\n\r", FIRMWARE);
 	printf("Numero de Serie= %15s\n\r", config_snapshot.nr_serial);
  printf(Linha);
}

void Relatorio_Printer (void)
{	
		Gerenciador_Config_Get_Config_Snapshot(&config_snapshot);
		DadosMedicao_t medicao_snapshot;
		Medicao_Get_UltimaMedicao(&medicao_snapshot);
		const Config_Grao_t* dados_grao_ativo = &config_snapshot.graos[config_snapshot.indice_grao_ativo];

		
    Cabecalho();
  
    printf("Produto       = %16s\n\r",  dados_grao_ativo->nome);
  	printf("Versao Equacao= %10lu\n\r",   (unsigned long)dados_grao_ativo->id_curva);
  	printf("Validade Curva= %13s\n\r", dados_grao_ativo->validade);
  	printf("Amostra Numero= %8i\n\r",      4);
  	printf("Temp.Amostra .= %8.1f 'C\n\r", 22.0);
  	printf("Temp.Instru ..= %8.1f 'C\n\r", medicao_snapshot.Temp_Instru);
  	printf("Peso Amostra .= %8.1f g\n\r", medicao_snapshot.Peso);
  	printf("Densidade ....= %8.1f Kg/hL\n\r",  medicao_snapshot.Densidade);
    printf(Linha);         
  	printf("Umidade ......= %14.*f %%\n\r", (int)config_snapshot.nr_decimals, medicao_snapshot.Umidade);
  	printf(Linha);

  	Assinatura();
}
