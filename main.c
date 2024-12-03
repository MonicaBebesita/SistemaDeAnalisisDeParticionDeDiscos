/**
 * @file main.c
 * @author 
 * Erwin Meza Vega <emezav@unicauca.edu.co>
 * @brief Programa para listar particiones de discos con esquemas MBR/GPT.
 * 
 * Este programa accede directamente a dispositivos de bloque y analiza
 * el primer sector para determinar el esquema de partición. Proporciona
 * herramientas para visualizar los datos en formato hexadecimal y ASCII.
 * 
 * @copyright MIT License
 */


#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#include "mbr.h"
#include "gpt.h"
#define SECTOR_SIZE 512 ///< Tamaño estándar de un sector de disco (512 bytes).

/**
 * @brief Muestra el contenido de un buffer en formato hexadecimal.
 * 
 * Esta función imprime los datos de un buffer en formato hexadecimal
 * y su representación ASCII correspondiente.
 * 
 * @param buf Puntero al buffer que contiene los datos a imprimir.
 * @param size Tamaño del buffer.
 */
void hex_dump(char *buf, size_t size);

/**
 * @brief Muestra el contenido de un buffer en formato ASCII.
 * 
 * Los caracteres no imprimibles son sustituidos por un punto (`.`).
 * 
 * @param buf Puntero al buffer que contiene los datos a imprimir.
 * @param size Tamaño del buffer.
 */
void ascii_dump(char *buf, size_t size);

/**
 * @brief Lee un sector específico de un disco y lo almacena en un buffer.
 * 
 * Esta función accede al disco o dispositivo especificado y lee un sector
 * lógico identificado por el número LBA (Logical Block Address). El contenido
 * del sector leído se almacena en el buffer proporcionado.
 * 
 * @param disk Puntero a un archivo o dispositivo de bloque donde se encuentra el disco.
 * @param lba Número lógico de bloque (LBA) que identifica el sector a leer.
 *            Este es un valor entero que representa la dirección lógica
 *            del sector en el disco.
 * @param buf Buffer de memoria donde se almacenará el contenido del sector leído.
 * 
 * @return int 1 si la lectura fue exitosa, 0 si ocurrió un error.
 */
int read_lba_sector(char *disk, unsigned long long lba, char buf[SECTOR_SIZE]);


