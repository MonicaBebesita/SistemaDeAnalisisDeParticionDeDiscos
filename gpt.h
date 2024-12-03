/**
 * @file gpt.h
 * @author 
 * Erwin Meza Vega <emezav@unicauca.edu.co>
 * Julián Alejandro Muñoz Pérez <julianalejom@unicauca.edu.co>
 * @brief Definiciones para discos inicializados con esquema GPT (GUID Partition Table).
 * 
 * Este archivo contiene las definiciones de estructuras, constantes y funciones necesarias
 * para manejar discos que utilizan el esquema de partición GPT. Incluye representaciones
 * de GUIDs, encabezados GPT y descriptores de particiones.
 * 
 * @copyright MIT License
 */

#ifndef GPT_H
#define GPT_H

#include "mbr.h"
/**
 * @struct guid
 * @brief Representación de un GUID (Globally Unique Identifier).
 * 
 * Un GUID es un identificador único utilizado en particiones GPT y otros contextos.
 * Para más detalles, consulte la especificación UEFI: 
 * https://uefi.org/specs/UEFI/2.10/Apx_A_GUID_and_Time_Formats.html
 * 
 * @var guid::time_lo
 * Campo bajo del timestamp.
 * @var guid::time_mid
 * Campo medio del timestamp.
 * @var guid::time_hi_and_version
 * Campo alto del timestamp y versión.
 * @var guid::clock_seq_hi_and_reserved
 * Campo alto de la secuencia de reloj.
 * @var guid::clock_seq_lo
 * Campo bajo de la secuencia de reloj.
 * @var guid::node
 * Identificador único espacial (6 bytes).
 */
typedef struct {
	unsigned int time_lo;                   ///< Campo bajo del timestamp.
    unsigned short time_mid;                ///< Campo medio del timestamp.
    unsigned short time_hi_and_version;     ///< Campo alto del timestamp y versión.
    unsigned char clock_seq_hi_and_reserved;///< Campo alto de la secuencia de reloj.
    unsigned char clock_seq_lo;             ///< Campo bajo de la secuencia de reloj.
    unsigned char node[6];                  ///< Identificador único espacial.
}__attribute__((packed))guid;


/**
 * @struct gpt_header
 * @brief Encabezado de la tabla GPT.
 * 
 * Representa el encabezado de una tabla de particiones GPT. 
 * Contiene información sobre el esquema y la tabla de particiones.

 */
typedef struct {
    unsigned char signature[8];                  ///< Firma GPT: "EFI PART" (8 bytes)
    unsigned short revision;                      ///< Versión (0x00010000 para GPT v1.0)
    unsigned short header_size;                   ///< Tamaño del encabezado GPT (generalmente 92 bytes)
    unsigned short header_crc32;                  ///< CRC32 del encabezado
    unsigned short reserved;                      ///< Reservado, debe ser 0
    unsigned long long current_lba;               ///< LBA del encabezado GPT actual
    unsigned long long backup_lba;                ///< LBA del encabezado GPT de respaldo
    unsigned long long first_usable_lba;          ///< Primer LBA usable (generalmente 34)
    unsigned long long last_usable_lba;           ///< Último LBA usable
    guid partition_entry_type_guid;               ///< GUID de tipo de partición (comúnmente "8DA63203-7B5A-4F9F-B21B-5F8B2D6B920E")
    unsigned long long partition_entries_lba;     ///< LBA de la tabla de particiones (comienza en LBA 2)
    unsigned int num_partition_entries;           ///< Número de entradas de partición
    unsigned int partition_entry_size;            ///< Tamaño de cada descriptor de partición
    unsigned short partition_entry_array_crc32;   ///< CRC32 de la tabla de particiones
    unsigned short reserved2;                     ///< Reservado
} __attribute__((packed)) gpt_header;





/**
 * @struct gpt_partition_descriptor
 * @brief Descriptor de partición GPT.
 * 
 * Representa una entrada en la tabla de particiones GPT. 
 * Cada entrada describe una partición en el disco.
 */