int main(int argc, char *argv[]) {

	// 1. Validar los argumentos de línea de comandos
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <dispositivo>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
	char * disk;
	
	// 2.1 Leer el primer sector del disco especificado

	// Iterar sobre los dispositivos pasados como argumentos
	for(int i=1 ; i<argc; i++){
		mbr boot_record; // Estructura para almacenar datos del MBR
		gpt_header hdr;
		disk=argv[i]; // Nombre del archivo o dispositivo actual
		
		printf("\nAnalizando dispositivo: %s\n", disk);
		// 2.1 Leer el primer sector del disco especificado
		// 2.2 Si la lectura falla imprimir error y terminar.
		if(read_lba_sector(disk,0, (char*)&boot_record)==0){
			fprintf(stderr, "Error: No se pudo abrir el dispositivo %s\n", disk);
			continue;//Salta al siguiente dispositivo
		}

		// Imprimir el contenido del primer sector en formato hexadecimal
		printf("Contenido del primer sector del disco:%s:\n", disk);
		hex_dump((char*)&boot_record, sizeof(mbr));
		//PRE: se pudo leer el primer sector del disco
		//3.Imprimir la tabla de particiones MBR leido

		// Paso 3.1Verificar si el MBR es válido
		if (is_mbr(&boot_record)==0) {
			fprintf(stderr, "Advertencia: El sector de arranque del dispositivo %s no contiene una firma válida.\n", disk);
			continue; // Saltar al siguiente dispositivo
		}
		printf("La firma del MBR es válida. Analizando el disco...\n");

		// 4. Si el esquema de particiones es MBR: terminar
		// 4.1 Determinar el esquema de partición (MBR o GPT)
		if (is_mbr(&boot_record)==2) {
            printf("El esquema de partición es GPT. Procediendo a imprimir la tabla GPT...\n");

            unsigned char buffer[SECTOR_SIZE]; // Buffer intermedio

			// Leer el encabezado GPT
			if (read_lba_sector(disk, 1, (char *)buffer) == 0) {
				fprintf(stderr, "Error: No se pudo leer el encabezado GPT en %s\n", disk);
				continue;
			}

			// Copiar el contenido relevante al encabezado GPT
			memcpy(&hdr, buffer, sizeof(gpt_header));

			// Validar el encabezado GPT
			if (!is_valid_gpt_header(&hdr)) {
				fprintf(stderr, "Advertencia: El encabezado GPT en %s no es válido.\n", disk);
				continue;
			}


            printf("Encabezado GPT válido. Analizando particiones...\n");

            unsigned long long partition_entries_lba = hdr.partition_entries_lba;
            unsigned int num_entries = hdr.num_partition_entries;
            unsigned int entry_size = hdr.partition_entry_size;
            printf("Tabla GPT contiene %u descriptores en LBA %llu\n", num_entries, partition_entries_lba);

 			// Crear un arreglo para almacenar las particiones
        	gpt_partition_descriptor partitions[num_entries];
            // Iterar sobre las particiones
            for (unsigned int i = 0; i < num_entries; i++) {
                unsigned long long lba = partition_entries_lba + (i * entry_size) / SECTOR_SIZE;
				// Leer descriptor de partición
				if (read_lba_sector(disk, lba, (char *)buffer) == 0) {
					fprintf(stderr, "Error: No se pudo leer el descriptor %u en %s\n", i, disk);
					break;
				}

			    gpt_partition_descriptor desc;
				
				// Copiar datos al descriptor de partición
				memcpy(&desc, buffer, sizeof(gpt_partition_descriptor));

                if (is_null_descriptor(&desc)) {
                    continue; // Si el descriptor es nulo, continuar con la siguiente partición            
				}

                // Almacenar los datos de la partición en el arreglo
                partitions[i].starting_lba = desc.starting_lba;
                partitions[i].ending_lba = desc.ending_lba;
                partitions[i].partition_type_guid = desc.partition_type_guid;
                // Aquí, asignas un nombre adecuado a la partición
                snprintf(partitions[i].partition_name, sizeof(partitions[i].partition_name), "Partición %u", i + 1);
            }

            // Imprimir la tabla de particiones si todo lo anterior es correcto para GPT
            print_gpt_partition_table(&hdr,partitions, num_entries);
			
		}else {
			printf("El esquema de partición es MBR. Imprimiendo tabla de particiones MBR...\n");
			print_mbr_partition_table(&boot_record);
			
		}
	}
	return 0;
}


int read_lba_sector(char * disk, unsigned long long lba, char buf[SECTOR_SIZE]) {

	/* TODO leer un sector de 512 bytes del disco especificado en el buffer */

	FILE *fp;
	//ABRIR EL DISPOSITIVO EN MODO LECTURA
	fp=fopen(disk, "r");

	if(fp==NULL){
		fprintf(stderr, "Error: No se pudo abrir el archivo o dispositivo %s\n", disk);
        return 0; // Retornar error si el archivo no puede abrirse
    }
// Mover el puntero del archivo al sector solicitado (LBA)

	if (fseek(fp, lba * SECTOR_SIZE, SEEK_SET) != 0){
		fprintf(stderr, "Error: No se pudo posicionar en el sector %llu del dispositivo %s\n", lba, disk);
        fclose(fp);
        return 0; // Retornar error si el posicionamiento falla
    }

// Leer el sector y almacenarlo en el buffer	
	if(fread(buf,1, SECTOR_SIZE,fp)!= SECTOR_SIZE){
		fprintf(stderr, "Error: No se pudo leer el sector %llu del dispositivo %s\n", lba, disk);
        fclose(fp);
        return 0; // Retornar error si la lectura falla
    }

	fclose(fp);
	return 1;  // Lectura exitosa
		//return 0;


}

void ascii_dump(char * buf, size_t size) {
	 // Iterar sobre los bytes del buffer y mostrar su representación ASCII
	for (size_t i = 0; i < size; i++) {
		if (buf[i] >= 0x20 && buf[i] < 0x7F) {
			printf("%c", buf[i]); // Mostrar caracteres imprimibles
		}else {
			printf("."); // Sustituir caracteres no imprimibles con un punto
		}
	}
}

void hex_dump(char * buf, size_t size) {
	int cols=0; //Contador de columnas

	for (size_t i=0; i < size; i++) {
		printf("%02x ", buf[i] & 0xff);//Imprimir cad abyte en formato hexadecimal
		// Imprimir 16 bytes por línea, seguidos de su representación ASCII
		if (++cols % 16 == 0) {
			ascii_dump(&buf[cols - 16], 16);//Imprimir el ASCII de los ultimos 16 bytes
			printf("\n");
		}
	}
}