typedef struct {
    guid partition_type_guid;             ///< GUID que identifica el tipo de partición (por ejemplo, "4F68BCE3-E8CD-4A90-A3A2-3B7B05A8A98B" para una partición de datos)
    guid unique_partition_guid;           ///< GUID único para la partición (diferente para cada partición)
    unsigned long long starting_lba;      ///< LBA de inicio de la partición
    unsigned long long ending_lba;        ///< LBA final de la partición
    unsigned long long attributes;        ///< Atributos de la partición (flags, como si es de arranque o no)
    char partition_name[72];              ///< Nombre de la partición (en formato Unicode, 72 caracteres)
} __attribute__((packed)) gpt_partition_descriptor;


/**
 * @brief imprime la tabla de particiones de gpt
 * @param partitions vector que describe los elementos importantes del descriptor de particiones de gpt
 * @param num_paritions es el numero de particiones de gpt encontradas
 */
void print_gpt_partition_table(gpt_partition_descriptor partitions[], int num_partitions);


/**
 * @struct gpt_partition_type
 * @brief Tipo de partición GPT.
 * 
 * Contiene información descriptiva sobre un tipo de partición GPT.
 * 
 * @var gpt_partition_type::os
 * Sistema operativo asociado al tipo de partición.
 * @var gpt_partition_type::description
 * Descripción textual del tipo de partición.
 * @var gpt_partition_type::guid
 * GUID asociado al tipo de partición.
 */
typedef struct {
	const char *os;           ///< Sistema operativo asociado.
    const char *description;  ///< Descripción del tipo de partición.
    const char *guid;         ///< GUID del tipo de partición.
} gpt_partition_type;


/**
 * @brief Obtiene la descripción de un tipo de partición GPT.
 * 
 * Devuelve información descriptiva de un tipo de partición GPT basado en su GUID.
 * 
 * @param guid_str GUID de la partición en formato de cadena.
 * @return Puntero a una estructura `gpt_partition_type` con la descripción del tipo de partición.
 */
const gpt_partition_type* get_gpt_partition_type(char * guid_str);

/**
 * @brief Decodifica el nombre de una partición GPT.
 * 
 * Convierte un nombre de partición codificado en dos bytes por carácter 
 * en una cadena legible por humanos.
 * 
 * @param name Nombre de la partición codificado (72 bytes).
 * @return Puntero a una cadena con el nombre decodificado.
 */
char *gpt_decode_partition_name(char name[72]);

/**
 * @brief Verifica si un sector de arranque es un MBR protector.
 * 
 * Un MBR protector indica la presencia de una tabla de particiones GPT.
 * 
 * @param boot_record Sector de arranque leído en memoria.
 * @return 1 si el sector es un MBR protector, 0 en caso contrario.
 */
int is_protective_mbr(mbr *boot_record);

/**
 * @brief Verifica si un encabezado GPT es válido.
 * 
 * Comprueba la validez del encabezado de la tabla GPT según las especificaciones.
 * 
 * @param hdr Puntero al encabezado GPT.
 * @return 1 si el encabezado es válido, 0 en caso contrario.
 */
int is_valid_gpt_header(gpt_header *hdr);

/**
 * @brief Verifica si un descriptor de partición GPT está vacío.
 * 
 * Un descriptor se considera vacío si su GUID de tipo de partición es nulo.
 * 
 * @param desc Descriptor de partición.
 * @return 1 si el descriptor está vacío, 0 en caso contrario.
 */
int is_null_descriptor(gpt_partition_descriptor *desc);

/**
 * @brief Crea una representación legible de un GUID.
 * 
 * Convierte un GUID en su representación textual en formato estándar.
 * 
 * @param buf Buffer que contiene el GUID.
 * @return Puntero a una nueva cadena con la representación textual del GUID.
 */
char *guid_to_str(guid *buf);

#endif